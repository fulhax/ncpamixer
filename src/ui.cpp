#include "ui.hpp"

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "tabs/playback.hpp"
#include "tabs/output.hpp"

Ui::Ui()
{
    running = false;
    current_tab = nullptr;
    width = 0;
    height = 0;
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
    nodelay(stdscr, TRUE);

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

void Ui::handleInput()
{
    unsigned int input = getch();

    switch (input) {
        case 'q':
            kill();
            return;

        case KEY_RESIZE:
            clear();
            refresh();

            getmaxyx(stdscr, height, width);
            break;

        case KEY_F(1): // extended keys
            if (current_tab != nullptr) {
                delete current_tab;
            }

            current_tab = new Playback();
            fprintf(stderr, "Extended %d\n", input);

            break;

        case KEY_F(3): // extended keys
            if (current_tab != nullptr) {
                delete current_tab;
            }

            current_tab = new Output();

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
