#ifndef PA_INPUT_
#define PA_INPUT_
#include "pa_object.hpp"

class PaInput : public PaObject
{
public:
    PaInput();
    uint32_t sink;
    char app_name[255];

    virtual uint32_t getRelation() {
        return sink;
    };

    virtual char* getAppName() {
        return app_name;
    };
};

#endif // PA_INPUT_
