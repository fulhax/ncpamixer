#ifndef PLAYBACK_HPP_
#define PLAYBACK_HPP_

#include "../tab.hpp"
#include <inttypes.h>

class Playback : public Tab
{
public:
    Playback();
    ~Playback();

    void draw(int w, int h);
    void handleInput(int input);
private:
    uint32_t selected_index;
};

#endif // PLAYBACK_HPP_
