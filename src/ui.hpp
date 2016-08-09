#ifndef UI_HPP_
#define UI_HPP_

#include <ncursesw/ncurses.h>
#include <pulse/pulseaudio.h>

#include <condition_variable>
#include <vector>

#include "pa.hpp"
#include "tab.hpp"

#define NUM_TABS 5

class Ui
{
public:
    Ui();
    virtual ~Ui();

    int init();
    void run();

    int width;
    int height;
private:
    bool running;

    const char *tabs[NUM_TABS] = {
        "Playback",
        "Recording",
        "Output Devices",
        "Input Devices",
        "Configuration"
    };

    unsigned int tab_index;
    Tab *current_tab;
    static void resize(int signum);

    void statusBar();
    void handleInput();
    void kill();
    void draw();
};

#endif // UI_HPP_
