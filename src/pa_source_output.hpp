#ifndef PA_SOURCE_OUTPUT_
#define PA_SOURCE_OUTPUT_
#include "pa_input.hpp"

class PaSourceOutput : public PaInput
{
public:
    PaSourceOutput();
    uint32_t source;

    uint32_t getRelation() override {
        return source;
    };
};

#endif // PA_SOURCE_
