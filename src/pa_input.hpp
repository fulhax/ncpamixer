#ifndef PA_INPUT_
#define PA_INPUT_
#include "pa_object.hpp"

class PaInput : public PaObject
{
public:
    PaInput();
    char app_name[255];
    uint32_t sink;
};

#endif // PA_INPUT_
