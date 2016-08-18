#ifndef UI_HPP_
#define UI_HPP_

#include <ncurses.h>

#include <condition_variable>
#include <vector>

#include "pa.hpp"
#include "tab.hpp"

#define NUM_TABS 5

#define COLOR_FOREGROUND -1
#define COLOR_BACKGROUND -1

enum TABS {
    TAB_PLAYBACK = 0,
    TAB_RECORDING,
    TAB_OUTPUT,
    TAB_INPUT,
    TAB_CONFIGURATION
};

enum THEME {
    COLOR_BAR_LOW = 1,
    COLOR_BAR_MID,
    COLOR_BAR_HIGH,
    COLOR_VOLUME_LOW,
    COLOR_VOLUME_MID,
    COLOR_VOLUME_HIGH,
    COLOR_VOLUME_PEAK,
    COLOR_VOLUME_INDICATOR,
    COLOR_DEFAULT,
    COLOR_SELECTED,
    COLOR_DROPDOWN_SELECTED,
    COLOR_DROPDOWN_UNSELECTED,
    COLOR_BORDER
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
