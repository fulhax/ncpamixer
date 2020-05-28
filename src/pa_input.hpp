#ifndef PA_INPUT_
#define PA_INPUT_
#include "pa_object.hpp"
#include <string>

class PaInput : public PaObject
{
public:
    PaInput();
    uint32_t sink;
    std::string app_name;

    uint32_t getRelation() override {
        return sink;
    };

    const char* getAppName() override {
        return app_name.c_str();
    };
};

#endif // PA_INPUT_
