#include "view.hpp"

#include <locale.h>
#include <unistd.h>

#include <iterator>
#include <cstring>
#include <thread>
#include <map>

View::View(Pa *p)
{
    pa = p;

    x = 0;
    y = 0;

    draw = false;
    running = true;

    view = 0;
    selectedInputIndex = 0;

    win = nullptr;
    setInputVolume = nullptr;
    moveInputSink = nullptr;

    init();
    std::thread t1(&View::handleInput, this);
    t1.detach();
}

View::~View()
{
    running = false;
}

void View::init()
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
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

    refresh();
}

void View::run()
{
    draw = true;
    refresh();

    while (running) {
        std::unique_lock<std::mutex> lck(upd);
        cv.wait(lck, [this] {return draw == true;});

        erase();

        draw = false;

        int baseY = 3;

        for (auto &i : pa->PA_INPUTS) {
            double perc = static_cast<double>(i.second.volume) /
                          (PA_VOLUME_NORM * 2);

            volumeBar(0, baseY, perc, perc);

            if (i.first == selectedInputIndex) {
                attron(COLOR_PAIR(1));
            }

            char label[255];

            if (i.second.mute) {
                snprintf(
                    label,
                    sizeof(label),
                    "%s (muted)",
                    i.second.name
                );
            } else {
                snprintf(
                    label,
                    sizeof(label),
                    "%s (%d%%)",
                    i.second.name,
                    static_cast<int>(perc * 100)
                );
            }

            mvaddstr(baseY - 2, 1, label);
            char* name = pa->PA_SINKS.find(i.second.sink)->second.name;

            unsigned int len = strlen(name);
            unsigned int sink_pos = x - 1 - len;

            mvaddstr(
                baseY - 2,
                sink_pos,
                name
            );

            if (i.first == selectedInputIndex) {
                attroff(COLOR_PAIR(1));
            }

            baseY += 5;

        }

        drawStatusbar();
        refresh();
    }

    kill();
}

void View::notify_draw()
{
    draw = true;
    cv.notify_one();
}

void View::handleInput()
{
    while (running) {
        switch (getch()) {
            case 'q':
                this->running = false;
                notify_draw();
                break;

            case '^':
                // set 0%
                break;

            case '$':
                // set 100%
                break;

            case 'm':

                if (setInputVolume != nullptr && selectedInputIndex != 0) {
                    toggleInputMute(selectedInputIndex);
                }

                break;

            case 'g': {
                pa->inputMtx.lock();
                auto i = pa->PA_INPUTS.begin();

                if (i != pa->PA_INPUTS.end()) {
                    selectedInputIndex = i->first;
                }

                pa->inputMtx.unlock();

                notify_draw();
                break;
            }

            case 'G': {
                pa->inputMtx.lock();
                auto i = pa->PA_INPUTS.rbegin();

                if (i != pa->PA_INPUTS.rend()) {
                    selectedInputIndex = i->first;
                }

                pa->inputMtx.unlock();

                notify_draw();
                break;
            }

            case 'k': {
                pa->inputMtx.lock();

                auto i = std::prev(pa->PA_INPUTS.find(selectedInputIndex), 1);

                if (i != pa->PA_INPUTS.end()) {
                    selectedInputIndex = i->first;
                }

                pa->inputMtx.unlock();
                notify_draw();
                break;

            }

            case 'j': {
                pa->inputMtx.lock();

                auto i = std::next(pa->PA_INPUTS.find(selectedInputIndex), 1);

                if (i != pa->PA_INPUTS.end()) {
                    selectedInputIndex = i->first;
                }

                pa->inputMtx.unlock();
                notify_draw();
                break;
            }

            case 'r':
                break;

            case 'h':
                if (setInputVolume != nullptr && selectedInputIndex != 0) {
                    setInputVolume(selectedInputIndex, -1);
                }

                break;

            case KEY_RIGHT:
            case 'l':
                if (setInputVolume != nullptr && selectedInputIndex != 0) {
                    setInputVolume(selectedInputIndex, 1);
                }

                break;

            // Change sink on input
            case '\t': {

                if (moveInputSink != nullptr || selectedInputIndex == 0) {
                    pa->inputMtx.lock();
                    auto i = pa->PA_INPUTS.find(selectedInputIndex);

                    if (i != pa->PA_INPUTS.end()) {
                        auto currentSink = pa->PA_SINKS.find(i->second.sink);

                        currentSink = std::next(currentSink, 1);

                        if (currentSink == pa->PA_SINKS.end()) {
                            currentSink = pa->PA_SINKS.begin();
                        }

                        moveInputSink(selectedInputIndex, currentSink->first);
                    }
                }

                pa->inputMtx.unlock();
            }
            break;
        }


    }
}

void View::resize_view()
{
    endwin();  // finns det ngt bättre sätt?
    refresh();
    notify_draw();
}

unsigned int View::getVolumeColor(int p)
{
    if (p < 33) {
        return 1;
    } else if (p >= 33 && p < 66) {
        return 2;
    } else {
        return 3;
    }
}

void View::volumeBar(int px, int py, double vol, double peak)
{
    // light      ░ \u2593
    // medium     ▒ \u2592
    // dark shade ▓ \u2593
    // block      █ \u2588
    // lower      ▁ \u2581
    // higher     ▔ \u2594
    // triangle   ▲ \u25b2
    // https://en.wikipedia.org/wiki/Block_Elements

    getmaxyx(stdscr, y, x);
    double dx = static_cast<double>(x);

    int pw = dx * peak;
    int vw = dx * vol;
    int fw = x - pw;

    unsigned int color;

    fillW(0, py - 1, "\u2581");

    for (int i = 0; i < vw; i++) {
        color = getVolumeColor(
                    (static_cast<float>(i) / x) * 100
                );
        attron(COLOR_PAIR(color));
        mvaddstr(py, i, "\u2588");
        attroff(COLOR_PAIR(color));
    }

    for (int i = 0; i < fw; i++) {
        color = getVolumeColor(
                    (static_cast<float>(pw + i) / x) * 100
                );
        attron(COLOR_PAIR(color + 3));
        mvaddstr(py, pw + i, "\u2593");
        attroff(COLOR_PAIR(color + 3));
    }

    fillW(0, py + 1, "\u2594");
    mvaddstr(py, vw - 1, "\u2588"); // Mark volume

}

void View::fillW(int offset_x, int offset_y, const char *str)
{
    getmaxyx(stdscr, y, x);
    int w = (x - offset_x);

    for (int i = 0; i < w; i++) {
        mvaddstr(offset_y, offset_x + i, str);
    }
}

void View::drawStatusbar()
{
    getmaxyx(stdscr, y, x);
    mvaddstr(y - 1, 0, " [SINKS] |  INPUTS  ");
}

void View::kill()
{
    endwin();
}
