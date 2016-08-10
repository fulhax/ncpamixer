#include "tab.hpp"

#include <string.h>
#include <ncurses.h>
#include <menu.h>

#include <vector>
#include <map>

void Tab::dropDown(std::map<uint32_t, PaObject *> objects, uint32_t current)
{
    if(objects.empty()) {
        return;
    }

    static int selected = 0;
    int width = 0;
    int height = 0;

    std::vector<ITEM *> items;
    MENU *menu = 0;
    WINDOW *menu_win = 0;

    for (auto i : objects) {
        char *tmp = new char[strlen(i.second->name) + 1];
        snprintf(tmp, strlen(i.second->name) + 1, "%s", i.second->name);
        items.push_back(new_item(tmp, ""));

        set_item_userptr(
            items.back(),
            reinterpret_cast<void *>(const_cast<uint32_t *>(&i.first))
        );

        if (i.first == current) {
            selected = items.size() - 1;

        }

        width = (width < strlen(i.second->name) + 3) ?
                strlen(i.second->name) :
                width;

        height = (height < 3) ? height + 1 : 3;
    }

    menu = new_menu(&items[0]);

    menu_win = newwin(height + 2, width + 2, 1, 1);
    keypad(menu_win, true);

    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, height + 1, width, 1, 1));
    set_menu_format(menu, height + 1, 1);

    //set_menu_mark(menu, "* ");
    set_menu_mark(menu, "");
    menu_opts_on(menu, O_ONEVALUE);
    menu_opts_off(menu, O_SHOWDESC);

    box(menu_win, 0, 0);
    post_menu(menu);
    set_current_item(menu, items[selected]);

    wrefresh(menu_win);

    bool selecting = true;

    while (selecting) {
        int c = wgetch(menu_win);

        switch (c) {
            case 10:
            case '\r':
            case KEY_ENTER:
                clrtoeol();
                fprintf(
                    stderr,
                    "Item selected is : %d\n",
                    *reinterpret_cast<uint32_t *>(
                        item_userptr(current_item(menu))
                    )
                );

            case 27:
                selecting = false;
                break;

            case 'j':
                menu_driver(menu, REQ_DOWN_ITEM);
                break;

            case 'k':
                menu_driver(menu, REQ_UP_ITEM);
                break;

            case KEY_NPAGE:
                menu_driver(menu, REQ_SCR_DPAGE);
                break;

            case KEY_PPAGE:
                menu_driver(menu, REQ_SCR_UPAGE);
                break;
        }

        wrefresh(menu_win);
    }

    free_menu(menu);

    for (auto i : items) {
        free_item(i);
    }

    items.clear();
    refresh();
}

void Tab::volumeBar(int w, int h, int px, int py, float vol, float peak)
{
    // light      ░ \u2593
    // medium     ▒ \u2592
    // dark shade ▓ \u2593
    // block      █ \u2588
    // lower      ▁ \u2581
    // higher     ▔ \u2594
    // triangle   ▲ \u25b2
    // https://en.wikipedia.org/wiki/Block_Elements

    float dw = static_cast<float>(w);

    int pw = dw * peak;
    int vw = dw * vol;
    int fw = w - pw;

    unsigned int color;

    fillW(w, h, 0, py - 1, "\u2581");

    for (int i = 0; i < pw; i++) {
        if (i >= vw) {
            color = 3;
        } else {
            color = getVolumeColor(
                        (static_cast<float>(i) / w) * 100
                    );
        }

        attron(COLOR_PAIR(color));
        mvaddstr(py, i, "\u2588");
        attroff(COLOR_PAIR(color));
    }

    for (int i = 0; i < fw; i++) {
        color = getVolumeColor(
                    (static_cast<float>(pw + i) / w) * 100
                );
        attron(COLOR_PAIR(color + 3));
        mvaddstr(py, pw + i, "\u2593");
        attroff(COLOR_PAIR(color + 3));
    }

    fillW(w, h, 0, py + 1, "\u2594");
    mvaddstr(py, vw - 1, "\u2588"); // Mark volume

}

unsigned int Tab::getVolumeColor(int p)
{
    if (p < 33) {
        return 1;
    } else if (p >= 33 && p < 66) {
        return 2;
    } else {
        return 3;
    }
}

void Tab::fillW(int w, int h, int offset_x, int offset_y, const char *str)
{
    int wo = (w - offset_x);

    for (int i = 0; i < wo; i++) {
        mvaddstr(offset_y, offset_x + i, str);
    }
}
