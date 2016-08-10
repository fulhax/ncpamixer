#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include "../tab.hpp"
#include <inttypes.h>

class Configuration : public Tab
{
public:
    Configuration();
    ~Configuration();

    void draw(int w, int h);
    void handleInput(int input);
private:
    uint32_t selected_sink_index;
};

#endif // PLAYBACK_HPP_
