#ifndef PA_OBJECT_
#define PA_OBJECT_
#include <pulse/pulseaudio.h>
enum pa_object_t {SINK, INPUT, SOURCE, SOURCE_OUTPUT};

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

    virtual uint32_t getSink(){
        throw -1;
    };

    virtual char * getAppName(){
        throw -1;
    };

    virtual uint32_t getSource(){
        throw -1;
    };
};

#endif // PA_OBJECT_
