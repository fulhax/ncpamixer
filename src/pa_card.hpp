#ifndef PA_CARD_
#define PA_CARD_
#include <vector>
#include "pa_object.hpp"

class PaCardProfile
{
public:
    char name[255];
    char description[255];
};

class PaCard : public PaObject
{
public:
    PaCard();
    void updateProfiles(pa_card_profile_info *pa_profiles, uint32_t n_profile);

    PaCardProfile active_profile;
    std::vector<PaCardProfile> profiles;
};

#endif // PA_CARD_
