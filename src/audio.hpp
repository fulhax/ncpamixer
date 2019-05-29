#ifndef AUDIO_HPP_
#define AUDIO_HPP_

#include <map>
#include <vector>
#include <string>

struct AudioObjectAttribute
{
    std::string name;
    std::string description;
};

using AudioObjectAttributes = std::vector<AudioObjectAttribute*>;

class AudioObject
{
    AudioObject() = default;

    std::string name;
    std::string pa_name;

    uint32_t index;
    bool is_default;

    unsigned int channels;
    bool mute;

    uint32_t monitor_index;
    float peak;

    AudioObjectAttribute* active_attribute;
    AudioObjectAttributes attributes;
public:
    virtual ~AudioObject() = 0;

    virtual void clearAttributes() = 0;
    virtual void move(uint32_t dest) = 0;

    virtual void setActiveAttribute(std::string name) = 0;
    virtual void setDefault() = 0;
    virtual void setVolume(float perc) = 0;

    virtual void stepVolume(int dir) = 0;
    virtual void toggleMute() = 0;

    virtual float getVolume() = 0;
    virtual std::string getAppName() = 0;
    virtual std::string getName() = 0;
    virtual uint32_t getRelation() = 0;
    virtual AudioObjectAttribute* getActiveAttribute() = 0;

    virtual bool isDefault() { return is_default; }
    virtual float getPeak() { return peak; }
    virtual bool getMuted() { return mute; }

    virtual AudioObjectAttributes getAttributes() { return attributes ;}
    virtual AudioObjectAttribute* getAttribute(uint32_t idx) {
        return attributes[idx];
    }
    virtual void switchNextAttribute() {
        if (!attributes.empty()) {
            uint32_t current_attribute = getRelation();

            if (current_attribute + 1 < attributes.size()) {
                current_attribute++;
            } else {
                current_attribute = 0;
            }

            setActiveAttribute(getAttribute(current_attribute)->name);
        }
    }
};

using AudioObjects = std::map<uint32_t, AudioObject*>;

class Audio
{
    Audio() = default;
public:
    virtual ~Audio() = 0;
    virtual bool init() = 0;
    virtual bool exists(AudioObjects object, int index) = 0;

    bool connected;
};

static Audio* audio = nullptr;

#ifdef PULSEAUDIO
    #include <pulse/pulseaudio.h>
    #include "pa.hpp"

    #define VOLUME_NORM PA_VOLUME_NORM

    using PaObjectAttribute = AudioObjectAttribute;

    if (audio == nullptr) {
        audio = *pulse;
    }
#else
    #define VOLUME_NORM 1
#endif

#endif // AUDIO_HPP_
