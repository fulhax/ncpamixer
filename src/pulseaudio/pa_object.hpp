#ifndef PA_OBJECT_
#define PA_OBJECT_

#include <pulse/pulseaudio.h>
#include <vector>

#include <audio.hpp>
#include "pa_port.hpp"

enum pa_object_t {SINK, INPUT, SOURCE, SOURCE_OUTPUT, CARD};

class PaObject : public AudioObject
{
    pa_object_t type;
    pa_volume_t volume;
    pa_stream *monitor_stream;

    PaObjectAttribute *active_attribute;
    std::vector<PaObjectAttribute *> attributes;

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
public:
    PaObject();
    virtual ~PaObject();

    void setVolume(float perc);
    void stepVolume(int dir);
    void move(uint32_t dest);
    void toggleMute();
    void setActiveAttribute(const char* name);
    void setDefault(const char* name);
    void clearAttributes();

    virtual char *getAppName()
    {
        return nullptr;
    };

    virtual uint32_t getRelation();
};

#endif // PA_OBJECT_
