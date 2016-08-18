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
#include "tabs/fallback.hpp"

#define KEY_ALT(x) (KEY_F(64 - 26) + (x - 'A'))

Ui ui;

Ui::Ui()
{
    disconnect = false;
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

    std::string theme = std::string(
                            "theme." + config.getString("theme", "default")
                        );

    init_pair(
        COLOR_BAR_LOW,
        config.getInt((theme + ".bar_low.front").c_str(), COLOR_BLACK),
        config.getInt((theme + ".bar_low.back").c_str(), COLOR_GREEN)
    );
    init_pair(
        COLOR_BAR_MID,
        config.getInt((theme + ".bar_mid.front").c_str(), COLOR_BLACK),
        config.getInt((theme + ".bar_mid.back").c_str(), COLOR_YELLOW)
    );
    init_pair(
        COLOR_BAR_HIGH,
        config.getInt((theme + ".bar_high.front").c_str(), COLOR_BLACK),
        config.getInt((theme + ".bar_high.back").c_str(), COLOR_RED)
    );
    init_pair(
        COLOR_VOLUME_LOW,
        config.getInt((theme + ".volume_low").c_str(), COLOR_GREEN),
        COLOR_BACKGROUND
    );
    init_pair(
        COLOR_VOLUME_MID,
        config.getInt((theme + ".volume_mid").c_str(), COLOR_YELLOW),
        COLOR_BACKGROUND
    );
    init_pair(
        COLOR_VOLUME_HIGH,
        config.getInt((theme + ".volume_high").c_str(), COLOR_RED),
        COLOR_BACKGROUND
    );
    init_pair(
        COLOR_VOLUME_PEAK,
        config.getInt((theme + ".volume_peak").c_str(), COLOR_RED),
        COLOR_BACKGROUND
    );
    init_pair(
        COLOR_VOLUME_INDICATOR,
        config.getInt((theme + ".volume_indicator").c_str(), COLOR_FOREGROUND),
        COLOR_BACKGROUND
    );
    init_pair(
        COLOR_SELECTED,
        config.getInt((theme + ".selected").c_str(), COLOR_GREEN),
        COLOR_BACKGROUND
    );
    init_pair(
        COLOR_DEFAULT,
        config.getInt((theme + ".default").c_str(), COLOR_FOREGROUND),
        COLOR_BACKGROUND
    );
    init_pair(
        COLOR_BORDER,
        config.getInt((theme + ".border").c_str(), COLOR_FOREGROUND),
        COLOR_BACKGROUND
    );

    init_pair(
        COLOR_DROPDOWN_SELECTED,
        config.getInt((theme + ".dropdown.selected_text").c_str(), COLOR_BLACK),
        config.getInt((theme + ".dropdown.selected").c_str(), COLOR_GREEN)
    );

    init_pair(
        COLOR_DROPDOWN_UNSELECTED,
        config.getInt((theme + ".dropdown.unselected").c_str(), COLOR_FOREGROUND),
        COLOR_BACKGROUND
    );

    // FG
    // init_pair(4, 0, COLOR_GREEN);
    // init_pair(5, 0, COLOR_YELLOW);
    // init_pair(6, 0, COLOR_RED);
    //
    // init_pair(7, COLOR_BLACK, COLOR_WHITE);

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

void Ui::checkPulseAudio()
{
    if(!pa.connected && !disconnect) {
        disconnect = true;
        current_tab = new Fallback();
    } else if(pa.connected && disconnect){
        disconnect = false;
        switchTab(tab_index);
    }
}

void Ui::switchTab(int index)
{
    if(disconnect) {
        return;
    }

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
    set_escdelay(0);

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

        case 27: { // Fix for alt/escape (also f-keys on some terminals)
            nodelay(window, true);
            input = wgetch(window);
            nodelay(window, false);

            if (input != -1 && input != 79) {
                std::string key = std::to_string(input);

                event = config.getString(
                            ("keycode.alt." +  key).c_str(),
                            "unbound"
                        ).c_str();

                break;
            } else if (input == 79) {
                nodelay(window, true);
                input = wgetch(window);
                nodelay(window, false);

                if (input != -1) {
                    std::string key = std::to_string(input);

                    event = config.getString(
                                ("keycode.f." +  key).c_str(),
                                "unbound"
                            ).c_str();

                    break;
                }
            }

            input = 27;
        }

        default:
            std::string key = std::to_string(input);
            event = config.getString(
                        ("keycode." +  key).c_str(),
                        "unbound"
                    ).c_str();

            break;
    }

    if (!strcmp("unbound", event)) {
        return;
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
        checkPulseAudio();
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
            wattron(statusbar, COLOR_PAIR(COLOR_SELECTED));
        } else {
            wattron(statusbar, COLOR_PAIR(COLOR_DEFAULT));
        }

        mvwaddstr(statusbar, 0, len, tabs[i]);
        len += strlen(tabs[i]) + 1;

        if (tab_index == i) {
            wattroff(statusbar, COLOR_PAIR(COLOR_SELECTED));
        } else {
            wattroff(statusbar, COLOR_PAIR(COLOR_DEFAULT));
        }
    }

    wrefresh(statusbar);
}
