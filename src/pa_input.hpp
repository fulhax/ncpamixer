#ifndef PA_INPUT_
#define PA_INPUT_
#include "pa_object.hpp"

class PaInput : public PaObject
{
public:
    PaInput();
    uint32_t sink;
    char app_name[255];

    uint32_t getRelation() override {
        return sink;
    };

    char* getAppName() override {
        return &app_name[0];
    };
};

#endif // PA_INPUT_
