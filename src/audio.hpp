#ifndef AUDIO_HPP_
#define AUDIO_HPP_

#include <audio_object.hpp>

class Audio
{
    bool connected;
public:
    Audio() {
        connected = false;
    };

    virtual ~Audio() = default;
    virtual bool init() = 0;
    virtual uint32_t exists(AudioObjects object, uint32_t index) = 0;

    virtual bool isConnected() { return connected; }
    virtual void setConnected(bool connected) {
        this->connected = connected;
    }

    virtual float  getVolumeNorm() { return 1; }
    virtual AudioObjects* getInputs() = 0;
    virtual AudioObjects* getSinks() = 0;
    virtual AudioObjects* getSources() = 0;
    virtual AudioObjects* getSourceOutputs() = 0;
    virtual AudioObjects* getCards() = 0;
};

extern Audio* audio;

#endif // AUDIO_HPP_
