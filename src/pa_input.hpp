#ifndef PA_INPUT_
#define PA_INPUT_
#include "pa_object.hpp"

class PaInput : public PaObject
{
public:
    PaInput();
    ~PaInput();
    uint32_t sink;
    char *app_name;

    uint32_t getRelation() override {
        return sink;
    };

    void setAppName(const char *name);
    char* getAppName() override {
        return app_name;
    };
};

#endif // PA_INPUT_
