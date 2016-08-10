#ifndef TAB_HPP_
#define TAB_HPP_

#include "pa.hpp"

#include <map>

class Tab
{
public:
    Tab() {};
    virtual ~Tab() {};

    virtual void draw(int w, int h) = 0;
    virtual void handleInput(int input) = 0;

    void volumeBar(int w, int h, int px, int py, float vol, float peak);
    uint32_t dropDown(int x, int y, std::map<uint32_t, PaObject *> objects,
                      uint32_t current = 0);
private:
    void fillW(int w, int h, int offset_x, int offset_y, const char *str);
    unsigned int getVolumeColor(int p);
};

#endif // TAB_HPP_
