#ifndef AUDIO_OBJECT_HPP_
#define AUDIO_OBJECT_HPP_

#include <map>
#include <string>
#include <vector>

class Audio;

struct AudioObjectAttribute {
    std::string name;
    std::string description;
};

using AudioObjectAttributes = std::vector<AudioObjectAttribute *>;

class AudioObject
{
    Audio *parent;

    std::string name;
protected:
    AudioObjectAttribute *active_attribute;
    AudioObjectAttributes attributes;

public:
    void setParent(Audio *parent);
    Audio *getParent();

    AudioObject();
    virtual ~AudioObject() = default;

    virtual void clearAttributes() = 0;
    virtual void move(uint32_t dest) = 0;

    virtual void switchActiveAttribute(std::string name) = 0;
    virtual void switchDefault() = 0;

    virtual void setVolume(float perc) = 0;
    virtual void setMuted(bool mute) = 0;

    virtual void stepVolume(int dir) = 0;
    virtual void toggleMute() = 0;

    virtual float getVolume() = 0;
    virtual uint32_t getRelation() = 0;
    virtual void setRelation(uint32_t relation) = 0;

    virtual bool isDefault() = 0;
    virtual float getPeak() = 0;
    virtual void setPeak(float peak) = 0;
    virtual bool getMuted() = 0;

    virtual void setName(std::string name);

    virtual AudioObjectAttribute *getActiveAttribute();
    virtual AudioObjectAttributes getAttributes();
    virtual AudioObjectAttribute *getAttribute(uint32_t idx);
    void setActiveAttribute(const std::string &name);
    void switchNextAttribute();
    void addAttribute(AudioObjectAttribute *attr);

    virtual std::string getAppName()
    {
        return "";
    };
    virtual std::string getName()
    {
        return name;
    };
};

using AudioObjects = std::map<uint32_t, AudioObject *>;

#endif // AUDIO_OBJECT_HPP_
