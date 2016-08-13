#ifndef UI_HPP_
#define UI_HPP_

#include <ncurses.h>

#include <condition_variable>
#include <vector>

#include "pa.hpp"
#include "tab.hpp"

#define NUM_TABS 5

enum TABS {
    TAB_PLAYBACK = 0,
    TAB_RECORDING,
    TAB_OUTPUT,
    TAB_INPUT,
    TAB_CONFIGURATION
};

class Ui
{
public:
    Ui();
    virtual ~Ui();

    int init();
    void run();

    int width;
    int height;

    WINDOW *window;
    WINDOW *statusbar;
private:
    bool running;

    const char *tabs[NUM_TABS] = {
        "Playback",
        "Recording",
        "Output Devices",
        "Input Devices",
        "Configuration"
    };

    int tab_index;
    Tab *current_tab;
    static void resize(int signum);

    void statusBar();
    void handleInput();
    void kill();
    void draw();
    void switchTab(int index);
};

extern Ui ui;

#endif // UI_HPP_
