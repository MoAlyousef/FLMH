#ifndef __FLTK_MODERN_HELPER__
#define __FLTK_MODERN_HELPER__

#include <FL/Fl_Menu.H>
#include <FL/Fl_Widget.H>
#include <type_traits>

namespace flmh {
template <typename Widget,
          typename = typename std::enable_if<std::is_base_of<Fl_Widget, Widget>::value>::type>
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
    template <typename Menu,
              typename = typename std::enable_if<std::is_base_of<Fl_Menu, Menu>::value>::type>
    void add(const char *name, int shortcut, std::function<void()> cb,
             int flag) {
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
    template <typename Menu,
              typename = typename std::enable_if<std::is_base_of<Fl_Menu, Menu>::value>::type>
    void insert(int index, const char *name, int shortcut,
                std::function<void()> cb, int flag) {
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
};
} // namespace flmh

#endif
