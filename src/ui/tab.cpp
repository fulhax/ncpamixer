#include "tab.hpp"

#include <string.h>
#include <ncurses.h>
#include <menu.h>

#include <string>
#include <vector>
#include <map>

#include "config.hpp"

void Tab::handleEvents(const char *event)
{
    if (object == nullptr) {
        return;
    }

    selected_index = pa.exists(*object, selected_index);

    if (selected_index == -1) {
        return;
    }

    auto pai = object->find(selected_index);

    PaObject *selected_pobj = nullptr;

    if (pai != object->end()) {
        selected_pobj = pai->second;
    }

    if (!strcmp("mute", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->toggle_mute();
        }
    } else if (!strcmp("move_first", event)) {
        auto i = object->begin();

        if (i != object->end()) {
            selected_index = i->first;
        }
    } else if (!strcmp("move_last", event)) {
        auto i = object->rbegin();

        if (i != object->rend()) {
            selected_index = i->first;
        }
    } else if (!strcmp("move_up", event)) {
        auto i = std::prev(object->find(selected_index), 1);

        if (i != object->end()) {
            selected_index = i->first;
        }
    } else if (!strcmp("move_down", event)) {
        auto i = std::next(object->find(selected_index), 1);

        if (i != object->end()) {
            selected_index = i->first;
        }
    } else if (!strcmp("volume_up", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->step_volume(1);
        }
    } else if (!strcmp("volume_down", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->step_volume(-1);
        }
    } else if (!strcmp("switch", event)) {
        if (selected_pobj != nullptr && toggle != nullptr) {
            auto current_toggle = toggle->find(selected_pobj->getSink());
            current_toggle = std::next(current_toggle, 1);

            if (current_toggle == toggle->end()) {
                current_toggle = toggle->begin();
            }

            selected_pobj->move(current_toggle->first);
        }
    } else if (!strcmp("dropdown", event)) {
        if (selected_pobj != nullptr && toggle != nullptr) {
            auto i = object->find(selected_index);

            if (i != object->end()) {
                uint32_t new_toggle = dropDown(
                                          1,
                                          1,
                                          *toggle,
                                          i->second->getSink()
                                      );
                selected_pobj->move(new_toggle);
            }
        }
    }
}

uint32_t Tab::dropDown(int x, int y, std::map<uint32_t, PaObject *> objects,
                       uint32_t current)
{
    if (objects.empty()) {
        return -1;
    }

    uint32_t selected = 0;
    int width = 0;
    int height = 0;

    std::vector<ITEM *> items;
    MENU *menu = 0;
    WINDOW *menu_win = 0;

    for (auto i : objects) {
        items.push_back(new_item(i.second->name, ""));
        set_item_userptr(items.back(), reinterpret_cast<void *>(i.second));

        if (i.first == current) {
            selected = items.size() - 1;
        }

        width = (width < strlen(i.second->name) + 3) ?
                strlen(i.second->name) :
                width;

        height = (height < 3) ? height + 1 : 3;
    }

    items.push_back(new_item(nullptr, nullptr));
    menu = new_menu(&items[0]);

    menu_win = newwin(height + 2, width + 2, y, x);
    keypad(menu_win, true);

    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, height + 1, width, 1, 1));
    set_menu_format(menu, height + 1, 1);

    //wbkgd(menu_win, COLOR_PAIR(7));
    //set_menu_back(menu, COLOR_PAIR(7));
    set_menu_fore(menu, COLOR_PAIR(4));
    //set_menu_grey(menu, COLOR_PAIR(7));

    //set_menu_mark(menu, "* ");
    set_menu_mark(menu, "");
    menu_opts_on(menu, O_ONEVALUE);
    menu_opts_off(menu, O_SHOWDESC);
    menu_opts_off(menu, O_NONCYCLIC);

    box(menu_win, 0, 0);
    post_menu(menu);
    set_current_item(menu, items[selected]);

    wrefresh(menu_win);

    bool selecting = true;

    while (selecting) {
        int input = wgetch(menu_win);
        const char *event = 0;

        if (input == ERR) {
            continue;
        }

        switch (input) {
            case KEY_RESIZE:
                selecting = false;
                continue;
            default:
                std::string key = std::to_string(input);
                event = config.getString(
                        ("keycode." +  key).c_str(),
                        "unbound"
                        ).c_str();

                if (!strcmp("unbound", event)) {
                    continue;
                }

                break;
        }

        if(!strcmp("select", event)) {
            clrtoeol();

            PaObject *object = reinterpret_cast<PaObject *>(
                    item_userptr(current_item(menu))
                    );

            selected = object->index;

            selecting = false;
        } else if(!strcmp("move_down", event)) {
            menu_driver(menu, REQ_DOWN_ITEM);
        } else if(!strcmp("move_up", event)) {
            menu_driver(menu, REQ_UP_ITEM);
        } else if(!strcmp("page_up", event)) {
            menu_driver(menu, REQ_SCR_UPAGE);
        } else if(!strcmp("page_down", event)) {
            menu_driver(menu, REQ_SCR_DPAGE);
        } else if(!strcmp("quit", event)) {
            selecting = false;
        }

        wrefresh(menu_win);
    }

    unpost_menu(menu);
    free_menu(menu);

    for (auto i : items) {
        free_item(i);
    }

    items.clear();
    refresh();

    return selected;
}

void Tab::borderBox(int w, int h, int px, int py)
{
    mvvline(py, px, ACS_VLINE, h);
    mvvline(py, px + w, ACS_VLINE, h);

    mvhline(py, px, ACS_HLINE, w);
    mvhline(py + h, px, ACS_HLINE, w);

    mvhline(py, px, ACS_ULCORNER, 1);
    mvhline(py, px + w, ACS_URCORNER, 1);

    mvhline(py + h, px, ACS_LLCORNER, 1);
    mvhline(py + h, px + w, ACS_LRCORNER, 1);

}

void Tab::selectBox(int w, int px, int py, bool selected)
{
    if (selected) {
        attron(COLOR_PAIR(1));
    }

    mvaddstr(py + 1, px + 2, "Digital Stereo (HDMI) Output");

    if (selected) {
        attroff(COLOR_PAIR(1));
    }

    borderBox(w, 2, px, py);
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
