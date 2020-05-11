/**
 * MIT License
 *
 * Copyright (c) 2020 Mohammed Alyousef
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __FLTK_MODERN_HELPER__
#define __FLTK_MODERN_HELPER__

#include <FL/Fl_Menu.H>
#include <FL/Fl_Widget.H>
#include <functional>
#include <type_traits>

namespace flmh {
template <typename Widget, typename = typename std::enable_if<
                               std::is_base_of<Fl_Widget, Widget>::value>::type>
class widget : public Widget {
    void *ev_data_ = NULL;
    void *draw_data_ = NULL;
    typedef int (*handler)(int, void *data);
    handler inner_handler = NULL;
    typedef void (*drawer)(void *data);
    drawer inner_drawer = NULL;
    void set_handler(handler h) { inner_handler = h; }
    void set_handler_data(void *data) { ev_data_ = data; }
    void set_drawer(drawer h) { inner_drawer = h; }
    void set_drawer_data(void *data) { draw_data_ = data; }
    int handle(int event) override {
        int ret = Widget::handle(event);
        int local = 0;
        if (ev_data_ && inner_handler) {
            local = inner_handler(event, ev_data_);
            if (local == 0)
                return ret;
            else
                return local;
        } else {
            return ret;
        }
    }
    void draw() override {
        Widget::draw();

        if (draw_data_ && inner_drawer)
            inner_drawer(draw_data_);
    }

  public:
    widget(int x, int y, int w, int h, const char *title = 0)
        : Widget(x, y, w, h, title) {}
    operator widget *() { return (widget *)*this; }
    void callback(std::function<void()> cb) {
        if (!cb)
            return;
        auto shim = [](Fl_Widget *, void *data) {
            if (!data)
                return;
            auto d = (std::function<void()> *)data;
            (*d)();
        };
        Widget::callback(shim, (void *)&cb);
    }
    void handle(std::function<bool(int)> cb) {
        if (!cb)
            return;
        auto shim = [](int _ev, void *data) -> int {
            if (!data)
                return 0;
            auto d = (std::function<bool(int)> *)data;
            return (*d)(_ev);
        };
        set_handler(shim);
        set_handler_data((void *)&cb);
    }
    void draw(std::function<void()> cb) {
        if (!cb)
            return;
        auto shim = [](void *data) {
            if (!data)
                return 0;
            auto d = (std::function<void()> *)data;
            (*d)();
        };
        set_drawer(shim);
        set_drawer_data((void *)&cb);
    }
    void add(const char *name, int shortcut, std::function<void()> cb,
             int flag) {
        if constexpr (std::is_base_of_v<Fl_Menu_, Widget>) {
            if (!cb)
                return;
            auto shim = [](Fl_Widget *, void *data) {
                if (!data)
                    return;
                auto d = (std::function<void()> *)data;
                (*d)();
            };
            Widget::add(name, shortcut, shim, (void *)&cb, flag);
        }
    }
    void insert(int index, const char *name, int shortcut,
                std::function<void()> cb, int flag) {
        if constexpr (std::is_base_of_v<Fl_Menu_, Widget>) {
            if (!cb)
                return;
            auto shim = [](Fl_Widget *, void *data) {
                if (!data)
                    return;
                auto d = (std::function<void()> *)data;
                (*d)();
            };
            Widget::insert(index, name, shortcut, shim, (void *)&cb, flag);
        }
    }
};
} // namespace flmh

#endif
