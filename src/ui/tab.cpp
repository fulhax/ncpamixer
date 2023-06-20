#include "tab.hpp"

#include <ncurses.h>
#include <menu.h>

#include <algorithm>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "config.hpp"
#include "ui.hpp"

int Tab::getBlockSize()
{
    int BLOCK_SIZE = 5;

    if (has_volume) {
        if (ui.hide_top) {
            BLOCK_SIZE -= 1;
        }

        if (ui.hide_bottom) {
            BLOCK_SIZE -= 1;
        }
    }

    return BLOCK_SIZE;
}

void Tab::draw()
{
    if (object == nullptr) {
        return;
    }

    int baseY = 0;
    int current_block = 0;
    int BLOCK_SIZE = getBlockSize();

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

        float perc = static_cast<float>(i.second->volume) /
                     (PA_VOLUME_NORM * 1.5f);

        if (has_volume) {
            if (ui.static_bar) {
                volumeBar(ui.width, ui.height, 0, baseY + 3, perc, perc);
            } else {
                volumeBar(ui.width, ui.height, 0, baseY + 3, perc, i.second->peak);
            }
        } else { // Configuration
            if (i.first == selected_index) {
                wattron(ui.window, COLOR_PAIR(COLOR_SELECTED));
            } else {
                wattron(ui.window, COLOR_PAIR(COLOR_DEFAULT));
            }

            if (i.second->active_attribute != nullptr) {
                mvwaddstr(
                    ui.window,
                    baseY + 3,
                    3,
                    i.second->active_attribute->description
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

        char label[255] = {0};
        std::string app = {0};

        int toggle_len = 0;

        if (toggle != nullptr) {
            auto rel = toggle->find(i.second->getRelation());

            if (rel != toggle->end()) {
                char *name = rel->second->name;

                if (name != nullptr) {
                    unsigned int len = strlen(name);
                    unsigned int sink_pos = ui.width - 1 - len;

                    mvwaddstr(
                        ui.window,
                        baseY + 1,
                        sink_pos,
                        name
                    );

                    toggle_len += strlen(name);
                }
            }
        } else {
            if (i.second->active_attribute != nullptr && has_volume) {
                unsigned int len = strlen(
                                       i.second->active_attribute->description
                                   );

                unsigned int sink_pos = ui.width - 1 - len;

                mvwaddstr(
                    ui.window,
                    baseY + 1,
                    sink_pos,
                    i.second->active_attribute->description
                );

                toggle_len += strlen(i.second->active_attribute->description);
            }
        }

        const char *app_name = i.second->getAppName();

        if (app_name != nullptr && strlen(i.second->getAppName()) > 0) {
            app = std::string(i.second->getAppName()) + ": " +
                  std::string(i.second->name);
        } else {
            app = i.second->name;
        }

        if (i.second->is_default) {
            app = ui.indicator + app;
        }

        bool dots = false;

        while (1) {
            if (has_volume) {
                if (i.second->mute) {
                    snprintf(
                        label,
                        sizeof(label),
                        "%s%s (muted)",
                        app.c_str(),
                        (dots) ? "..." : ""
                    );
                } else {
                    snprintf(
                        label,
                        sizeof(label),
                        "%s%s (%d%%)",
                        app.c_str(),
                        (dots) ? "..." : "",
                        static_cast<int>(perc * (1.5f * 100.f) + 0.1f)
                    );
                }
            } else {
                snprintf(
                    label,
                    sizeof(label),
                    "%s%s",
                    app.c_str(),
                    (dots) ? "..." : ""
                );
            }

            int output_len = strlen(label);
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

        mvwaddstr(ui.window, baseY + 1, 1, label);

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

void Tab::handleEvents(const char* const &event)
{
    if (object == nullptr) {
        return;
    }

    selected_index = pulse.exists(*object, selected_index);

    if (selected_index == static_cast<uint32_t>(-1)) {
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
            selected_block = 0;
        }

    } else if (!strcmp("move_last", event)) {
        auto i = object->rbegin();

        if (i != object->rend()) {
            selected_index = i->first;
            selected_block = object->size() - 1;
        }
    } else if (!strcmp("move_up", event)) {
        auto i = std::prev(object->find(selected_index), 1);

        if (i != object->end()) {
            selected_index = i->first;
            selected_block = (selected_block > 0) ? selected_block - 1 : 0;
        }
    } else if (!strcmp("move_down", event)) {
        auto i = std::next(object->find(selected_index), 1);

        if (i != object->end()) {
            selected_index = i->first;
            selected_block++;
        }
    } else if (!strcmp("toggle_static", event)) {
        ui.static_bar = !ui.static_bar;
    } else if (!strcmp("volume_up", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->step_volume(1);
        }
    } else if (!strcmp("volume_down", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->step_volume(-1);
        }
    } else if (!strcmp("set_volume_0", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(0);
        }
    } else if (!strcmp("set_volume_10", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.1f);
        }
    } else if (!strcmp("set_volume_20", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.2f);
        }
    } else if (!strcmp("set_volume_30", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.3f);
        }
    } else if (!strcmp("set_volume_40", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.4f);
        }
    } else if (!strcmp("set_volume_50", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.5f);
        }
    } else if (!strcmp("set_volume_60", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.6f);
        }
    } else if (!strcmp("set_volume_70", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.7f);
        }
    } else if (!strcmp("set_volume_80", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.8f);
        }
    } else if (!strcmp("set_volume_90", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(.9f);
        }
    } else if (!strcmp("set_volume_100", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_volume(1.0f);
        }
    } else if (!strcmp("set_default", event)) {
        if (selected_pobj != nullptr) {
            selected_pobj->set_default(selected_pobj->pa_name);
        }
    } else if (!strcmp("switch", event)) {
        if (selected_pobj != nullptr && toggle != nullptr) {
            auto current_toggle = toggle->find(selected_pobj->getRelation());
            current_toggle = std::next(current_toggle, 1);

            if (current_toggle == toggle->end()) {
                current_toggle = toggle->begin();
            }

            selected_pobj->move(current_toggle->first);
        } else if (selected_pobj != nullptr) {
            uint32_t current_attribute = selected_pobj->getRelation();

            if (current_attribute + 1 < selected_pobj->attributes.size()) {
                current_attribute++;
            } else {
                current_attribute = 0;
            }

            if (selected_pobj->attributes.size() > 0) {
                selected_pobj->set_active_attribute(
                    selected_pobj->attributes[current_attribute]->name
                );
            }
        }
    } else if (!strcmp("dropdown", event)) {
        handleDropDown(selected_pobj);
    }
}

void Tab::handleMouse(int x, int y, int button)
{
    if (object == nullptr) {
        return;
    }

    int baseY = 0;
    int current_block = 0;
    int BLOCK_SIZE = getBlockSize();

    total_blocks = (ui.height - 2) / BLOCK_SIZE;
    int blocks_drawn = 0;

    bool more_up = false;
    bool more_down = false;

    for (auto &i : *object) {
        auto item = i.second;
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

        if (has_volume) {
            if (handleMouseVolumeBar(item, x, y, ui.width, ui.height, 0, baseY + 3)) {
                return;
            }
        } else { // Configuration
            if (item->active_attribute != nullptr) {
                if (handleMouseDropDown(item, x, y, ui.width - 2, 2, 1, baseY + 2)) {
                    return;
                }
            }
        }

        unsigned int len = 0;
        unsigned int sink_pos = 0;
        PaObject* toggle_item = nullptr;

        if (toggle != nullptr) {
            auto rel = toggle->find(i.second->getRelation());

            if (rel != toggle->end()) {
                char *name = rel->second->name;

                if (name != nullptr) {
                    len = strlen(name);
                    sink_pos = ui.width - 1 - len;
                    toggle_item = rel->second;
                }
            }
        } else {
            if (i.second->active_attribute != nullptr && has_volume) {
                len = strlen(i.second->active_attribute->description);
                sink_pos = ui.width - 1 - len;
                toggle_item = i.second;
            }
        }

        if (toggle_item != nullptr) {
            auto save_selected_index = selected_index;
            auto save_selected_block = selected_block;

            selected_index = i.first;
            selected_block = current_block;

            bool done = handleMouseDropDown(toggle_item, x, y, len, 1, sink_pos, baseY + 1);

            selected_index = save_selected_index;
            selected_block = save_selected_block;

            if (done) {
                return;
            }
        }

        baseY += BLOCK_SIZE;
        current_block++;
    }

    if (more_up) {
        if (handleMouseMoreUp(x, y, 4, 1, (ui.width / 2) - 4, 0)) {
            return;
        }
    }

    if (more_down) {
        if (handleMouseMoreDown(x, y, 4, 1, (ui.width / 2) - 4, ui.height - 2)) {
            return;
        }
    }
}

void Tab::handleDropDown(PaObject* selected_pobj)
{
    uint32_t selected = 0;

    int BLOCK_SIZE = getBlockSize();

    if (selected_pobj != nullptr && toggle != nullptr) {
        selected = dropDown(
                       -1,
                       std::min(
                           selected_block * (BLOCK_SIZE),
                           (total_blocks - 1) * BLOCK_SIZE
                       ),
                       *toggle,
                       selected_pobj->getRelation()
                   );

        if (selected != static_cast<uint32_t>(-1)) {
            selected_pobj->move(selected);
        }
    } else if (selected_pobj != nullptr) {
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
                       selected_pobj->attributes,
                       selected_pobj->getRelation(),
                       w
                   );

        if (selected != static_cast<uint32_t>(-1)) {
            selected_pobj->set_active_attribute(
                selected_pobj->attributes[selected]->name
            );
        }
    }
}

bool Tab::handleMouseVolumeBar(
    PaObject* item,
    int mousex,
    int mousey,
    int w,
    int h,
    int x,
    int y
)
{
    if (mousex < x || mousex >= x + w || mousey < (y - 1) || mousey >= y + 2) {
        return false;
    }

    item->set_volume(((mousex - x + 1) / (float)(w - x)) * 1.5f);

    return true;
}

bool Tab::handleMouseDropDown(
    PaObject* item,
    int mousex,
    int mousey,
    int w,
    int h,
    int x,
    int y
)
{
    if (mousex < x || mousex >= x + w || mousey < y || mousey >= y + 3) {
        return false;
    }

    handleDropDown(item);

    return true;
}

bool Tab::handleMouseMoreUp(
    int mousex,
    int mousey,
    int w,
    int h,
    int x,
    int y
)
{
    if (mousex < x || mousex >= x + w || mousey < y || mousey > y) {
        return false;
    }

    handleEvents("move_up");

    return true;
}

bool Tab::handleMouseMoreDown(
    int mousex,
    int mousey,
    int w,
    int h,
    int x,
    int y
)
{
    if (mousex < x || mousex >= x + w || mousey < y || mousey > y) {
        return false;
    }

    handleEvents("move_down");

    return true;
}

uint32_t Tab::dropDown(
    int x,
    int y,
    std::vector<PaObjectAttribute *> attributes,
    uint32_t current,
    uint32_t width,
    uint32_t height
)
{
    if (attributes.empty()) {
        return -1;
    }

    std::map<uint32_t, std::string> tmp;

    for (uint32_t i = 0; i < attributes.size(); i++) {
        tmp[i] = attributes[i]->description;
    }

    return dropDown(x, y, tmp, current, width, height);
}

uint32_t Tab::dropDown(
    int x,
    int y,
    std::map<uint32_t, PaObject *> objects,
    uint32_t current,
    uint32_t width,
    uint32_t height
)
{
    if (objects.empty()) {
        return -1;
    }

    std::map<uint32_t, std::string> tmp;
    std::for_each(
        objects.begin(),
        objects.end(),
    [&tmp](std::pair<const uint32_t, PaObject *> const & obj) {
        tmp[obj.first] = obj.second->name;
    }
    );

    return dropDown(x, y, tmp, current, width, height);
}

uint32_t Tab::dropDown(
    int x,
    int y,
    std::map<uint32_t, std::string> objects,
    uint32_t current,
    uint32_t width,
    uint32_t height
)
{
    if (objects.empty()) {
        return -1;
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
    MENU *menu = 0;
    WINDOW *menu_win = 0;

    for (auto &i : objects) {
        ITEM *item = (ITEM*) malloc(sizeof(ITEM));
        memset(item, 0, sizeof(ITEM));

        item->opt = O_SELECTABLE;
        item->name.str = new char[i.second.length() + 1];
        memcpy(
            const_cast<char *>(item->name.str),
            i.second.c_str(),
            i.second.length()
        );
        item->name.length = i.second.length();
        item->userptr = static_cast<void *>(&i);

        items.push_back(item);

        if (i.first == current) {
            selected = items.size() - 1;
        }

        if (autowidth) {
            width = (width < strlen(i.second.c_str()) + 3) ?
                    strlen(i.second.c_str()) :
                    width;
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

        if (input == ERR) {
            continue;
        }

#ifdef KEY_MOUSE
        if (input == KEY_MOUSE) {
            MEVENT mevent;
            int ok;

            ok = getmouse(&mevent);
            if (ok != OK) {
                continue;
            }

            if (mevent.bstate & BUTTON1_PRESSED) {
                if (mevent.y < y || mevent.y > y + (int)height + 1 ||
                    mevent.x < x || mevent.x > x + (int)width + 1) {
                    clrtoeol();

                    ITEM *item = current_item(menu);
                    selected = static_cast<std::pair<uint32_t, std::string>*>(
                                   item->userptr
                               )->first;

                    selecting = false;
                    continue;
                }
            }

            if (mevent.y == y || mevent.y == y + (int)height + 1 ||
                mevent.x == x || mevent.x == x + (int)width + 1) {
                continue;
            }

            if (mevent.bstate & BUTTON1_PRESSED) {
                int top = top_row(menu);
                int idx = mevent.y - y - 1 + top;
                ITEM** its = menu_items(menu);
                int itc = item_count(menu);
                if (idx < itc) {
                    ITEM* item = its[idx];
                    set_current_item(menu, item);
                    selected = static_cast<std::pair<uint32_t, std::string>*>(
                                   item->userptr
                               )->first;
                }

                selecting = false;
                continue;
            }
#if NCURSES_MOUSE_VERSION > 1
            else if (mevent.bstate & BUTTON4_PRESSED) {
                menu_driver(menu, REQ_UP_ITEM);
            } else if (mevent.bstate & BUTTON5_PRESSED) {
                menu_driver(menu, REQ_DOWN_ITEM);
            }
#endif
        }
#endif

        if (input == KEY_RESIZE) {
            selecting = false;
            continue;
        }

        std::string key = std::to_string(input);
        
        std::string event = config.getString(("keycode." +  key).c_str(), "unbound");

        if (!strcmp("unbound", event.c_str())) {
            continue;
        }

        if (!strcmp("select", event.c_str())) {
            clrtoeol();

            ITEM *item = current_item(menu);
            selected = static_cast<std::pair<uint32_t, std::string>*>(
                           item->userptr
                       )->first;

            selecting = false;
        } else if (!strcmp("move_down", event.c_str())) {
            menu_driver(menu, REQ_DOWN_ITEM);
        } else if (!strcmp("move_up", event.c_str())) {
            menu_driver(menu, REQ_UP_ITEM);
        } else if (!strcmp("page_up", event.c_str())) {
            menu_driver(menu, REQ_SCR_UPAGE);
        } else if (!strcmp("page_down", event.c_str())) {
            menu_driver(menu, REQ_SCR_DPAGE);
        } else if (!strcmp("quit", event.c_str())) {
            selecting = false;
        }

        wrefresh(menu_win);
    }

    unpost_menu(menu);
    free_menu(menu);

    for (auto i : items) {
        if ((i != nullptr) && (i->name.str != nullptr)) {
            delete[] i->name.str;
        }
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

void Tab::selectBox(int w, int px, int py, bool selected)
{
    if (selected) {
        wattron(ui.window, COLOR_PAIR(COLOR_SELECTED));
    } else {
        wattron(ui.window, COLOR_PAIR(COLOR_DEFAULT));
    }

    mvwaddstr(ui.window, py + 1, px + 2, "Digital Stereo (HDMI) Output");

    if (selected) {
        wattroff(ui.window, COLOR_PAIR(COLOR_SELECTED));
    } else {
        wattroff(ui.window, COLOR_PAIR(COLOR_DEFAULT));
    }

    borderBox(w, 2, px, py);
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
