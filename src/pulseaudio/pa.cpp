#include "pa.hpp"

#include <unistd.h>

#include <cstring>
#include <cmath>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

// https://freedesktop.org/software/pulseaudio/doxygen/introspect_8h.html


Pa::Pa()
{
    notify_update_cb = nullptr;
    pa_ml = nullptr;
    pa_api  = nullptr;
    pa_init = false;
    pa_ctx = nullptr;
    reconnect_running = false;
}

Pa::~Pa()
{
    clearAllPaObjects();

    if (pa_init) {
        exitPa();
    }
}


void Pa::clearAllPaObjects()
{
    deletePaObjects(&PA_SOURCE_OUTPUTS);
    deletePaObjects(&PA_INPUTS);
    deletePaObjects(&PA_SOURCES);
    deletePaObjects(&PA_SINKS);
    deletePaObjects(&PA_CARDS);
}


void Pa::do_reconnect(Pa *pa)
{
    if (pa->reconnect_running || pa->isConnected()) {
        return;
    }

    pa->reconnect_running = true;
    pa->clearAllPaObjects();

    while (!pa->isConnected()) {
        pa->pa_connect();
        usleep(500000);
    }

    pa->reconnect_running = false;
}

void Pa::reconnect()
{

    if (!reconnect_running && !isConnected()) {
        std::thread rThread(do_reconnect, this);
        rThread.detach();
    }
}


bool Pa::pa_connect()
{
    if (pa_ctx != nullptr) {
        return false;
    }

    pa_threaded_mainloop_lock(pa_ml);

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, "ncpamixer");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, "ncpamixer");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "audio-card");
    pa_ctx = pa_context_new_with_proplist(pa_api, nullptr, proplist);
    pa_proplist_free(proplist);

    if (pa_ctx == nullptr) {
        fprintf(stderr, "Unable to create PA context.\n");
        pa_threaded_mainloop_unlock(pa_ml);
        pa_ctx = nullptr;
        exit(EXIT_FAILURE);
        return false;
    }

    pa_context_set_state_callback(pa_ctx, &Pa::ctx_state_cb, this);
    pa_context_set_subscribe_callback(pa_ctx, &Pa::subscribe_cb, this);

    if (pa_context_connect(pa_ctx, nullptr, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        pa_context_disconnect(pa_ctx);
        pa_context_unref(pa_ctx); // Tror ej den behövs?
        pa_threaded_mainloop_unlock(pa_ml);
        pa_ctx = nullptr;

        connectionMtx.unlock();
        reconnect();
        return false;
    }

    pa_threaded_mainloop_unlock(pa_ml);

    connectionMtx.unlock();
    return true;

}

bool Pa::init()
{
    pa_init = true;

    pa_ml = pa_threaded_mainloop_new();

    if (pa_ml == nullptr) {
        fprintf(stderr, "Unable to create PA main loop.\n");
        exit(EXIT_FAILURE);
        return false;
    }

    pa_api  = pa_threaded_mainloop_get_api(pa_ml);

    if (pa_threaded_mainloop_start(pa_ml) < 0) {
        fprintf(stderr, "Unable to start PA mainloop.\n");
        pa_context_disconnect(pa_ctx);
        pa_context_unref(pa_ctx);
        pa_threaded_mainloop_unlock(pa_ml);
        pa_ctx = nullptr;
        exit(EXIT_FAILURE);
        return false;
    }

    return pa_connect();
}

void Pa::ctx_success_cb(pa_context *ctx, int success, void *instance)
{
    auto p = reinterpret_cast<Pa *>(instance);
    if (p == nullptr) {
        return;
    }

    pa_threaded_mainloop_signal(p->pa_ml, 0);
}

void Pa::fetchPaobjects()
{
    //pa_threaded_mainloop_lock(pa_ml);
    pa_operation *o;

    // get cards
    o = pa_context_get_card_info_list(pa_ctx, &Pa::ctx_cardlist_cb, this);
    waitOnPaOperation(o);
    pa_operation_unref(o);

    // source list cb
    o = pa_context_get_source_info_list(pa_ctx, &Pa::ctx_sourcelist_cb, this);
    waitOnPaOperation(o);
    pa_operation_unref(o);

    // Sink devices list cb
    o = pa_context_get_sink_info_list(pa_ctx, &Pa::ctx_sinklist_cb, this);
    waitOnPaOperation(o);
    pa_operation_unref(o);

    // Sink input list (application) cb
    o = pa_context_get_sink_input_info_list(pa_ctx, &Pa::ctx_inputlist_cb, this);
    waitOnPaOperation(o);
    pa_operation_unref(o);

    // source outputs list cb
    o = pa_context_get_source_output_info_list(
            pa_ctx,
            &Pa::ctx_sourceoutputlist_cb,
            this
        );
    waitOnPaOperation(o);
    pa_operation_unref(o);

    // Get server info
    o = pa_context_get_server_info(pa_ctx, &Pa::ctx_serverinfo_cb, this);
    waitOnPaOperation(o);
    pa_operation_unref(o);

    //pa_threaded_mainloop_unlock(pa_ml);
}

void Pa::deletePaObjects(AudioObjects *objects)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    for (auto i = objects->begin(); i != objects->end();) {
        delete i->second;
        i = objects->erase(i);
    }
}

void Pa::removePaObject(AudioObjects *objects, uint32_t index)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    for (auto i = objects->begin(); i != objects->end(); i++) {
        if (i->first == index) {
            delete i->second;
            i = objects->erase(i);
            return;
        }
    }
}

void Pa::exitPa()
{
    if (pa_ctx != nullptr) {
        pa_context_disconnect(pa_ctx);
        pa_threaded_mainloop_stop(pa_ml);
        pa_threaded_mainloop_free(pa_ml);
    }
}

uint32_t Pa::exists(AudioObjects objects, uint32_t index)
{
    if (objects.empty()) {
        return -1;
    }

    if ((objects.find(index) == objects.end())) {
        index = objects.begin()->first;
    }

    return index;
}

void Pa::setDefaults(const pa_server_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

/*     for (auto &s : PA_SINKS) { */
/*         if (!strcmp(s.second->pa_name, info->default_sink_name)) { */
/*             s.second->is_default = true; */
/*         } else { */
/*             s.second->is_default = false; */
/*         } */
/*     } */

/*     for (auto &s : PA_SOURCES) { */
/*         if (!strcmp(s.second->pa_name, info->default_source_name)) { */
/*             s.second->is_default = true; */
/*         } else { */
/*             s.second->is_default = false; */
/*         } */
/*     } */
}

void Pa::update_source_output(const pa_source_output_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    // https://github.com/pulseaudio/pavucontrol/blob/master/src/mainwindow.cc#L802
    const char *app = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_ID);

    if (app != nullptr) {
        if (strcmp(app, "org.PulseAudio.pavucontrol") == 0
            || strcmp(app, "org.gnome.VolumeControl") == 0
            || strcmp(app, "org.kde.kmixd") == 0
            || strcmp(app, "ncpamixer") == 0) {
            return;
        }
    }

    PaSourceOutput *p = nullptr;

    if (PA_SOURCE_OUTPUTS.count(info->index) == 0) {
        p = new PaSourceOutput();
        PA_SOURCE_OUTPUTS[info->index] = p;
        p->setMonitorStream(nullptr);
    } else {
        p = dynamic_cast<PaSourceOutput *>(PA_SOURCE_OUTPUTS[info->index]);
    }

    p->setParent(this);
    p->setIndex(info->index);
    p->setRelation(info->source);
    p->setChannels(info->channel_map.channels);
    p->setMonitorIndex(info->source);
    p->setMuted(info->mute == 1);
    p->setName(info->name);

    p->initVolume(pa_cvolume_avg(&info->volume));

    const char *app_name;
    app_name = pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_NAME);

    if (app_name != nullptr) {
        p->setAppName(app_name);
    }

    notify_update();
}

void Pa::update_source(const pa_source_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    PaSource *p = nullptr;

    bool newObj = false;

    if (PA_SOURCES.count(info->index) == 0) {
        p = new PaSource();
        PA_SOURCES[info->index] = p;
        p->setMonitorIndex(info->index);
        p->setMonitorStream(nullptr);
        newObj = true;
    } else {
        p = dynamic_cast<PaSource *>(PA_SOURCES[info->index]);
    }

    p->setParent(this);
    p->setIndex(info->index);
    p->setChannels(info->channel_map.channels);
    p->setMuted(info->mute == 1);
    p->setName(info->description);
    p->setPaName(info->name);

    p->initVolume(pa_cvolume_avg(&info->volume));

    if (newObj) {
        create_monitor_stream_for_paobject(p);
    }

    p->updatePorts(info->ports, info->n_ports);

    if (info->active_port != nullptr) {
        p->setActiveAttribute(info->active_port->name);
    }

    notify_update();
}

void Pa::update_card(const pa_card_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    PaCard *p = nullptr;

    if (PA_CARDS.count(info->index) == 0) {
        p = new PaCard;
        PA_CARDS[info->index] = p;
        p->setMonitorIndex(info->index);
        p->setMonitorStream(nullptr);
    } else {
        p = reinterpret_cast<PaCard *>(PA_CARDS[info->index]);
    }

    p->setParent(this);
    p->setIndex(info->index);
    p->setChannels(0);
    p->setMuted(false);
    p->updateProfiles(info->profiles, info->n_profiles);

    p->initVolume(0);

    if (info->active_profile != nullptr) {
        p->setActiveAttribute(info->active_profile->name);
    }

    const char *description = pa_proplist_gets(
                                  info->proplist,
                                  PA_PROP_DEVICE_DESCRIPTION
                              );

    if (description != nullptr) {
        p->setName(description);
    } else {
        p->setName(info->name);
    }

    notify_update();
}

void Pa::update_sink(const pa_sink_info *info)
{
    std::lock_guard<std::mutex> lk(inputMtx);

    PaSink *p = nullptr;

    if (PA_SINKS.count(info->index) == 0) {
        p = new PaSink;
        PA_SINKS[info->index] = p;
        p->setMonitorStream(nullptr);
    } else {
        p = dynamic_cast<PaSink *>(PA_SINKS[info->index]);
    }

    p->setParent(this);
    p->setIndex(info->index);
    p->setChannels(info->channel_map.channels);
    p->setMonitorIndex(info->monitor_source);
    p->setMuted(info->mute == 1);

    p->setName(info->description);
    p->setPaName(info->name);

    p->initVolume(pa_cvolume_avg(&info->volume));

    p->updatePorts(info->ports, info->n_ports);

    if (info->active_port != nullptr) {
        p->setActiveAttribute(info->active_port->name);
    }

    notify_update();
}

void Pa::update_input(const pa_sink_input_info *info)
{

    std::lock_guard<std::mutex> lk(inputMtx);

    PaInput *p = nullptr;

    bool sink_changed = true;

    if (PA_INPUTS.count(info->index) == 0) {
        p = new PaInput();
        PA_INPUTS[info->index] = p;
        p->setMonitorStream(nullptr);
    } else {
        p = dynamic_cast<PaInput *>(PA_INPUTS[info->index]);

        if (PA_INPUTS.count(info->index) > 0) {
            sink_changed = info->sink != p->getRelation();
        }
    }

    p->setParent(this);
    p->setIndex(info->index);
    p->setChannels(info->channel_map.channels);
    p->setMuted(info->mute == 1);
    p->setRelation(info->sink);
    p->setName(info->name);

    p->initVolume(pa_cvolume_avg(&info->volume));

    const char *app_name = pa_proplist_gets(
        info->proplist,
        PA_PROP_APPLICATION_NAME
    );

    if (app_name != nullptr) {
        p->setAppName(app_name);
    }

    if (sink_changed) {
        create_monitor_stream_for_paobject(p);
    }

    notify_update();
}

// https://github.com/pulseaudio/pavucontrol/blob/master/src/mainwindow.cc#L541
void Pa::read_callback(pa_stream *s, size_t length, void *instance)
{
    auto pa = reinterpret_cast<Pa *>(instance);

    std::lock_guard<std::mutex> lk(pa->inputMtx);
    const void *data;
    float v;

    if (pa_stream_peek(s, &data, &length) < 0) {
        return;
    }

    if (data == nullptr) {
        /*  NULL data means either a hole or empty buffer.
            Only drop the stream when there is a hole (length > 0) */
        if (length > 0) {
            pa_stream_drop(s);
        }

        return;
    }

    assert(length > 0);
    assert(length % sizeof(float) == 0);

    v = static_cast<const float *>(data)[length / sizeof(float) - 1];

    pa_stream_drop(s);

    v = std::fmin(1, std::fmax(0, v));

    uint32_t input_idx = pa_stream_get_monitor_stream(s);

    if (input_idx != PA_INVALID_INDEX) {
        pa->PA_INPUTS[input_idx]->setPeak(v);
    } else {
        pa->updatePeakByDeviceId(pa_stream_get_device_index(s), v);
    }

    pa->notify_update();
}

void Pa::updatePeakByDeviceId(uint32_t index, float peak)
{
    for (auto &s : PA_SINKS) {
        auto sink = dynamic_cast<PaSink*>(s.second);
        if (sink->checkMonitorIndex(index)) {
            sink->setPeak(peak);
        }
    }

    for (auto &s : PA_SOURCES) {
        auto source = dynamic_cast<PaSource*>(s.second);
        if (source->checkMonitorIndex(index)) {
            source->setPeak(peak);
        }
    }

    for (auto &s : PA_SOURCE_OUTPUTS) {
        auto output = dynamic_cast<PaSourceOutput*>(s.second);
        if (output->checkMonitorIndex(index)) {
            output->setPeak(peak);
        }
    }
}

// https://github.com/pulseaudio/pavucontrol/blob/master/src/mainwindow.cc#L574
pa_stream *Pa::create_monitor_stream_for_source(
    uint32_t source_index,
    uint32_t stream_index = -1
)
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
    attr.maxlength = static_cast<uint32_t>(-1);

    snprintf(t, sizeof(t), "%u", source_index);

    if (!(s = pa_stream_new(pa_ctx, "Peak detect", &ss, nullptr))) {
        return nullptr;
    }

    if (stream_index != static_cast<uint32_t>(-1)) {
        pa_stream_set_monitor_stream(s, stream_index);
    }

    pa_stream_set_read_callback(s, &Pa::read_callback, this);

    flags = static_cast<pa_stream_flags_t>(
                PA_STREAM_DONT_MOVE
                | PA_STREAM_PEAK_DETECT
                | PA_STREAM_ADJUST_LATENCY
            );

    if (pa_stream_connect_record(s, &t[0], &attr, flags) < 0) {
        pa_stream_unref(s);
        return nullptr;
    }

    return s;
}

void Pa::create_monitor_stream_for_paobject(PaObject *po)
{
    pa_stream *monitor = po->getMonitorStream();

    if (monitor != nullptr) {
        pa_stream_disconnect(monitor);
        po->setMonitorStream(nullptr);
    }

    if (po->getType() == pa_object_t::INPUT) {
        auto input = dynamic_cast<PaInput *>(po);
        auto sink = dynamic_cast<PaSink *>(PA_SINKS[input->getRelation()]);

        input->setMonitorStream(
            create_monitor_stream_for_source(sink->getMonitorIndex(), input->getIndex())
        );
    } else {
        po->setMonitorStream(
            create_monitor_stream_for_source(po->getMonitorIndex(), -1)
        );

    }
}

void Pa::set_notify_update_cb(const notify_update_callback &cb)
{
    notify_update_cb = cb;
}

void Pa::notify_update()
{
    if (notify_update_cb != nullptr) {
        notify_update_cb();
    }
}

void Pa::ctx_cardlist_cb(
    pa_context *ctx,
    const pa_card_info *info,
    int eol,
    void *instance
)
{
    auto pa = reinterpret_cast<Pa *>(instance);

    if (eol == 0) {
        pa->update_card(info);
    }
}




void Pa::ctx_sourcelist_cb(
    pa_context *ctx,
    const pa_source_info *info,
    int eol,
    void *instance
)
{
    auto pa = reinterpret_cast<Pa *>(instance);

    if (eol == 0) {
        pa->update_source(info);
    }
}

void Pa::ctx_sourceoutputlist_cb(
    pa_context *ctx,
    const pa_source_output_info *info,
    int eol,
    void *instance
)
{
    auto pa = reinterpret_cast<Pa *>(instance);

    if (eol == 0) {
        pa->update_source_output(info);
    }
}

void Pa::ctx_inputlist_cb(
    pa_context *ctx,
    const pa_sink_input_info *info,
    int eol,
    void  *instance
)
{
    auto pa = reinterpret_cast<Pa *>(instance);

    if (eol == 0) {
        pa->update_input(info);
    }
}

void Pa::ctx_sinklist_cb(
    pa_context *ctx,
    const pa_sink_info *info,
    int eol,
    void  *instance
)
{
    auto pa = reinterpret_cast<Pa *>(instance);

    if (eol == 0) {
        pa->update_sink(info);
    }
}

void Pa::ctx_serverinfo_cb(
    pa_context *ctx,
    const pa_server_info *info,
    void *instance
)
{
    auto pa = reinterpret_cast<Pa *>(instance);

    pa->setDefaults(info);
}

void Pa::subscribe_cb(
    pa_context *ctx,
    pa_subscription_event_type_t t,
    uint32_t index,
    void *instance
)
{

    int type = (t & PA_SUBSCRIPTION_EVENT_TYPE_MASK);
    Pa *pa = reinterpret_cast<Pa *>(instance);
    // https://freedesktop.org/software/pulseaudio/doxygen/def_8h.html#a6bedfa147a9565383f1f44642cfef6a3

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK: {
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->removePaObject(&pa->PA_SINKS, index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {

                pa_operation *o = pa_context_get_sink_info_by_index(
                                      ctx,
                                      index,
                                      &Pa::ctx_sinklist_cb,
                                      instance
                                  );
                pa->waitOnPaOperation(o);
                pa_operation_unref(o);
            }

            break;
        }

        case PA_SUBSCRIPTION_EVENT_SINK_INPUT: {

            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->removePaObject(&pa->PA_INPUTS, index);
            } else if (type == PA_SUBSCRIPTION_EVENT_NEW ||
                       type == PA_SUBSCRIPTION_EVENT_CHANGE) {

                pa_operation *o = pa_context_get_sink_input_info(
                                      ctx,
                                      index,
                                      &Pa::ctx_inputlist_cb,
                                      instance
                                  );
                pa->waitOnPaOperation(o);
                pa_operation_unref(o);
            }

            break;
        }

        case PA_SUBSCRIPTION_EVENT_SOURCE: {
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->removePaObject(&pa->PA_SOURCES, index);
            } else if (
                type == PA_SUBSCRIPTION_EVENT_NEW
                || type == PA_SUBSCRIPTION_EVENT_CHANGE
            ) {
                pa_operation *o = pa_context_get_source_info_by_index(
                                      ctx,
                                      index,
                                      &Pa::ctx_sourcelist_cb,
                                      instance
                                  );

                if (o == nullptr) {
                    return;
                }

                pa->waitOnPaOperation(o);
                pa_operation_unref(o);
            }

            break;
        }

        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->removePaObject(&pa->PA_SOURCE_OUTPUTS, index);
            } else if (
                type == PA_SUBSCRIPTION_EVENT_NEW
                || type == PA_SUBSCRIPTION_EVENT_CHANGE
            ) {
                pa_operation *o = pa_context_get_source_output_info(
                                      ctx,
                                      index,
                                      &Pa::ctx_sourceoutputlist_cb,
                                      instance
                                  );

                pa->waitOnPaOperation(o);
                pa_operation_unref(o);
            }

            break;

        case PA_SUBSCRIPTION_EVENT_CARD:
            if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
                pa->removePaObject(&pa->PA_CARDS, index);
            } else if (
                type == PA_SUBSCRIPTION_EVENT_NEW
                || type == PA_SUBSCRIPTION_EVENT_CHANGE
            ) {
                pa_operation *o = pa_context_get_card_info_by_index(
                                      ctx,
                                      index,
                                      &Pa::ctx_cardlist_cb,
                                      instance
                                  );

                pa->waitOnPaOperation(o);
                pa_operation_unref(o);
            }

            break;

        case PA_SUBSCRIPTION_EVENT_SERVER: {
            pa_operation *o = pa_context_get_server_info(
                                  ctx,
                                  &Pa::ctx_serverinfo_cb,
                                  instance
                              );

            pa->waitOnPaOperation(o);
            pa_operation_unref(o);
        }

        break;

        case PA_SUBSCRIPTION_EVENT_MODULE:
        case PA_SUBSCRIPTION_EVENT_CLIENT:
        case PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE:
        default:
            break;

    }

}

void Pa::waitOnPaOperation(pa_operation *o)
{
    while (pa_operation_get_state(o) == PA_OPERATION_DONE) {
        pa_threaded_mainloop_wait(pa_ml);
    }
}

void Pa::ctx_state_cb(pa_context *ctx, void *instance)
{
    int state = pa_context_get_state(ctx);
    auto pa = reinterpret_cast<Pa *>(instance);

    switch (state) {
        case PA_CONTEXT_READY: {
            pa_operation *o = pa_context_subscribe(
                                  pa->pa_ctx,
                                  (pa_subscription_mask_t)
                                  (
                                      PA_SUBSCRIPTION_MASK_SINK |
                                      PA_SUBSCRIPTION_MASK_SOURCE |
                                      PA_SUBSCRIPTION_MASK_SINK_INPUT |
                                      PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT |
                                      PA_SUBSCRIPTION_MASK_CARD |
                                      PA_SUBSCRIPTION_MASK_SERVER
                                  ),
                                  &Pa::ctx_success_cb, instance);

            pa->waitOnPaOperation(o);
            pa_operation_unref(o);
            pa->fetchPaobjects();

            pa->connectionMtx.lock();
            pa->setConnected(true);
            pa->connectionMtx.unlock();
            break;
        }

        case PA_CONTEXT_FAILED:
            pa->connectionMtx.lock();
            pa->setConnected(false);
            pa_context_unref(pa->pa_ctx);
            pa->pa_ctx = nullptr;
            pa->connectionMtx.unlock();
            pa->reconnect();
            break;


        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
        case PA_CONTEXT_TERMINATED:
            break;
    }
}
