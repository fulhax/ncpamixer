#include "ui.hpp"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <string>

#include "config.hpp"

#include "tabs/playback.hpp"
#include "tabs/recording.hpp"
#include "tabs/output.hpp"
#include "tabs/input.hpp"
#include "tabs/configuration.hpp"

#define KEY_ALT(x) (KEY_F(64 - 26) + (x - 'A'))

Ui ui;

Ui::Ui()
{
    running = false;
    current_tab = nullptr;
    window = nullptr;
    statusbar = nullptr;
    width = 0;
    height = 0;
    tab_index = 0;
}

Ui::~Ui()
{
    if (current_tab != nullptr) {
        delete current_tab;
    }
}

int Ui::init()
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);

    noecho();
    nonl();
    raw();

    start_color();
    use_default_colors();

    // BG
    init_pair(1, COLOR_GREEN, -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_RED, -1);

    // FG
    init_pair(4, 0, COLOR_GREEN);
    init_pair(5, 0, COLOR_YELLOW);
    init_pair(6, 0, COLOR_RED);

    init_pair(7, COLOR_BLACK, COLOR_WHITE);

    running = true;
    current_tab = new Playback();

    getmaxyx(stdscr, height, width);

    statusbar = newwin(1, width, height - 1, 0);
    window = newwin(0, width, 0, 0);

    keypad(window, true);
    nodelay(window, true);
    idlok(window, true);

    erase();
    refresh();

    wrefresh(window);
    wrefresh(statusbar);

    return 1;
}

void Ui::switchTab(int index)
{
    if (current_tab != nullptr) {
        delete current_tab;
    }

    tab_index = index;

    switch (index) {
        default:
        case TAB_PLAYBACK:
            tab_index = TAB_PLAYBACK;
            current_tab = new Playback();
            break;

        case TAB_RECORDING:
            current_tab = new Recording();
            break;

        case TAB_OUTPUT:
            current_tab = new Output();
            break;

        case TAB_INPUT:
            current_tab = new Input();
            break;

        case -1:
        case TAB_CONFIGURATION:
            tab_index = TAB_CONFIGURATION;
            current_tab = new Configuration();
            break;
    }
}

void Ui::handleInput()
{
    set_escdelay(25);

    int input = wgetch(window);
    const char *event = 0;

    if (input == ERR) {
        return;
    }

    switch (input) {
        case KEY_RESIZE:
            getmaxyx(stdscr, height, width);
            wresize(window, 0, width);

            mvwin(statusbar, height - 1, 0);
            wresize(statusbar, 1, width);

            return;

        default:
            std::string key = std::to_string(input);
            event = config.getString(
                        ("keycode." +  key).c_str(),
                        "unbound"
                    ).c_str();

            if (!strcmp("unbound", event)) {
                return;
            }

            break;
    }

    if (!strcmp("quit", event)) {
        kill();
    } else if (!strcmp("tab_next", event)) {
        switchTab(++tab_index);
    } else if (!strcmp("tab_prev", event)) {
        switchTab(--tab_index);
    } else if (!strcmp("tab_playback", event)) {
        switchTab(TAB_PLAYBACK);
    } else if (!strcmp("tab_recording", event)) {
        switchTab(TAB_RECORDING);
    } else if (!strcmp("tab_output", event)) {
        switchTab(TAB_OUTPUT);
    } else if (!strcmp("tab_input", event)) {
        switchTab(TAB_INPUT);
    } else if (!strcmp("tab_config", event)) {
        switchTab(TAB_CONFIGURATION);
    } else {
        current_tab->handleEvents(event);
    }
}

void Ui::kill()
{
    endwin();
    running = false;
}

void Ui::draw()
{
    werase(window);
    current_tab->draw();
    wrefresh(window);

    statusBar();
}

void Ui::run()
{
    while (running) {
        draw();

        usleep(20000);

        handleInput();
    }
}

void Ui::statusBar()
{
    werase(statusbar);

    int len = 0;

    for (int i = 0; i < NUM_TABS; ++i) {
        if (tab_index == i) {
            wattron(statusbar, COLOR_PAIR(1));
        }

        mvwaddstr(statusbar, 0, len, tabs[i]);
        len += strlen(tabs[i]) + 1;

        if (tab_index == i) {
            wattroff(statusbar, COLOR_PAIR(1));
        }
    }

    wrefresh(statusbar);
}
