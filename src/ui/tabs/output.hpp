#ifndef OUTPUT_HPP_
#define OUTPUT_HPP_

#include "../tab.hpp"
#include <inttypes.h>

class Output : public Tab
{
public:
    Output();
    ~Output();

    void draw(int w, int h);
    void handleInput(int input);
private:
    uint32_t selected_index;
};

#endif // PLAYBACK_HPP_
