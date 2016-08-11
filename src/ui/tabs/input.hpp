#ifndef INPUT_HPP_
#define INPUT_HPP_

#include "../tab.hpp"
#include <inttypes.h>

class Input : public Tab
{
public:
    Input();
    ~Input();

    void draw(int w, int h);
};

#endif // PLAYBACK_HPP_
