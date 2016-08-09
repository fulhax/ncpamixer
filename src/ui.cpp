#include "ui.hpp"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "tabs/playback.hpp"
#include "tabs/recording.hpp"
#include "tabs/output.hpp"
#include "tabs/input.hpp"
#include "tabs/configuration.hpp"

#define KEY_ALT(x) (KEY_F(64 - 26) + (x - 'A'))

Ui::Ui()
{
    running = false;
    current_tab = nullptr;
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

    keypad(stdscr, true);
    nodelay(stdscr, true);

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
        case 0:
            tab_index = 0;
            current_tab = new Playback();
            break;

        case 1:
            current_tab = new Recording();
            break;

        case 2:
            current_tab = new Output();
            break;

        case 3:
            current_tab = new Input();
            break;

        case -1:
        case 4:
            tab_index = 4;
            current_tab = new Configuration();
            break;
    }
}

void Ui::handleInput()
{
    set_escdelay(25);
    int input = getch();

    if (input == ERR) {
        return;
    }

    switch (input) {
        case 'q':
            kill();
            return;

        case KEY_RESIZE:
            clear();
            refresh();

            getmaxyx(stdscr, height, width);
            break;

        case 'H':
            switchTab(--tab_index);
            break;

        case 'L':
            switchTab(++tab_index);
            break;

        case KEY_F(1):
            switchTab(0);
            break;

        case KEY_F(2):
            switchTab(1);
            break;

        case KEY_F(3):
            switchTab(2);
            break;

        case KEY_F(4):
            switchTab(3);
            break;

        case KEY_F(5):
            switchTab(4);
            break;

        default:
            current_tab->handleInput(input);
            break;
    }
}

void Ui::kill()
{
    endwin();
    running = false;
}

void Ui::draw()
{
    erase();

    current_tab->draw(width, height);
    statusBar();

    refresh();
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
    int len = 0;

    for (int i = 0; i < NUM_TABS; ++i) {
        if (tab_index == i) {
            attron(COLOR_PAIR(1));
        }

        mvaddstr(height - 1, len, tabs[i]);
        len += strlen(tabs[i]) + 1;

        if (tab_index == i) {
            attroff(COLOR_PAIR(1));
        }
    }
}
