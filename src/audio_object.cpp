#include "audio_object.hpp"

#include <audio.hpp>

AudioObject::AudioObject() {
    active_attribute = nullptr;
    parent = nullptr;
    name = "";
}

void AudioObject::setParent(Audio* parent) {
    this->parent = parent;
}

Audio* AudioObject::getParent() {
    return parent;
}

void AudioObject::setName(std::string name) {
    this->name = std::move(name);
};
