#ifndef RECORDING_HPP_
#define RECORDING_HPP_

#include "../tab.hpp"
#include <inttypes.h>

class Recording : public Tab
{
public:
    Recording();
    ~Recording();

    void draw(int w, int h);
};

#endif // PLAYBACK_HPP_
