#ifndef PA_OBJECT_
#define PA_OBJECT_

#include <pulse/pulseaudio.h>
#include <vector>

#include <pulseaudio/pa_port.hpp>

#include <audio_object.hpp>

enum pa_object_t {SINK, INPUT, SOURCE, SOURCE_OUTPUT, CARD};

class PaObject : public AudioObject
{
    bool is_default;
    bool mute;
    float peak;

    uint32_t index;

    std::string pa_name;

    unsigned int channels;
    uint32_t monitor_index;

    pa_volume_t volume;
    pa_stream *monitor_stream;
public:
    PaObject();
    ~PaObject() override;
    virtual pa_object_t getType() = 0;

    void setVolume(float perc) override;
    void stepVolume(int dir) override;
    void move(uint32_t dest) override;
    void toggleMute() override;
    void switchActiveAttribute(std::string name) override;
    void setDefault() override;
    void clearAttributes() override;
    uint32_t getRelation() override;

    void setRelation(uint32_t relation) override {}

    void setPaName(std::string pa_name) {
        this->pa_name = std::move(pa_name);
    }

    bool isDefault() override {
        return is_default;
    }
    float getPeak() override {
        return peak;
    }
    void setPeak(float peak) override {
        this->peak = peak;
    }
    void setChannels(int channels) {
        this->channels = channels;
    }
    bool getMuted() override {
        return mute;
    }

    void setMuted(bool mute) override {
        this->mute = mute;
    };

    float getVolume() override {
        return volume;
    }

    void initVolume(float volume) {
        this->volume = volume;
    }

    void setIndex(uint32_t index) {
        this->index = index;
    }

    uint32_t getIndex() const {
        return index;
    }

    void setMonitorIndex(uint32_t index) {
        monitor_index = index;
    }

    pa_stream* getMonitorStream() {
        return monitor_stream;
    }

    uint32_t getMonitorIndex() const {
        return monitor_index;
    }

    void setMonitorStream(pa_stream* stream) {
        monitor_stream = stream;
    }

    bool checkMonitorIndex(uint32_t index) const {
        return monitor_index == index;
    }

    pa_operation *(*pa_set_volume)(
        pa_context *,
        uint32_t,
        const pa_cvolume *,
        pa_context_success_cb_t,
        void *
    );
    pa_operation *(*pa_set_mute)(
        pa_context *,
        uint32_t,
        int,
        pa_context_success_cb_t,
        void *
    );
    pa_operation *(*pa_move)(
        pa_context *,
        uint32_t,
        uint32_t,
        pa_context_success_cb_t,
        void *
    );
    pa_operation *(*pa_set_active_attribute)(
        pa_context *,
        uint32_t,
        const char *,
        pa_context_success_cb_t,
        void *
    );
    pa_operation *(*pa_set_default)(
        pa_context *,
        const char*,
        pa_context_success_cb_t,
        void *
    );
};

#endif // PA_OBJECT_
