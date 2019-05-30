#ifndef PA_HPP
#define PA_HPP

#include <map>
#include <mutex>

#include <audio.hpp>

#include "pa_card.hpp"
#include "pa_input.hpp"
#include "pa_object.hpp"
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

using notify_update_callback = void(*)();

class Pa : public Audio
{
    bool pa_init;
    void waitOnPaOperation(pa_operation *o);
    void deletePaObjects(AudioObjects *objects);

    std::mutex sinkMtx;

    bool reconnect_running;

    static void subscribe_cb(
        pa_context *ctx,
        pa_subscription_event_type_t t,
        uint32_t index,
        void *instance
    );
    static void ctx_state_cb(
        pa_context *ctx,
        void *instance
    );
    static void ctx_success_cb(
        pa_context *ctx,
        int success,
        void *instance
    );
    static void ctx_sinklist_cb(
        pa_context *ctx,
        const pa_sink_info *info,
        int eol,
        void  *instance
    );
    static void ctx_inputlist_cb(
        pa_context *ctx,
        const pa_sink_input_info *info,
        int eol,
        void  *instance
    );
    static void ctx_sourcelist_cb(
        pa_context *ctx,
        const pa_source_info *info,
        int eol,
        void  *instance
    );
    static void ctx_sourceoutputlist_cb(
        pa_context *ctx,
        const pa_source_output_info *info,
        int eol,
        void *instance
    );
    static void ctx_cardlist_cb(
        pa_context *ctx,
        const pa_card_info *info,
        int eol,
        void *instance
    );
    static void ctx_serverinfo_cb(
        pa_context *ctx,
        const pa_server_info *info,
        void *instance
    );

    AudioObjects PA_INPUTS;
    AudioObjects PA_SINKS;
    AudioObjects PA_SOURCES;
    AudioObjects PA_SOURCE_OUTPUTS;
    AudioObjects PA_CARDS;
public:
    Pa();
    ~Pa() final;

    AudioObjects* getInputs() override { return &PA_INPUTS; };
    AudioObjects* getSinks() override { return &PA_SINKS; };
    AudioObjects* getSources() override { return &PA_SOURCES; };
    AudioObjects* getSourceOutputs() override { return &PA_SOURCE_OUTPUTS; };
    AudioObjects* getCards() override { return &PA_CARDS; };

    bool init() override;
    void exitPa();
    void updatePeakByDeviceId(uint32_t index, float peak);
    void update_input(const pa_sink_input_info *info);
    void update_sink(const pa_sink_info *info);
    void update_source(const pa_source_info *info);
    void update_card(const pa_card_info *info);
    void update_source_output(const pa_source_output_info *info);
    void removePaObject(AudioObjects *objects, uint32_t index);
    void toggle_input_mute(uint32_t index);
    void toggle_sink_mute(uint32_t index);
    void move_input_sink(uint32_t input_index, uint32_t sink_index);
    void setDefaults(const pa_server_info *info);
    void fetchPaobjects();
    void reconnect();
    void static do_reconnect(Pa *pa);
    bool pa_connect();
    void clearAllPaObjects();

    uint32_t exists(AudioObjects objects, uint32_t index) override;

    static void read_callback(pa_stream *s, size_t length, void *instance);
    static void stream_suspended_cb(pa_stream *stream, void *instance);
    static void stream_state_cb(pa_stream *stream, void *info);

    void create_monitor_stream_for_paobject(PaObject *po);
    pa_stream *create_monitor_stream_for_source(
        uint32_t source_index,
        uint32_t stream_index
    );

    float getVolumeNorm() override {
        return PA_VOLUME_NORM;
    }
    void set_notify_update_cb(const notify_update_callback &cb);
    void notify_update();

    void (*notify_update_cb)();
    std::mutex inputMtx;
    std::mutex connectionMtx;

    pa_context *pa_ctx;
    pa_threaded_mainloop *pa_ml;
    pa_mainloop_api *pa_api;
};

#endif
