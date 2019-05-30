#ifndef PA_SOURCE_OUTPUT_
#define PA_SOURCE_OUTPUT_

#include <pulseaudio/pa_input.hpp>

class PaSourceOutput : public PaInput
{
    uint32_t source;
public:
    PaSourceOutput();
    ~PaSourceOutput() override = default;

    pa_object_t getType() override;

    uint32_t getRelation() override;
    void setRelation(uint32_t relation) override;
};

#endif // PA_SOURCE_
