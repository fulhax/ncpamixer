#include "audio_object.hpp"

#include <audio.hpp>

#include <algorithm>

AudioObject::AudioObject(): name("")
{
    active_attribute = nullptr;
    parent = nullptr;
}

void AudioObject::setParent(Audio *parent)
{
    this->parent = parent;
}

Audio *AudioObject::getParent()
{
    return parent;
}

void AudioObject::setName(std::string name)
{
    this->name = std::move(name);
}

AudioObjectAttribute *AudioObject::getActiveAttribute()
{
    return active_attribute;
}

AudioObjectAttributes AudioObject::getAttributes()
{
    return attributes;
}

AudioObjectAttribute *AudioObject::getAttribute(uint32_t idx)
{
    return attributes[idx];
}

void AudioObject::setActiveAttribute(const std::string &name)
{
    AudioObjectAttribute tmp;

    auto attr = std::find_if(
                    attributes.begin(),
                    attributes.end(),
                    [&name](auto const & attr) {
                        return attr->name == name;
                    }
                );

    if (attr != attributes.end()) {
        active_attribute = *attr;
    }
}

void AudioObject::switchNextAttribute()
{
    if (!attributes.empty()) {
        uint32_t current_attribute = getRelation() + 1;

        if (current_attribute >= attributes.size()) {
            current_attribute = 0;
        }

        switchActiveAttribute(getAttribute(current_attribute)->name);
    }
}

void AudioObject::addAttribute(AudioObjectAttribute *attr)
{
    attributes.push_back(attr);
}

void AudioObject::setChannels(const AudioObjectChannels &channels)
{
    this->channels = channels;
}

AudioObjectChannel* AudioObject::getChannel(uint8_t channel) 
{
    return &channels[channel];
}

size_t AudioObject::getChannelCount()
{
    return channels.size();
}
