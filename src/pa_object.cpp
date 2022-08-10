#include "pa_object.hpp"

#include <string.h>

#include "pa.hpp"

PaObject::PaObject() : type(pa_object_t::SINK)
{
    index = 0;
    memset(name, 0, sizeof(name));
    memset(pa_name, 0, sizeof(pa_name));
    channels = 0;
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

void PaObject::set_volume(float perc)
{

    if (pa_set_volume != nullptr) {
        pa_threaded_mainloop_lock(pulse.pa_ml);

        int vol = (PA_VOLUME_NORM * perc);
        pa_cvolume cvol;

        pa_cvolume_init(&cvol);
        pa_cvolume_set(&cvol, channels, vol);


        pa_operation *o = pa_set_volume(
                              pulse.pa_ctx,
                              index,
                              &cvol,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
        pa_threaded_mainloop_unlock(pulse.pa_ml);
    }
}

void PaObject::step_volume(int dir)
{
    if (volume < 0x400 && dir == -1) {
        set_volume(0);
        return;
    }

    set_volume(static_cast<float>(volume + (0x400 * dir)) / PA_VOLUME_NORM);
}


void PaObject::toggle_mute()
{
    if (pa_set_mute != nullptr) {
        pa_operation *o = pa_set_mute(
                              pulse.pa_ctx,
                              index,
                              !mute,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
    }
}


void PaObject::move(uint32_t dest)
{
    if (pa_move != nullptr) {
        pa_operation *o = pa_move(
                              pulse.pa_ctx,
                              index,
                              dest,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
    }
}


void PaObject::set_active_attribute(const char *name)
{
    if (pa_set_active_attribute != nullptr) {
        pa_operation *o = pa_set_active_attribute(
                              pulse.pa_ctx,
                              index,
                              name,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
    }
}


void PaObject::set_default(const char *name)
{
    if (pa_set_default != nullptr) {
        pa_operation *o = pa_set_default(
                              pulse.pa_ctx,
                              name,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
    }
}

void PaObject::clearAttributes()
{
    for (uint32_t i = 0; i < attributes.size(); i++) {
        delete attributes[i];
    }

    attributes.clear();
}

uint32_t PaObject::getRelation()
{
    if (active_attribute != nullptr) {
        for (uint32_t j = 0; j < attributes.size(); j++) {
            int current = strcmp(attributes[j]->name, active_attribute->name);

            if (current == 0) {
                return j;
            }
        }
    }

    return -1;
}
