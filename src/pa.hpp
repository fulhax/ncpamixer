#ifndef PA_HPP
#define PA_HPP

#include <pulse/pulseaudio.h>
#include <map>
#include <mutex>

#include "pa_object.hpp"
#include "pa_input.hpp"
#include "pa_sink.hpp"
#include "pa_source.hpp"
#include "pa_source_output.hpp"

struct PA_SINK {
    uint32_t index;
    char name[255];
    char app_name[255];
    pa_volume_t volume;
    bool mute;
    unsigned int channels;

    uint32_t monitor_index;
    pa_stream *monitor_stream;
    float peak;
};

struct PA_INPUT : PA_SINK {
    uint32_t sink;
};

typedef void (*notify_update_callback)();

class Pa
{
public:
    explicit Pa();
    ~Pa();

    void updatePeakByDeviceId(uint32_t index, float peak);
    void update_input(const pa_sink_input_info *info);
    void update_sink(const pa_sink_info *info);
    void update_source(const pa_source_info *info);
    void remove_input(uint32_t index);
    void remove_sink(uint32_t index);
    void set_input_volume(uint32_t index, int dir);
    void set_sink_volume(uint32_t index, int dir);
    void toggle_input_mute(uint32_t index);
    void toggle_sink_mute(uint32_t index);
    void move_input_sink(uint32_t input_index, uint32_t sink_index);
    uint32_t sink_input_exists(uint32_t index);
    uint32_t sink_exists(uint32_t index);

    static void ctx_state_cb(pa_context *ctx, void *userdata);
    static void subscribe_cb(pa_context *ctx, pa_subscription_event_type_t t,
                             uint32_t index, void *userdata);
    static void ctx_sinklist_cb(pa_context *ctx, const pa_sink_info *info,
                                int eol, void  *userdata);
    static void ctx_inputlist_cb(pa_context *ctx, const pa_sink_input_info *info,
                                 int eol, void  *userdata);
    static void ctx_sourcelist_cb(pa_context *ctx, const pa_source_info *info,
                                  int eol, void  *userdata);
    static void read_callback(pa_stream *s, size_t length, void *instance);
    static void stream_suspended_cb(pa_stream *stream, void *instance);
    static void stream_state_cb(pa_stream *stream, void *info);

    void create_monitor_stream_for_paobject(PaObject* po);
    pa_stream* create_monitor_stream_for_source(uint32_t source_index, uint32_t stream_index);

    void set_notify_update_cb(notify_update_callback cb);
    void notify_update();

    std::map<uint32_t, PaInput> PA_INPUTS;
    std::map<uint32_t, PaSink> PA_SINKS;
    std::map<uint32_t, PaSource> PA_SOURCES;
    std::map<uint32_t, PaSourceOutput> PA_SOURCE_OUTPUTS;

    void (*notify_update_cb)();
    std::mutex inputMtx;
private:
    pa_context *pa_ctx;
    pa_threaded_mainloop *pa_ml;
    pa_mainloop_api *pa_api;

    void wait_on_pa_operation(pa_operation *o);
    std::mutex sinkMtx;
};

extern Pa pa;

#endif
