#include "pa_card.hpp"

PaCard::PaCard()
{
    pa_set_active_attribute = pa_context_set_card_profile_by_index;
    pa_set_volume = nullptr;
    pa_set_mute = nullptr;
    pa_move = nullptr;
    active_attribute = nullptr;
    pa_set_default = nullptr;
}

pa_object_t PaCard::getType() {
    return pa_object_t::CARD;
}

void PaCard::updateProfiles(pa_card_profile_info *pa_profiles, uint32_t n_profile)
{
    clearAttributes();

    for (uint32_t i = 0; i < n_profile; i++) {
        auto p = new AudioObjectAttribute;
        p->name = &pa_profiles[i].name[0];
        p->description = &pa_profiles[i].description[0];

        addAttribute(p);
    }
}
