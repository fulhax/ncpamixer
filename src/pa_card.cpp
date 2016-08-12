#include "pa_card.hpp"
#include <stdio.h>

PaCard::PaCard()
{
    type = pa_object_t::SINK;
    monitor_stream = nullptr;

    pa_set_volume = nullptr;
    pa_set_mute = nullptr;
    pa_move = nullptr;
}

void PaCard::updateProfiles(pa_card_profile_info *pa_profiles, uint32_t n_profile)
{
    profiles.clear();
    for (uint32_t i = 0; i < n_profile; i++) {
        PaCardProfile p;
        snprintf(p.name, sizeof(p.name), "%s", pa_profiles[i].name);
        snprintf(p.description, sizeof(p.description), "%s",
             pa_profiles[i].description);
       profiles.push_back(p); 
    }

}
