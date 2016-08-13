#ifndef PA_SOURCE_OUTPUT_
#define PA_SOURCE_OUTPUT_
#include "pa_input.hpp"

class PaSourceOutput : public PaInput
{
public:
    PaSourceOutput();
    uint32_t source;

    virtual uint32_t getRelation() {
        return source;
    };
};

#endif // PA_SOURCE_
