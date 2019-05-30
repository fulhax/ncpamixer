#include "pa_object.hpp"

#include <cstring>

#include <pulseaudio/pa.hpp>

PaObject::PaObject()
{
    index = 0;
    pa_name = "";
    channels = 0;
    volume = 0;
    mute = false;
    monitor_index = 0;
    monitor_stream = nullptr;
    peak = 0;
    pa_set_volume = nullptr;
    pa_set_mute = nullptr;
    pa_move = nullptr;
    pa_set_active_attribute = nullptr;
    pa_set_default = nullptr;
    active_attribute = nullptr;
    is_default = false;
}

PaObject::~PaObject()
{
    clearAttributes();
}

void PaObject::setVolume(float perc)
{
    auto pulse = reinterpret_cast<Pa*>(getParent());
    if (pulse == nullptr) {
        return;
    }

    if (pa_set_volume != nullptr) {
        pa_threaded_mainloop_lock(pulse->pa_ml);

        int vol = static_cast<int>(PA_VOLUME_NORM * perc);
        pa_cvolume cvol;

        pa_cvolume_init(&cvol);
        pa_cvolume_set(&cvol, channels, vol);


        pa_operation *o = pa_set_volume(
                              pulse->pa_ctx,
                              index,
                              &cvol,
                              nullptr,
                              nullptr
                          );
        pa_operation_unref(o);
        pa_threaded_mainloop_unlock(pulse->pa_ml);
    }
}

void PaObject::stepVolume(int dir)
{
    if (volume <= 1000 && dir == -1) {
        return;
    }

    setVolume(static_cast<float>(volume + (1000 * dir)) / PA_VOLUME_NORM);
}


void PaObject::toggleMute()
{
    auto pulse = reinterpret_cast<Pa*>(getParent());
    if (pulse == nullptr) {
        return;
    }

    if (pa_set_mute != nullptr) {
        pa_operation *o = pa_set_mute(
                              pulse->pa_ctx,
                              index,
                              !mute ? 1 : 0,
                              nullptr,
                              nullptr
                          );
        pa_operation_unref(o);
    }
}


void PaObject::move(uint32_t dest)
{
    auto pulse = reinterpret_cast<Pa*>(getParent());
    if (pulse == nullptr) {
        return;
    }

    if (pa_move != nullptr) {
        pa_operation *o = pa_move(
                              pulse->pa_ctx,
                              index,
                              dest,
                              nullptr,
                              nullptr
                          );
        pa_operation_unref(o);
    }
}


void PaObject::switchActiveAttribute(std::string name)
{
    auto pulse = reinterpret_cast<Pa*>(getParent());
    if (pulse == nullptr) {
        return;
    }

    if (pa_set_active_attribute != nullptr) {
        pa_operation *o = pa_set_active_attribute(
                              pulse->pa_ctx,
                              index,
                              name.c_str(),
                              nullptr,
                              nullptr
                          );
        pa_operation_unref(o);
    }
}


void PaObject::setDefault()
{
    auto pulse = reinterpret_cast<Pa*>(getParent());
    if (pulse == nullptr) {
        return;
    }

    if (pa_set_default != nullptr) {
        pa_operation *o = pa_set_default(
                              pulse->pa_ctx,
                              getName().c_str(),
                              nullptr,
                              nullptr
                          );
        pa_operation_unref(o);
    }
}

void PaObject::clearAttributes()
{
    for (size_t i = 0; i < attributes.size(); i++) {
        delete attributes[i];
    }

    attributes.clear();
}

uint32_t PaObject::getRelation()
{
    if (active_attribute != nullptr) {
        for (size_t j = 0; j < attributes.size(); j++) {
            int current = attributes[j]->name.compare(active_attribute->name);

            if (current == 0) {
                return j;
            }
        }
    }

    return UINT32_MAX;
}

std::string PaObject::getPaName() {
    return pa_name;
}
