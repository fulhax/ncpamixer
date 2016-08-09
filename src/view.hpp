#ifndef VIEW_HPP
#define VIEW_HPP
#include <ncursesw/ncurses.h>
#include <condition_variable>
#include <pulse/pulseaudio.h>
#include <vector>
#include "pa.hpp"

class View
{
public:
    explicit View(Pa *p);
    ~View();

    void drawStatusbar();
    void run();
    void notify_draw();
    void remove_input(uint32_t index);
    void resize_view();

    void (*setInputVolume)(uint32_t, int);
    void (*moveInputSink)(uint32_t, uint32_t);
    void (*toggleInputMute)(uint32_t);    
    bool running;

    unsigned int  view;
    uint32_t selectedInputIndex;

private:
    WINDOW *win;
    Pa *pa;
    int x;
    int y;
    bool draw;

    std::condition_variable cv;
    std::mutex upd;
    std::mutex rdyStateMtx;

    void init();
    void kill();
    void render();
    void handleInput();
    void fillW(int offset_x, int offset_y, const char *str);
    void volumeBar(int x, int y, double vol, double peak);
    unsigned int getVolumeColor(int p);
};



#endif /* ifndef VIEW_HPP */
