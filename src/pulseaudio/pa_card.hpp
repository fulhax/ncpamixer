#ifndef PA_CARD_
#define PA_CARD_
#include <vector>
#include "pa_object.hpp"
#include "pa_object_attribute.hpp"

class PaCard : public PaObject
{
public:
    PaCard();
    void updateProfiles(pa_card_profile_info *pa_profiles, uint32_t n_profile);
};

#endif // PA_CARD_
