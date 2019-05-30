#ifndef PA_CARD_
#define PA_CARD_

#include <vector>

#include <pulseaudio/pa_object.hpp>
#include <pulseaudio/pa_object_attribute.hpp>

class PaCard : public PaObject
{
public:
    PaCard();
    ~PaCard() final = default;

    pa_object_t getType() override;

    void updateProfiles(pa_card_profile_info *pa_profiles, uint32_t n_profile);
};

#endif // PA_CARD_
