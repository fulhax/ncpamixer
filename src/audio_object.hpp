#ifndef AUDIO_OBJECT_HPP_
#define AUDIO_OBJECT_HPP_

#include <map>
#include <string>
#include <vector>

class Audio;

struct AudioObjectAttribute
{
    std::string name;
    std::string description;
};

using AudioObjectAttributes = std::vector<AudioObjectAttribute*>;

class AudioObject
{
    Audio* parent;

    std::string name;
protected:
    AudioObjectAttribute* active_attribute;
    AudioObjectAttributes attributes;

public:
    void setParent(Audio* parent);
    Audio* getParent();

    AudioObject();
    virtual ~AudioObject() = default;

    virtual void clearAttributes() = 0;
    virtual void move(uint32_t dest) = 0;

    virtual void setActiveAttribute(std::string name) = 0;
    virtual void setDefault() = 0;
    virtual void setVolume(float perc) = 0;
    virtual void setMuted(bool mute) = 0;

    /* virtual setAppName() { return ; }; */
    virtual void setName(std::string name);

    virtual void stepVolume(int dir) = 0;
    virtual void toggleMute() = 0;

    virtual float getVolume() = 0;
    virtual std::string getAppName() { return ""; };
    virtual std::string getName() { return name; };
    virtual uint32_t getRelation() = 0;
    virtual void setRelation(uint32_t relation) = 0;

    virtual bool isDefault() = 0;
    virtual float getPeak() = 0;
    virtual void setPeak(float peak) = 0;
    virtual bool getMuted() = 0;

    virtual AudioObjectAttribute* getActiveAttribute() {
        return active_attribute;
    }
    virtual AudioObjectAttributes getAttributes() {
        return attributes ;
    }
    virtual AudioObjectAttribute* getAttribute(uint32_t idx) {
        return attributes[idx];
    }
    void switchNextAttribute() {
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
    void addAttribute(AudioObjectAttribute* attr) {
        attributes.push_back(attr);
    }
};

using AudioObjects = std::map<uint32_t, AudioObject*>;

#endif // AUDIO_OBJECT_HPP_
