#ifndef PA_INPUT_
#define PA_INPUT_

#include <pulseaudio/pa_object.hpp>

class PaInput : public PaObject
{
    uint32_t sink;
    std::string app_name;
public:
    PaInput();
    virtual ~PaInput() override = default;

    pa_object_t getType() override;

    uint32_t getRelation() override;
    void setRelation(uint32_t relation) override;

    std::string getAppName() override;
    virtual void setAppName(std::string app_name);
};

#endif // PA_INPUT_
