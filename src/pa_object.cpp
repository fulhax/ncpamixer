#include "pa_object.hpp"
#include <stdio.h>

PaObject::~PaObject()
{
    if(monitor_stream != nullptr){
        pa_stream_disconnect(monitor_stream);
        pa_stream_unref(monitor_stream);
    }
    fprintf(stderr, "constructor\n");
}
