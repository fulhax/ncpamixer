#ifndef UI_HPP_
#define UI_HPP_

#define _XOPEN_SOURCE_EXTENDED 1

#include <ncurses.h>

#include <condition_variable>
#include <vector>

#include "pa.hpp"
#include "tab.hpp"

#define NUM_TABS 5

#define COLOR_FOREGROUND (-1)
#define COLOR_BACKGROUND (-1)

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

enum BAR {
    BAR_BG = 0,
    BAR_FG,
    BAR_INDICATOR,
    BAR_TOP,
    BAR_BOTTOM,
    BAR_SIZE,
};

class Ui
{
public:
    Ui();
    virtual ~Ui();

    int init(int tab);
    void run();

    int width;
    int height;

    WINDOW *window;
    WINDOW *statusbar;

    std::string bar[BAR_SIZE + 1];
    std::string indicator;

    bool hide_indicator;
    bool hide_top;
    bool hide_bottom;
    bool static_bar;
private:
    bool running;
    bool disconnect;

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
    void checkPulseAudio();
};

extern Ui ui;

#endif // UI_HPP_
