#include "pa.hpp"
#include <cstring>
#include <functional>

// https://freedesktop.org/software/pulseaudio/doxygen/introspect_8h.html

Pa pa;

Pa::Pa()
{
    notify_update_cb = nullptr;

    pa_ml = pa_threaded_mainloop_new();
    pa_api  = pa_threaded_mainloop_get_api(pa_ml);

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, "ncpamixer");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, "ncpamixer");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "audio-card");
    pa_ctx = pa_context_new_with_proplist(pa_api, NULL, proplist);

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

uint32_t Pa::sink_exists(uint32_t index)
{
    if ((PA_SINKS.find(index) == PA_SINKS.end())) {
        index = PA_SINKS.begin()->first;
    }

    return index;
}

uint32_t Pa::source_exists(uint32_t index)
{
    if ((PA_SOURCES.find(index) == PA_SOURCES.end())) {
        index = PA_SOURCES.begin()->first;
    }

    return index;
}

uint32_t Pa::sink_source_output_exists(uint32_t index)
{
    if ((PA_SOURCE_OUTPUTS.find(index) == PA_SOURCE_OUTPUTS.end())) {
        index = PA_SOURCE_OUTPUTS.begin()->first;
    }

    return index;

}

uint32_t Pa::sink_input_exists(uint32_t index)
{
    if ((PA_INPUTS.find(index) == PA_INPUTS.end())) {
        index = PA_INPUTS.begin()->first;
    }

    return index;
}




void Pa::update_source_output(const pa_source_output_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    // https://github.com/pulseaudio/pavucontrol/blob/master/src/mainwindow.cc#L802
    const char *app;

    if ((app = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_ID)))
        if (strcmp(app, "org.PulseAudio.pavucontrol") == 0
            || strcmp(app, "org.gnome.VolumeControl") == 0
            || strcmp(app, "org.kde.kmixd") == 0
            || strcmp(app, "ncpamixer") == 0) {
            return;
        }

    PaSourceOutput *p;

    if (PA_SOURCE_OUTPUTS.count(info->index) == 0) {
        p = new PaSourceOutput;
        PA_SOURCE_OUTPUTS[info->index] = p;
    } else {
        p = reinterpret_cast<PaSourceOutput *>(PA_SOURCE_OUTPUTS[info->index]);
    }

    bool monitor_changed = true;

    if (PA_SOURCE_OUTPUTS.count(info->index)) {
        monitor_changed = info->source != p->source;
    }

    p->index = info->index;
    p->source = info->source;
    p->channels = info->channel_map.channels;
    p->monitor_index = PA_SOURCES[info->source]->index;
    p->volume = (const pa_volume_t) pa_cvolume_avg(&info->volume);
    p->mute = info->mute;

    strncpy(p->name, info->name, 255);

    const char *app_name;
    app_name = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_NAME);

    if (app_name != nullptr) {
        strncpy(p->app_name, app_name, 255);
    }

    if (monitor_changed) {
        create_monitor_stream_for_paobject(p);
    }

    notify_update();
}

void Pa::update_source(const pa_source_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    PaSource *p;
    if (PA_SOURCES.count(info->index) == 0) {
        p = new PaSource;
        PA_SOURCES[info->index] = p;
    } else {
        p = reinterpret_cast<PaSource *>(PA_SOURCES[info->index]);
    }

    bool monitor_changed = true;

    if (PA_SOURCES.count(info->index)) {
        monitor_changed = info->index != p->monitor_index;
    }

    p->channels = info->channel_map.channels;
    p->monitor_index = info->index;
    p->volume = (const pa_volume_t) pa_cvolume_avg(&info->volume);
    PA_SOURCES[info->index]->mute = info->mute;

    strncpy(p->name, info->description, 255);

    if (monitor_changed) {
        create_monitor_stream_for_paobject(p);
    }

    notify_update();


}

void Pa::update_sink(const pa_sink_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    PaSink *p;
    if (PA_SINKS.count(info->index) == 0) {
        p = new PaSink;
        PA_SINKS[info->index] = p;
    } else {
        p = reinterpret_cast<PaSink *>(PA_SINKS[info->index]);
    }


    bool monitor_changed = true;

    if (PA_INPUTS.count(info->index)) {
        monitor_changed = info->monitor_source !=
                          PA_INPUTS[info->index]->monitor_index;
    }

    p->channels = info->channel_map.channels;
    p->monitor_index = info->monitor_source;
    p->volume = (const pa_volume_t) pa_cvolume_avg(&info->volume);
    p->mute = info->mute;

    strncpy(p->name, info->description, 255);

    if (monitor_changed) {
        create_monitor_stream_for_paobject(p);
    }

    notify_update();
}

void Pa::update_input(const pa_sink_input_info *info)
{

    std::lock_guard<std::mutex> lk(inputMtx);

    PaInput *p;
    if (PA_INPUTS.count(info->index) == 0) {
        p = new PaInput;
        PA_INPUTS[info->index] = p;
    } else {
        p = reinterpret_cast<PaInput *>(PA_INPUTS[info->index]);
    }

    bool sink_changed = true;

    if (PA_INPUTS.count(info->index)) {
        sink_changed = info->sink != p->sink;
    }

    p->index = info->index;

    p->channels = info->channel_map.channels;
    p->volume = (const pa_volume_t) pa_cvolume_avg(&info->volume);
    p->mute = info->mute;
    p->sink = info->sink;
    p->monitor_stream = nullptr;
    strcpy(p->name, info->name);

    const char *app_name;
    app_name = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_NAME);

    if (app_name != nullptr) {
        strncpy(p->app_name, app_name, 255);
    }

    if (sink_changed) {
        create_monitor_stream_for_paobject(p);
    }

    notify_update();
}

// https://github.com/pulseaudio/pavucontrol/blob/master/src/mainwindow.cc#L541
void Pa::read_callback(pa_stream *s, size_t length, void *instance)
{
    Pa *pa = (Pa *) instance;
    std::lock_guard<std::mutex> lk(pa->inputMtx);
    const void *data;
    float v;

    if (pa_stream_peek(s, &data, &length) < 0) {
        return;
    }

    if (!data) {
        /*  NULL data means either a hole or empty buffer.
            Only drop the stream when there is a hole (length > 0) */
        if (length) {
            pa_stream_drop(s);
        }

        return;
    }

    assert(length > 0);
    assert(length % sizeof(float) == 0);

    v = ((const float *) data)[length / sizeof(float) - 1];

    pa_stream_drop(s);

    if (v < 0) {
        v = 0;
    }

    if (v > 1) {
        v = 1;
    }

    uint32_t input_idx = pa_stream_get_monitor_stream(s);

    if (input_idx != PA_INVALID_INDEX) {
        pa->PA_INPUTS[input_idx]->peak = v;
    } else {
        pa->updatePeakByDeviceId(pa_stream_get_device_index(s), v);
    }

    pa->notify_update();
}

void Pa::updatePeakByDeviceId(uint32_t index, float peak)
{
    for (auto &s : PA_SINKS) {
        if (s.second->monitor_index == index) {
            s.second->peak = peak;
        }
    }

    for (auto &s : PA_SOURCES) {
        if (s.second->monitor_index == index) {
            s.second->peak = peak;
        }
    }

    for (auto &s : PA_SOURCE_OUTPUTS) {
        if (s.second->monitor_index == index) {
            s.second->peak = peak;
        }
    }
}

void Pa::stream_state_cb(pa_stream *stream, void *instance)
{
    pa_stream_state_t state = pa_stream_get_state(stream);

    if (state == PA_STREAM_TERMINATED || state == PA_STREAM_FAILED) {
        PaInput *i = reinterpret_cast<PaInput *>(instance);
        i->monitor_stream = nullptr;
    }
}

// https://github.com/pulseaudio/pavucontrol/blob/master/src/mainwindow.cc#L574
pa_stream *Pa::create_monitor_stream_for_source(uint32_t source_index,
        uint32_t stream_index = -1)
{
    pa_stream *s;
    char t[16];
    pa_buffer_attr attr;
    pa_sample_spec ss;
    pa_stream_flags_t flags;

    ss.channels = 1;
    ss.format = PA_SAMPLE_FLOAT32;
    ss.rate = 25;

    memset(&attr, 0, sizeof(attr));
    attr.fragsize = sizeof(float);
    attr.maxlength = (uint32_t) - 1;

    snprintf(t, sizeof(t), "%u", source_index);

    if (!(s = pa_stream_new(pa_ctx, "Peak detect", &ss, NULL))) {
        return NULL;
    }

    if (stream_index != (uint32_t) - 1) {
        pa_stream_set_monitor_stream(s, stream_index);
    }

    pa_stream_set_read_callback(s, &Pa::read_callback, this);

    if (stream_index != (uint32_t) - 1) {
        pa_stream_set_state_callback(s, &Pa::stream_state_cb,
                                     &PA_INPUTS[stream_index]);
    }

    flags = (pa_stream_flags_t)(PA_STREAM_DONT_MOVE | PA_STREAM_PEAK_DETECT |
                                PA_STREAM_ADJUST_LATENCY);

    if (pa_stream_connect_record(s, t, &attr, flags) < 0) {
        pa_stream_unref(s);
        return NULL;
    }

    return s;
}

void Pa::create_monitor_stream_for_paobject(PaObject *po)
{
    if (po->monitor_stream != nullptr) {
        pa_stream_disconnect(po->monitor_stream);
        po->monitor_stream = nullptr;
    }

    if (po->type == pa_object_t::INPUT) {
        PaInput *input = reinterpret_cast<PaInput *>(po);
        input->monitor_stream = create_monitor_stream_for_source(
                                    PA_SINKS[input->sink]->monitor_index,
                                    input->index
                                );
    } else {
        po->monitor_stream = create_monitor_stream_for_source(
                                 po->monitor_index,
                                 -1
                             );

    }
}

void Pa::toggle_input_mute(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);
    auto i = PA_INPUTS.find(index);

    if (i != PA_INPUTS.end()) {
        pa_operation *o = pa_context_set_sink_input_mute(
                              pa_ctx,
                              index,
                              !i->second->mute,
                              NULL,
                              NULL
                          );

        pa_operation_unref(o);
    }
}

void Pa::toggle_sink_mute(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);
    auto i = PA_SINKS.find(index);

    if (i != PA_SINKS.end()) {
        pa_operation *o = pa_context_set_sink_mute_by_index(
                              pa_ctx,
                              index,
                              !i->second->mute,
                              NULL,
                              NULL
                          );

        pa_operation_unref(o);
    }
}


void Pa::set_sink_volume(uint32_t index, int dir)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    if (PA_SINKS.find(index) == PA_SINKS.end()) {
        return;
    }

    if (PA_SINKS[index]->volume <= 1000 && dir == -1) {
        return;
    }

    pa_threaded_mainloop_lock(pa_ml);

    int volume = PA_SINKS[index]->volume + (1000 * dir);
    pa_cvolume cvol;

    pa_cvolume_init(&cvol);
    pa_cvolume_set(&cvol, PA_SINKS[index]->channels, volume);
    pa_operation *o = pa_context_set_sink_volume_by_index(
                          pa_ctx,
                          index,
                          &cvol,
                          NULL,
                          NULL
                      );
    pa_operation_unref(o);

    pa_threaded_mainloop_unlock(pa_ml);
}

void Pa::set_input_volume(uint32_t index, int dir)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    if (PA_INPUTS.find(index) == PA_INPUTS.end()) {
        return;
    }

    if (PA_INPUTS[index]->volume <= 1000 && dir == -1) {
        return;
    }

    pa_threaded_mainloop_lock(pa_ml);

    int volume = PA_INPUTS[index]->volume + (1000 * dir);
    pa_cvolume cvol;

    pa_cvolume_init(&cvol);
    pa_cvolume_set(&cvol, PA_INPUTS[index]->channels, volume);
    pa_operation *o = pa_context_set_sink_input_volume(
                          pa_ctx,
                          index,
                          &cvol,
                          NULL,
                          NULL
                      );
    pa_operation_unref(o);

    pa_threaded_mainloop_unlock(pa_ml);

}

void Pa::move_input_sink(uint32_t input_index, uint32_t sink_index)
{
    pa_threaded_mainloop_lock(pa_ml);
    pa_operation *o = pa_context_move_sink_input_by_index(
                          pa_ctx,
                          input_index,
                          sink_index,
                          NULL,
                          NULL
                      );
    pa_operation_unref(o);
    pa_threaded_mainloop_unlock(pa_ml);
}

void Pa::remove_input(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    auto i = PA_INPUTS.find(index);

    if (i != PA_INPUTS.end()) {
        if (i->second->monitor_stream != nullptr) {
            pa_stream_disconnect(i->second->monitor_stream);
            pa_stream_unref(i->second->monitor_stream);
        }
        
        delete i->second;

        PA_INPUTS.erase(index);
    }

    notify_update();
}


void Pa::remove_source_output(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    auto i = PA_SOURCE_OUTPUTS.find(index);

    if (i != PA_SOURCE_OUTPUTS.end()) {
        if (i->second->monitor_stream != nullptr) {
            pa_stream_disconnect(i->second->monitor_stream);
            pa_stream_unref(i->second->monitor_stream);
        }

        delete i->second;
        PA_SOURCE_OUTPUTS.erase(index);
    }

    notify_update();
}

void Pa::remove_source(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    auto i = PA_SOURCES.find(index);

    if (i != PA_SOURCES.end()) {
        if (i->second->monitor_stream != nullptr) {
            pa_stream_disconnect(i->second->monitor_stream);
            pa_stream_unref(i->second->monitor_stream);
        }

        delete i->second;
        PA_SOURCES.erase(index);
    }

    notify_update();
}


void Pa::remove_sink(uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    auto i = PA_SINKS.find(index);

    if (i != PA_SINKS.end()) {
        if (i->second->monitor_stream != nullptr) {
            pa_stream_disconnect(i->second->monitor_stream);
            pa_stream_unref(i->second->monitor_stream);
        }

        delete i->second;
        PA_SINKS.erase(index);
    }
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
                           void *instance)
{
    Pa *pa = reinterpret_cast<Pa *>(instance);

    if (!eol) {
        pa->update_source(info);
    }

    return;
}

void Pa::ctx_sourceoutputlist_cb(pa_context *ctx,
                                 const pa_source_output_info *info, int eol, void *instance)
{
    Pa *pa = reinterpret_cast<Pa *>(instance);

    if (!eol) {
        pa->update_source_output(info);
    }

    return;
}

void Pa::ctx_inputlist_cb(pa_context *ctx, const pa_sink_input_info *info,
                          int eol, void  *instance)
{
    Pa *pa = reinterpret_cast<Pa *>(instance);

    if (!eol) {
        pa->update_input(info);
    }
}

void Pa::ctx_sinklist_cb(pa_context *ctx, const pa_sink_info *info, int eol,
                         void  *instance)
{
    Pa *pa = reinterpret_cast<Pa *>(instance);

    if (!eol) {
        pa->update_sink(info);
    }
}

void Pa::subscribe_cb(pa_context *ctx, pa_subscription_event_type_t t,
                      uint32_t index, void *instance)
{

    int type = (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK);
    Pa *pa = reinterpret_cast<Pa *>(instance);

    // https://freedesktop.org/software/pulseaudio/doxygen/def_8h.html#a6bedfa147a9565383f1f44642cfef6a3

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK: {
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->remove_sink(index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {

                pa_operation *o = pa_context_get_sink_info_by_index(
                                      ctx,
                                      index,
                                      &Pa::ctx_sinklist_cb,
                                      instance
                                  );
                pa_operation_unref(o);
            }

            break;
        }

        case PA_SUBSCRIPTION_EVENT_SINK_INPUT: {

            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->remove_input(index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {

                pa_operation *o = pa_context_get_sink_input_info(
                                      ctx,
                                      index,
                                      &Pa::ctx_inputlist_cb,
                                      instance
                                  );
                pa_operation_unref(o);
            }

            break;
        }

        case PA_SUBSCRIPTION_EVENT_SOURCE:
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->remove_source(index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {

                pa_operation *o = pa_context_get_source_info_by_index(
                                      ctx,
                                      index,
                                      &Pa::ctx_sourcelist_cb,
                                      instance
                                  );
                pa_operation_unref(o);
            }

            break;

        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->remove_source_output(index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {
                pa_operation *o = pa_context_get_source_output_info(ctx, index,
                                  &Pa::ctx_sourceoutputlist_cb, instance);
                pa_operation_unref(o);
            }

        case PA_SUBSCRIPTION_EVENT_MODULE:
        case PA_SUBSCRIPTION_EVENT_CLIENT:
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
        case PA_SUBSCRIPTION_EVENT_SERVER:
        case PA_SUBSCRIPTION_EVENT_CARD:
        default:
            return;

    }
}

void Pa::wait_on_pa_operation(pa_operation *o)
{
    while (pa_operation_get_state(o) == PA_OPERATION_DONE) {
        pa_threaded_mainloop_wait(pa_ml);
    }
}

void Pa::ctx_state_cb(pa_context *ctx, void *instance)
{
    int state = pa_context_get_state(ctx);

    switch (state) {
        case PA_CONTEXT_READY: {
            pa_operation *o;

            // source list cb
            o = pa_context_get_source_info_list(ctx, &Pa::ctx_sourcelist_cb, instance);
            pa_operation_unref(o);

            // Sink devices list cb
            o = pa_context_get_sink_info_list(ctx, &Pa::ctx_sinklist_cb, instance);
            pa_operation_unref(o);

            // Sink input list (application) cb
            o = pa_context_get_sink_input_info_list(ctx, &Pa::ctx_inputlist_cb, instance);
            pa_operation_unref(o);

            // source outputs list cb
            o = pa_context_get_source_output_info_list(ctx, &Pa::ctx_sourceoutputlist_cb,
                    instance);
            pa_operation_unref(o);

            // Subscribe to changes
            pa_context_set_subscribe_callback(ctx, &Pa::subscribe_cb, instance);
            pa_context_subscribe(ctx, PA_SUBSCRIPTION_MASK_ALL, NULL, NULL);

            break;
        }

        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            break;
    }
}
