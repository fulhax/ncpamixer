#include <audio.hpp>

#ifdef USE_PULSEAUDIO
    #include <pulse/pulseaudio.h>
    #include <pulseaudio/pa.hpp>

    Audio* audio = new Pa();
#else
    Audio* audio = nullptr;
#endif
