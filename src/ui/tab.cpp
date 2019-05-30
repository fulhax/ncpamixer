#include "tab.hpp"

#include <menu.h>
#include <ncurses.h>

#ifdef __linux__
    #include <linux/limits.h>
#endif

#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "config.hpp"
#include "ui.hpp"

void Tab::draw()
{
    if (object == nullptr) {
        return;
    }

    int baseY = 0;
    int current_block = 0;
    int BLOCK_SIZE = 5;

    if (has_volume) {
        if (ui.hide_top) {
            BLOCK_SIZE -= 1;
        }

        if (ui.hide_bottom) {
            BLOCK_SIZE -= 1;
        }
    }

    total_blocks = (ui.height - 2) / BLOCK_SIZE;
    int blocks_drawn = 0;

    bool more_up = false;
    bool more_down = false;

    for (auto &i : *object) {
        if (current_block <= selected_block - total_blocks) {
            current_block++;
            more_up = true;
            continue;
        }

        if (blocks_drawn >= total_blocks) {
            more_down = true;
            break;
        }

        blocks_drawn++;

        float perc = static_cast<float>(i.second->getVolume()) / (audio->getVolumeNorm() * 1.5f);

        if (has_volume) {
            if (ui.static_bar) {
                volumeBar(
                    ui.width,
                    ui.height,
                    0,
                    baseY + 3,
                    perc,
                    perc
                );
            } else {
                volumeBar(
                    ui.width,
                    ui.height,
                    0,
                    baseY + 3,
                    perc,
                    i.second->getPeak()
                );
            }
        } else { // Configuration
            if (i.first == selected_index) {
                wattron(ui.window, COLOR_PAIR(COLOR_SELECTED));
            } else {
                wattron(ui.window, COLOR_PAIR(COLOR_DEFAULT));
            }

            auto attr = i.second->getActiveAttribute();
            if (attr != nullptr) {
                mvwaddstr(
                    ui.window,
                    baseY + 3,
                    3,
                    &attr->description[0]
                );
                borderBox(ui.width - 2, 2, 1, baseY + 2);
            }

            if (i.first == selected_index) {
                wattroff(ui.window, COLOR_PAIR(COLOR_SELECTED));
            } else {
                wattroff(ui.window, COLOR_PAIR(COLOR_DEFAULT));
            }
        }

        if (current_block == selected_block) {
            wattron(ui.window, COLOR_PAIR(COLOR_SELECTED));
        } else {
            wattron(ui.window, COLOR_PAIR(COLOR_DEFAULT));
        }

        auto rel = i.second->getRelation();
        uint16_t toggle_len = 0;

        if (toggle != nullptr && rel != UINT32_MAX) {
            auto curr = toggle->find(rel);

            if (curr != toggle->end() && curr->second != nullptr) {
                uint16_t len = curr->second->getName().length();

                if (len > 0) {
                    uint16_t sink_pos = ui.width - 1 - len;

                    mvwaddstr(
                        ui.window,
                        baseY + 1,
                        sink_pos,
                        curr->second->getName().c_str()
                    );

                    toggle_len += len;
                }
            }
        } else {
            auto attr = i.second->getActiveAttribute();

            if (attr != nullptr && has_volume) {
                unsigned int len = attr->description.length();
                unsigned int sink_pos = ui.width - 1 - len;

                mvwaddstr(
                    ui.window,
                    baseY + 1,
                    sink_pos,
                    attr->description.c_str()
                );

                toggle_len += len;
            }
        }

        std::string app;

        if (i.second->getAppName().length() > 0) {
            app = i.second->getAppName() + ": " + i.second->getName();
        } else {
            app = i.second->getName();
        }

        if (i.second->isDefault()) {
            app.insert(0, ui.indicator);
        }

        std::string label;
        bool dots = false;

        std::string output_volume;

        if (has_volume && i.second->getMuted()) {
            output_volume = " (muted)";
        } else if(has_volume) {
            char vol[32] = {0};
            snprintf(
                &vol[0],
                sizeof(vol),
                "%.0f",
                perc * (1.5F * 100.F) + 0.1F
            );

            output_volume =  " (" + std::string(&vol[0]) + "%)";
        }

        while (true) {
            label = app + ((dots) ? "..." : "");
            label.append(output_volume);

            int output_len = label.length();
            int volume_len = (has_volume) ? 4 : 1;

            if ((output_len + toggle_len + volume_len) > ui.width) {
                if (app.length() > 0) {
                    app.resize(app.length() - 1);
                } else {
                    break;
                }

                dots = true;
            } else {
                break;
            }
        }

        mvwaddstr(ui.window, baseY + 1, 1, label.c_str());

        if (current_block == selected_block) {
            wattroff(ui.window, COLOR_PAIR(COLOR_SELECTED));
        } else {
            wattroff(ui.window, COLOR_PAIR(COLOR_DEFAULT));
        }

        baseY += BLOCK_SIZE;
        current_block++;
    }

    if (more_up) {
        mvwaddstr(
            ui.window,
            0,
            (ui.width / 2) - 4,
            "\u2191\u2191\u2191\u2191"
        );
    }

    if (more_down) {
        mvwaddstr(
            ui.window,
            ui.height - 2,
            (ui.width / 2) - 4,
            "\u2193\u2193\u2193\u2193"
        );
    }
}

void Tab::handleEvents(const char *event)
{
    if (object == nullptr) {
        return;
    }

    selected_index = audio->exists(*object, selected_index);

    if (selected_index == UINT32_MAX) {
        return;
    }

    int BLOCK_SIZE = 5;

    if (has_volume) {
        if (ui.hide_top) {
            BLOCK_SIZE -= 1;
        }

        if (ui.hide_bottom) {
            BLOCK_SIZE -= 1;
        }
    }

    auto pai = object->find(selected_index);

    AudioObject *selected_pobj = nullptr;

    if (pai != object->end()) {
        selected_pobj = pai->second;
    }

    if (selected_pobj != nullptr) {
        if (strcmp("mute", event) == 0) {
            selected_pobj->toggleMute();
        }  else if (strcmp("volume_up", event) == 0) {
            selected_pobj->stepVolume(1);
        } else if (strcmp("volume_down", event) == 0) {
            selected_pobj->stepVolume(-1);
        } else if (strcmp("set_volume_0", event) == 0) {
            selected_pobj->setVolume(0);
        } else if (strcmp("set_volume_10", event) == 0) {
            selected_pobj->setVolume(.1F);
        } else if (strcmp("set_volume_20", event) == 0) {
            selected_pobj->setVolume(.2F);
        } else if (strcmp("set_volume_30", event) == 0) {
            selected_pobj->setVolume(.3F);
        } else if (strcmp("set_volume_40", event) == 0) {
            selected_pobj->setVolume(.4F);
        } else if (strcmp("set_volume_50", event) == 0) {
            selected_pobj->setVolume(.5F);
        } else if (strcmp("set_volume_60", event) == 0) {
            selected_pobj->setVolume(.6F);
        } else if (strcmp("set_volume_70", event) == 0) {
            selected_pobj->setVolume(.7F);
        } else if (strcmp("set_volume_80", event) == 0) {
            selected_pobj->setVolume(.8F);
        } else if (strcmp("set_volume_90", event) == 0) {
            selected_pobj->setVolume(.9F);
        } else if (strcmp("set_volume_100", event) == 0) {
            selected_pobj->setVolume(1.0f);
        } else if (strcmp("set_default", event) == 0) {
            selected_pobj->switchDefault();
        } else if (strcmp("switch", event) == 0) {
            if (toggle != nullptr) {
                auto current_toggle = toggle->find(selected_pobj->getRelation());
                current_toggle = std::next(current_toggle, 1);

                if (current_toggle == toggle->end()) {
                    current_toggle = toggle->begin();
                }

                selected_pobj->move(current_toggle->first);
            } else {
                selected_pobj->switchNextAttribute();
            }
        } else if (strcmp("dropdown", event) == 0) {
            uint32_t selected = 0;

            if (toggle != nullptr) {
                selected = dropDown(
                               -1,
                               std::min(
                                   selected_block * (BLOCK_SIZE),
                                   (total_blocks - 1) * BLOCK_SIZE
                               ),
                               *toggle,
                               selected_pobj->getRelation()
                           );

                if (selected != UINT32_MAX) {
                    selected_pobj->move(selected);
                }
            } else {
                uint32_t w = 0;
                int x = 0;

                int y = std::min(
                            selected_block * (BLOCK_SIZE),
                            (total_blocks - 1) * BLOCK_SIZE
                        );


                if (has_volume) {
                    x = -1;
                } else {
                    x = 1;
                    w = ui.width - 3;
                    y += 2;
                }

                selected = dropDown(
                               x,
                               y,
                               selected_pobj->getAttributes(),
                               selected_pobj->getRelation(),
                               w
                           );

                if (selected != UINT32_MAX) {
                    selected_pobj->switchActiveAttribute(
                        selected_pobj->getAttribute(selected)->name
                    );
                }
            }
        }
    }

    if (strcmp("move_first", event) == 0) {
        auto i = object->begin();

        if (i != object->end()) {
            selected_index = i->first;
            selected_block = 0;
        }
    } else if (strcmp("move_last", event) == 0) {
        auto i = object->rbegin();

        if (i != object->rend()) {
            selected_index = i->first;
            selected_block = object->size() - 1;
        }
    } else if (strcmp("move_up", event) == 0) {
        auto i = std::prev(object->find(selected_index), 1);

        if (i != object->end()) {
            selected_index = i->first;
            selected_block = (selected_block > 0) ? selected_block - 1 : 0;
        }
    } else if (strcmp("move_down", event) == 0) {
        auto i = std::next(object->find(selected_index), 1);

        if (i != object->end()) {
            selected_index = i->first;
            selected_block++;
        }
    } else if (strcmp("toggle_static", event) == 0) {
        ui.static_bar = !ui.static_bar;
    }
}

uint32_t Tab::dropDown(
    int x,
    int y,
    AudioObjectAttributes attributes,
    uint32_t current,
    int width,
    int height
)
{
    if (attributes.empty()) {
        return UINT32_MAX;
    }

    std::map<uint32_t, std::string> tmp;

    for (size_t i = 0; i < attributes.size(); i++) {
        tmp[i] = attributes[i]->description;
    }

    return dropDown(x, y, tmp, current, width, height);
}

uint32_t Tab::dropDown(
    int x,
    int y,
    AudioObjects objects,
    uint32_t current,
    int width,
    int height
)
{
    if (objects.empty()) {
        return UINT32_MAX;
    }

    std::map<uint32_t, std::string> tmp;
    std::for_each(
        objects.begin(),
        objects.end(),
        [&tmp](auto const & obj) {
            tmp[obj.first] = obj.second->getName();
        }
    );

    return dropDown(x, y, tmp, current, width, height);
}

uint32_t Tab::dropDown(
    int x,
    int y,
    std::map<uint32_t, std::string> objects,
    uint32_t current,
    int width,
    int height
)
{
    if (objects.empty()) {
        return UINT32_MAX;
    }

    bool autowidth = false;
    bool autoheight = false;

    if (width == 0) {
        autowidth = true;
    }

    if (height == 0) {
        autoheight = true;
    }

    uint32_t selected = 0;

    std::vector<ITEM *> items;
    MENU *menu = nullptr;
    WINDOW *menu_win = nullptr;

    for (auto &i : objects) {
        ITEM *item = new ITEM;
        memset(item, 0, sizeof(ITEM));

        item->opt = O_SELECTABLE;

        int len = i.second.length();
        if (len > ui.width - 4) {
            len = ui.width - 4;
        }

        item->name.str = new char[len + 1];
        memcpy(
            const_cast<char *>(item->name.str),
            i.second.c_str(),
            len
        );
        item->name.length = len;
        item->userptr = static_cast<void *>(&i);

        items.push_back(item);

        if (i.first == current) {
            selected = items.size() - 1;
        }

        if (autowidth) {
            width = (width < len + 3) ? len : width;
        }

        if (autoheight) {
            height = (height < 5) ? height + 1 : 5;
        }
    }

    items.push_back(new_item(nullptr, nullptr));
    menu = new_menu(&items[0]);

    if (x < 0) {
        x = ui.width - (width + 2);
    }

    menu_win = newwin(height + 2, width + 2, y, x);
    keypad(menu_win, true);

    set_menu_win(menu, menu_win);
    set_menu_sub(menu, derwin(menu_win, height + 1, width, 1, 1));
    set_menu_format(menu, height, 1);

    wbkgd(menu_win, COLOR_PAIR(COLOR_BORDER));
    //set_menu_back(menu, COLOR_PAIR(7));
    set_menu_fore(menu, COLOR_PAIR(COLOR_DROPDOWN_SELECTED));
    set_menu_grey(menu, COLOR_PAIR(COLOR_DROPDOWN_UNSELECTED));

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
        const char *event = nullptr;

        if (input == ERR) {
            continue;
        }

        if (input == KEY_RESIZE) {
            selecting = false;
            continue;
        }

        std::string key = std::to_string(input);
        event = config.getString(("keycode." +  key).c_str(), "unbound").c_str();

        if (strcmp("unbound", event) == 0) {
            continue;
        }

        if (strcmp("select", event) == 0) {
            clrtoeol();

            ITEM *item = current_item(menu);
            selected = static_cast<std::pair<uint32_t, std::string>*>(
                           item->userptr
                       )->first;

            selecting = false;
        } else if (strcmp("move_down", event) == 0) {
            menu_driver(menu, REQ_DOWN_ITEM);
        } else if (strcmp("move_up", event) == 0) {
            menu_driver(menu, REQ_UP_ITEM);
        } else if (strcmp("page_up", event) == 0) {
            menu_driver(menu, REQ_SCR_UPAGE);
        } else if (strcmp("page_down", event) == 0) {
            menu_driver(menu, REQ_SCR_DPAGE);
        } else if (strcmp("quit", event) == 0) {
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
    wattron(ui.window, COLOR_PAIR(COLOR_BORDER));

    mvwvline(ui.window, py, px, ACS_VLINE, h);
    mvwvline(ui.window, py, px + w, ACS_VLINE, h);

    mvwhline(ui.window, py, px, ACS_HLINE, w);
    mvwhline(ui.window, py + h, px, ACS_HLINE, w);

    mvwhline(ui.window, py, px, ACS_ULCORNER, 1);
    mvwhline(ui.window, py, px + w, ACS_URCORNER, 1);

    mvwhline(ui.window, py + h, px, ACS_LLCORNER, 1);
    mvwhline(ui.window, py + h, px + w, ACS_LRCORNER, 1);

    wattroff(ui.window, COLOR_PAIR(COLOR_BORDER));
}

void Tab::volumeBar(int w, int h, int px, int py, float vol, float peak)
{
    auto dw = static_cast<float>(w);

    int pw = static_cast<int>(dw * peak);
    int vw = static_cast<int>(dw * vol);
    int fw = w - pw;

    unsigned int color;

    if (!ui.hide_top) {
        fillW(w, h, 0, py - 1, ui.bar[BAR_TOP].c_str());
    } else {
        py -= 1;
    }

    for (int i = 0; i < pw; i++) {
        if (i >= vw) {
            color = COLOR_VOLUME_PEAK;
        } else {
            color = getVolumeColor(static_cast<int>(
                static_cast<float>(i) / w * 100.0f
            ));
        }

        wattron(ui.window, COLOR_PAIR(color));
        mvwaddstr(ui.window, py, i, ui.bar[BAR_FG].c_str());
        wattroff(ui.window, COLOR_PAIR(color));
    }

    for (int i = 0; i < fw; i++) {
        color = getBarColor(static_cast<int>(
            static_cast<float>(pw + i) / w * 100.0f
        ));

        wattron(ui.window, COLOR_PAIR(color));
        mvwaddstr(ui.window, py, pw + i, ui.bar[BAR_BG].c_str());
        wattroff(ui.window, COLOR_PAIR(color));
    }

    if (!ui.hide_bottom) {
        fillW(w, h, 0, py + 1, ui.bar[BAR_BOTTOM].c_str());
    }

    if (!ui.hide_indicator) {
        wattron(ui.window, COLOR_PAIR(COLOR_VOLUME_INDICATOR));

        mvwaddstr(
            ui.window,
            py,
            vw - 1,
            ui.bar[BAR_INDICATOR].c_str()
        ); // Mark volume

        wattroff(ui.window, COLOR_PAIR(COLOR_VOLUME_INDICATOR));
    }
}

unsigned int Tab::getVolumeColor(int p)
{
    if (p < 33) {
        return COLOR_VOLUME_LOW;
    } else if (p < 66) {
        return COLOR_VOLUME_MID;
    } else {
        return COLOR_VOLUME_HIGH;
    }
}

unsigned int Tab::getBarColor(int p)
{
    if (p < 33) {
        return COLOR_BAR_LOW;
    } else if (p < 66) {
        return COLOR_BAR_MID;
    } else {
        return COLOR_BAR_HIGH;
    }
}

void Tab::fillW(int w, int h, int offset_x, int offset_y, const char *str)
{
    int wo = (w - offset_x);

    wattron(ui.window, COLOR_PAIR(COLOR_BORDER));

    for (int i = 0; i < wo; i++) {
        mvwaddstr(ui.window, offset_y, offset_x + i, str);
    }

    wattroff(ui.window, COLOR_PAIR(COLOR_BORDER));
}
