#include "ui.hpp"

#include <locale.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>

#include "config.hpp"

#include "tabs/configuration.hpp"
#include "tabs/fallback.hpp"
#include "tabs/input.hpp"
#include "tabs/output.hpp"
#include "tabs/playback.hpp"
#include "tabs/recording.hpp"

#define KEY_ALT(x) (KEY_F(64 - 26) + ((x) - 'A'))

Ui ui;

constexpr char Ui::HELP_HEADER[];
constexpr char Ui::HELP_FOOTER[];

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
    hide_indicator = false;
    hide_top = false;
    hide_bottom = false;
    static_bar = false;
}

Ui::~Ui()
{
    delete current_tab;
}

int Ui::init(int tab)
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);

    noecho();
    nonl();
    raw();

    start_color();
    use_default_colors();

    std::string theme = std::string("theme." + config.getString("theme", "default"));

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

    bar[BAR_BG].assign(
        config.getString((theme + ".bar_style.bg").c_str(), u8"░")
    );
    bar[BAR_FG].assign(
        config.getString((theme + ".bar_style.fg").c_str(), u8"█")
    );

    if(!config.keyEmpty((theme + ".bar_style.indicator").c_str())) {
        bar[BAR_INDICATOR].assign(
            config.getString((theme + ".bar_style.indicator").c_str(), u8"█")
        );
    } else {
        hide_indicator = true;
    }
    if(!config.keyEmpty((theme + ".bar_style.top").c_str())) {
        bar[BAR_TOP].assign(
            config.getString((theme + ".bar_style.top").c_str(), u8"▁")
        );
    } else {
        hide_top = true;
    }
    if(!config.keyEmpty((theme + ".bar_style.bottom").c_str())) {
        bar[BAR_BOTTOM].assign(
            config.getString((theme + ".bar_style.bottom").c_str(), u8"▔")
        );
    } else {
        hide_bottom = true;
    }

    static_bar = config.getBool((theme + ".static_bar").c_str(), false);

    indicator.assign(
        config.getString((theme + ".default_indicator").c_str(), u8"♦ ")
    );

    running = true;
    switchTab(tab);

    getmaxyx(stdscr, height, width);

    statusbar = newwin(1, width, height - 1, 0);
    window = newwin(height - 1, width, 0, 0);

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
    if (!pulse.connected && !disconnect) {
        disconnect = true;
        delete current_tab;
        current_tab = new Fallback();
    } else if (pulse.connected && disconnect) {
        disconnect = false;
        switchTab(tab_index);
    }
}

void Ui::switchTab(int index)
{
    if (disconnect) {
        return;
    }

    delete current_tab;

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

#ifdef KEY_MOUSE
    mouseinterval(0);
#if NCURSES_MOUSE_VERSION > 1
    mousemask(BUTTON1_PRESSED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
#else
    mousemask(BUTTON1_PRESSED, NULL);
#endif
#endif

    int input = wgetch(window);
    std::string event{};

    if (input == ERR) {
        return;
    }

    switch (input) {
#ifdef KEY_MOUSE
        case KEY_MOUSE: {
            MEVENT mevent;
            int ok;

            ok = getmouse(&mevent);
            if (ok != OK) {
                return;
            }
            if (mevent.y == height - 1) {
                if (mevent.bstate & BUTTON1_PRESSED) {
                    int x = 0;
                    for (int i = 0; i < NUM_TABS; i++) {
                        int len = strlen(tabs[i]);
                        if (mevent.x >= x && mevent.x < x + len) {
                            switchTab(i);
                            return;
                        }
                        x += len + 1;
                    }
#if NCURSES_MOUSE_VERSION > 1
                } else if (mevent.bstate & BUTTON4_PRESSED) {
                    switchTab(++tab_index);
                    return;
                } else if (mevent.bstate & BUTTON5_PRESSED) {
                    switchTab(--tab_index);
                    return;
#endif
                }
            } else {
                int button = 0;
                if (mevent.bstate & BUTTON1_PRESSED) {
                    button = 1;
                }
#if NCURSES_MOUSE_VERSION > 1
                else if (mevent.bstate & BUTTON4_PRESSED) {
                    button = 4;
                } else if (mevent.bstate & BUTTON5_PRESSED) {
                    button = 5;
                }
#endif

                current_tab->handleMouse(mevent.x, mevent.y, button);
            }

            return;
        }
#endif
        case KEY_RESIZE:
            getmaxyx(stdscr, height, width);
            wresize(window, height - 1, width);

            mvwin(statusbar, height - 1, 0);
            wresize(statusbar, 1, width);

            return;

        case 27: { // Fix for alt/escape (also f-keys on some terminals)
            input = wgetch(window);

            if (input != -1 && input != 79) {
                std::string key = std::to_string(input);

                event = config.getString(
                            ("keycode.alt." +  key).c_str(),
                            "unbound"
                        );

                break;
            }

            if (input == 79) {
                input = wgetch(window);

                if (input != -1) {
                    std::string key = std::to_string(input);

                    event = config.getString(
                            ("keycode.f." +  key).c_str(),
                            "unbound"
                            );

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
                    );

            break;
    }

    if (!strcmp("unbound", event.c_str())) {
        return;
    }

    if (!strcmp("quit", event.c_str())) {
        kill();
    } else if (!strcmp("tab_next", event.c_str())) {
        switchTab(++tab_index);
    } else if (!strcmp("tab_prev", event.c_str())) {
        switchTab(--tab_index);
    } else if (!strcmp("tab_playback", event.c_str())) {
        switchTab(TAB_PLAYBACK);
    } else if (!strcmp("tab_recording", event.c_str())) {
        switchTab(TAB_RECORDING);
    } else if (!strcmp("tab_output", event.c_str())) {
        switchTab(TAB_OUTPUT);
    } else if (!strcmp("tab_input", event.c_str())) {
        switchTab(TAB_INPUT);
    } else if (!strcmp("tab_config", event.c_str())) {
        switchTab(TAB_CONFIGURATION);
    } else if (!strcmp("help", event.c_str())) {
        show_help();
    } else {
        current_tab->handleEvents(event.c_str());
    }
}

void Ui::show_help() {
    if (LINES < 10) return;
    if (COLS < 40) return;

    // Create a new pad. The size of the pad is defined by HELP_LINES.
    const auto& c { config.getKeycodeNameEvents() };
    const auto HELP_LINES = static_cast<int>(c.size());
    if (HELP_LINES < 1) return; // No help to show :(
    WINDOW *pad = newpad(HELP_LINES, COLS - 10);

    // Add some text to the pad
    int line = 0;
    //  max_length left-justifies the string and pads it with spaces to the right, ensuring 
    //  that the equals sign always appears at the same position
    int max_length = 0;
    for (const auto& it : c) {
        max_length = std::max(max_length, static_cast<int>(it.first.length()));
    }

    int start_pos = (getmaxx(pad) - (max_length + 10)) / 2; // 10 is an arbitrary number
    if (start_pos < 0) start_pos = 0; // Ensure start_pos is not negative
    for (const auto& it : c) {
        mvwprintw(pad, line++, start_pos,  "%-*s = %s", max_length, it.first.c_str(), it.second.c_str());
    }

    // Calculate the size and position of the window
    int window_height = LINES - 10;
    int window_width = COLS - 10;
    int window_top = (LINES - window_height) / 2;
    int window_left = (COLS - window_width) / 2;

    // Create a new window to display the pad
    WINDOW *win = newwin(window_height, window_width, window_top, window_left);
    box(win, 0, 0);

    // Add a title and a footer to the window
    mvwprintw(win, 0, (window_width - HELP_HEADER_SIZE) / 2, "%s", HELP_HEADER);
    mvwprintw(win, window_height - 1, (window_width - HELP_FOOTER_SIZE) / 2, "%s", HELP_FOOTER);

    wrefresh(win);

    int pad_top_line = 0;

    // Main loop
    while (true) {
        // Display the pad in the window
        prefresh(pad, pad_top_line, 0, window_top + 1, window_left + 1, window_top + window_height - 2, window_left + window_width - 2);

        // Wait for user input
        int ch = getch();
        if (ch == KEY_UP || ch == 'k') {
            // Up arrow key pressed. Check if the top of the pad is reached.
            if (pad_top_line > 0) {
                --pad_top_line;
            }
        } else if (ch == KEY_DOWN || ch == 'j') {
            // Down arrow key pressed. Check if the bottom of the pad is reached.
            if (pad_top_line < HELP_LINES - (window_height - 2)) {
                ++pad_top_line;
            }
        } else if (ch == 'q') {
            // 'q' key pressed. Exit the loop.
            break;
        }
    }

    // Delete the window and the pad
    delwin(win);
    delwin(pad);
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
