#ifndef PA_SOURCE_
#define PA_SOURCE_
#include "pa_object.hpp"

class PaSource : public PaObject
{
public:
    PaSource();
    void updatePorts(pa_source_port_info **info, uint32_t n_ports);
};

#endif // PA_SOURCE_
