#ifndef PLAYBACK_HPP_
#define PLAYBACK_HPP_

#include "../tab.hpp"

class Playback : public Tab
{
public:
    Playback();
    ~Playback();

    void draw(int w, int h);
    void handleInput(unsigned int input);
private:
    unsigned int selected_input_index;
};

#endif // PLAYBACK_HPP_
