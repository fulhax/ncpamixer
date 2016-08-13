#ifndef TAB_HPP_
#define TAB_HPP_

#include "pa.hpp"

#include <map>

class Tab
{
public:
    uint32_t selected_index;

    std::map<uint32_t, PaObject *> *object;
    std::map<uint32_t, PaObject *> *toggle;

    Tab()
    {
        object = nullptr;
        toggle = nullptr;
    };
    virtual ~Tab() {};

    virtual void draw(int w, int h);
    void handleEvents(const char *event);

    void volumeBar(int w, int h, int px, int py, float vol, float peak);
    void borderBox(int w, int h, int px, int py);
    void selectBox(int w, int px, int py, bool selected);
    uint32_t dropDown(int x, int y, std::map<uint32_t, PaObject *> objects,
                      uint32_t current = 0, int width = 0, int height = 0);
    uint32_t dropDown(int x, int y, std::map<uint32_t, std::string> objects,
                      uint32_t current = 0, int width = 0, int height = 0);
private:
    void fillW(int w, int h, int offset_x, int offset_y, const char *str);
    unsigned int getVolumeColor(int p);
};

#endif // TAB_HPP_
