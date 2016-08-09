#ifndef UI_HPP_
#define UI_HPP_

#include <ncursesw/ncurses.h>
#include <pulse/pulseaudio.h>

#include <condition_variable>
#include <vector>

#include "pa.hpp"
#include "tab.hpp"

class Ui
{
public:
    explicit Ui();
    virtual ~Ui();

    int init();
    void run();
    void handleInput();
    void kill();
    void draw();

    int width;
    int height;
private:
    bool running;

    Tab* current_tab;
    static void resize(int signum);
};

#endif // UI_HPP_
