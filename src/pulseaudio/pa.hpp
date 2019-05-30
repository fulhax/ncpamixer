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
    bool reconnect_running;

    AudioObjects PA_INPUTS;
    AudioObjects PA_SINKS;
    AudioObjects PA_SOURCES;
    AudioObjects PA_SOURCE_OUTPUTS;
    AudioObjects PA_CARDS;

    std::mutex connection_mtx;
    std::mutex input_mtx;
    std::mutex sink_mtx;

    pa_mainloop_api *pa_api;

    void waitOnPaOperation(pa_operation *o);
    void deletePaObjects(AudioObjects *objects);

    void (*notifyUpdateCB)();

    static void subscribeCB(
        pa_context *ctx,
        pa_subscription_event_type_t t,
        uint32_t index,
        void *instance
    );
    static void ctxStateCB(
        pa_context *ctx,
        void *instance
    );
    static void ctxSuccessCB(
        pa_context *ctx,
        int success,
        void *instance
    );
    static void ctxSinklistCB(
        pa_context *ctx,
        const pa_sink_info *info,
        int eol,
        void  *instance
    );
    static void ctxInputlistCB(
        pa_context *ctx,
        const pa_sink_input_info *info,
        int eol,
        void  *instance
    );
    static void ctxSourcelistCB(
        pa_context *ctx,
        const pa_source_info *info,
        int eol,
        void  *instance
    );
    static void ctxSourceoutputlistCB(
        pa_context *ctx,
        const pa_source_output_info *info,
        int eol,
        void *instance
    );
    static void ctxCardlistCB(
        pa_context *ctx,
        const pa_card_info *info,
        int eol,
        void *instance
    );
    static void ctxServerinfoCB(
        pa_context *ctx,
        const pa_server_info *info,
        void *instance
    );
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
    void updateInput(const pa_sink_input_info *info);
    void updateSink(const pa_sink_info *info);
    void updateSource(const pa_source_info *info);
    void updateCard(const pa_card_info *info);
    void updateSourceOutput(const pa_source_output_info *info);
    void removePaObject(AudioObjects *objects, uint32_t index);
    void toggleInputMute(uint32_t index);
    void toggleSinkMute(uint32_t index);
    void moveInputSink(uint32_t input_index, uint32_t sink_index);
    void setDefaults(const pa_server_info *info);
    void fetchPaObjects();
    void reconnect();
    void static doReconnect(Pa *pa);
    bool paConnect();
    void clearAllPaObjects();

    uint32_t exists(AudioObjects objects, uint32_t index) override;

    static void readCallback(pa_stream *s, size_t length, void *instance);
    static void streamSuspendedCB(pa_stream *stream, void *instance);
    static void streamStateCB(pa_stream *stream, void *info);

    void createMonitorStreamForPaObject(PaObject *po);
    pa_stream *createMonitorStreamForSource(
        uint32_t source_index,
        uint32_t stream_index
    );

    float getVolumeNorm() override {
        return PA_VOLUME_NORM;
    }
    void notifyUpdate();

    pa_context *pa_ctx;
    pa_threaded_mainloop *pa_ml;
};

#endif
