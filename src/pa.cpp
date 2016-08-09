#include "pa.hpp"
#include <cstring>
#include <functional>

// https://freedesktop.org/software/pulseaudio/doxygen/introspect_8h.html

Pa::Pa()
{
    notify_update_cb = nullptr;

    pa_ml = pa_threaded_mainloop_new();
    pa_api  = pa_threaded_mainloop_get_api(pa_ml);
    pa_ctx = pa_context_new(pa_api, "pulsemixer - fulhax");

    pa_threaded_mainloop_lock(pa_ml);
    pa_threaded_mainloop_start(pa_ml);
    pa_context_connect(pa_ctx, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);
    pa_context_set_state_callback(pa_ctx, &Pa::ctx_state_cb, this);
    pa_threaded_mainloop_unlock(pa_ml);


}
Pa::~Pa()
{
    pa_context_disconnect(pa_ctx);
    pa_threaded_mainloop_stop(pa_ml);
    pa_threaded_mainloop_free(pa_ml);
}

void Pa::update_sink(const pa_sink_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);
    PA_SINKS[info->index].channels = info->channel_map.channels;
    PA_SINKS[info->index].volume = (const pa_volume_t) pa_cvolume_avg(
                                       &info->volume);
    PA_SINKS[info->index].mute = info->mute;

    strncpy(PA_SINKS[info->index].name, info->description, 255);

    notify_update();
}

void Pa::update_input(const pa_sink_input_info *info)
{

    std::lock_guard<std::mutex> lk(inputMtx);
    PA_INPUTS[info->index].channels = info->channel_map.channels;
    PA_INPUTS[info->index].volume = (const pa_volume_t) pa_cvolume_avg(
                                        &info->volume);
    PA_INPUTS[info->index].mute = info->mute;
    PA_INPUTS[info->index].sink = info->sink;

    const char *name;
    name = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_NAME);
    strncpy(PA_INPUTS[info->index].name, name, 255);
    notify_update();
}

void Pa::toggle_input_mute(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);
    auto i = PA_INPUTS.find(index);
    if(i != PA_INPUTS.end()){
        pa_operation *o = pa_context_set_sink_input_mute(pa_ctx, index, !i->second.mute, NULL, NULL);
        pa_operation_unref(o);
    }
}

void Pa::set_input_volume(uint32_t index, int dir)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    if(PA_INPUTS.find(index) == PA_INPUTS.end())
    {
        return;
    }
    if (PA_INPUTS[index].volume <= 1000 && dir == -1) {
        return;
    }

    pa_threaded_mainloop_lock(pa_ml);

    int volume = PA_INPUTS[index].volume + (1000 * dir);
    pa_cvolume cvol;

    pa_cvolume_init(&cvol);
    pa_cvolume_set(&cvol, PA_INPUTS[index].channels, volume);
    pa_operation *o = pa_context_set_sink_input_volume(pa_ctx, index, &cvol, NULL,
                      NULL);
    pa_operation_unref(o);

    pa_threaded_mainloop_unlock(pa_ml);

}

void Pa::move_input_sink(uint32_t input_index, uint32_t sink_index)
{
    pa_threaded_mainloop_lock(pa_ml);
    pa_operation *o = pa_context_move_sink_input_by_index(pa_ctx, input_index,
                      sink_index, NULL, NULL);
    pa_operation_unref(o);
    pa_threaded_mainloop_unlock(pa_ml);
}

void Pa::remove_input(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);
    PA_INPUTS.erase(index);
    notify_update();
}

void Pa::remove_sink(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);
    PA_SINKS.erase(index);
}

void Pa::set_notify_update_cb(notify_update_callback cb)
{
    notify_update_cb = cb;
}

void Pa::notify_update()
{
    if (notify_update_cb != nullptr) {
        notify_update_cb();
    }
}

void Pa::ctx_sourcelist_cb(pa_context *ctx, const pa_source_info *info,
                           int eol,
                           void *userdata)
{
    return;
}

void Pa::ctx_inputlist_cb(pa_context *ctx, const pa_sink_input_info *info,
                          int eol,
                          void  *instance)
{


    if (!eol) {
        ((Pa *)instance)->update_input(info);
    }
}

void Pa::ctx_sinklist_cb(pa_context *ctx, const pa_sink_info *info, int eol,
                         void  *instance)
{
    if (!eol) {
        ((Pa *)instance)->update_sink(info);
    }
}

void Pa::subscribe_cb(pa_context *ctx, pa_subscription_event_type_t t,
                      uint32_t index, void *instance)
{

    int type = (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK);


    // https://freedesktop.org/software/pulseaudio/doxygen/def_8h.html#a6bedfa147a9565383f1f44642cfef6a3

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK: {
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                ((Pa *)instance)->remove_sink(index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {

                pa_operation *o = pa_context_get_sink_info_by_index(ctx, index,
                                  &Pa::ctx_sinklist_cb, instance);
                pa_operation_unref(o);
            }

            break;
        }

        case PA_SUBSCRIPTION_EVENT_SINK_INPUT: {

            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                ((Pa *)instance)->remove_input(index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {

                pa_operation *o = pa_context_get_sink_input_info(ctx, index,
                                  &Pa::ctx_inputlist_cb, instance);
                pa_operation_unref(o);
            }

            break;
        }

        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
        case PA_SUBSCRIPTION_EVENT_MODULE:
        case PA_SUBSCRIPTION_EVENT_CLIENT:
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
        case PA_SUBSCRIPTION_EVENT_SERVER:
        case PA_SUBSCRIPTION_EVENT_CARD:
        default:
            return;

    }
}

void Pa::wait_on_pa_operation(pa_operation* o)
{
    while(pa_operation_get_state(o) == PA_OPERATION_DONE){
        pa_threaded_mainloop_wait( pa_ml );
    }
}

void Pa::ctx_state_cb(pa_context *ctx, void *instance)
{

    switch (pa_context_get_state(ctx)) {
        case PA_CONTEXT_READY: {
            pa_operation *o;

            // source list cb
            o = pa_context_get_source_info_list(ctx, &Pa::ctx_sourcelist_cb, instance);
            pa_operation_unref(o);

            // Sink input list (application) cb
            o = pa_context_get_sink_input_info_list(ctx, &Pa::ctx_inputlist_cb, instance);
            pa_operation_unref(o);

            // Sink devices list cb
            o = pa_context_get_sink_info_list(ctx, &Pa::ctx_sinklist_cb, instance);
            pa_operation_unref(o);

            // Subscribe to changes
            pa_context_set_subscribe_callback(ctx, &Pa::subscribe_cb, instance);
            pa_context_subscribe(ctx, PA_SUBSCRIPTION_MASK_ALL, NULL, NULL);

            break;
        }

    }
}
