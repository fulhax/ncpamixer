#ifndef PA_OBJECT_
#define PA_OBJECT_
#include <pulse/pulseaudio.h>
enum pa_object_t {SINK, INPUT};

class PaObject
{
public:
    PaObject() {};
    virtual ~PaObject();
    pa_object_t type;

    uint32_t index;
    char name[255];

    unsigned int channels;
    pa_volume_t volume;
    bool mute;

    uint32_t monitor_index;
    pa_stream* monitor_stream;
    float peak;
};

#endif // PA_OBJECT_
