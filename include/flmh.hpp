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

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <cassert>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace flmh {

template <typename T>
struct Sender {
    static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>);
    void emit(const T &t) const {
        auto *temp = new T(t);
        Fl::awake(temp);
    }
};

template <typename T>
struct Receiver {
    static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>);
    std::optional<T> recv() const {
        auto msg = Fl::thread_message();
        if (!msg)
            return std::nullopt;
        auto val = *static_cast<T *>(msg);
        delete static_cast<T *>(msg);
        return val;
    }
};

template <typename T>
auto channel() -> std::pair<Sender<T>, Receiver<T>> {
    static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>);
    Sender<T> s;
    Receiver<T> r;
    return std::make_pair(s, r);
}

template <typename W>
class Widget final : public W {
    static_assert(std::is_same_v<W, Fl_Widget> || std::is_base_of_v<Fl_Widget, W>);

    std::function<void(Widget *)> callback_fn_ = nullptr;

    std::function<bool(Widget *, int)> handler_fn_ = nullptr;

    std::function<void(Widget *)> drawer_fn_ = nullptr;

    int handle(int event) override {
        int local = 0;
        if (handler_fn_) {
            local = handler_fn_(this, event);
            if (local == 0)
                return W::handle(event);
            else
                return W::handle(event) | local;
        } else {
            return W::handle(event);
        }
    }

    void draw() override {
        W::draw();
        if (drawer_fn_)
            drawer_fn_(this);
    }

  public:
    Widget(int x, int y, int w, int h, const char *title = 0) : W(x, y, w, h, title) {}
    Widget(int w, int h, const char *title = 0) : W(0, 0, w, h, title) {
        if constexpr (std::is_same_v<W, Fl_Window> || std::is_base_of_v<Fl_Window, W>) {
            this->free_position();
        }
    }
    Widget(const char *title = 0) : W(0, 0, 0, 0, title) {
        if constexpr (std::is_same_v<W, Fl_Window> || std::is_base_of_v<Fl_Window, W>) {
            this->free_position();
        }
    }

    Widget(const Widget &) = delete;

    Widget(Widget &&) = delete;

    operator W *() { return (W *)this; }

    void callback(std::function<void(Widget *)> &&cb) {
        if (!cb)
            return;
        auto shim = [](Fl_Widget *w, void *data) {
            if (!data)
                return;
            auto d = static_cast<std::function<void(Widget *)> *>(data);
            (*d)((Widget *)w);
        };
        callback_fn_ = cb;
        W::callback(shim, (void *)&callback_fn_);
    }

    template <typename Message, typename = std::enable_if_t<std::is_pod_v<Message>>>
    void emit(const Sender<Message> &s, const Message &m) {
        callback([&] { s.emit(m); });
    }

    void handle(std::function<bool(Widget *, int)> &&cb) { handler_fn_ = cb; }

    void draw(std::function<void(Widget *)> &&cb) { drawer_fn_ = cb; }

    void center_of(Fl_Widget *w) {
        assert(w->w() != 0 && w->h() != 0);
        double sw = this->w();
        double sh = this->h();
        double ww = w->w();
        double wh = w->h();
        auto sx = (ww - sw) / 2.0;
        auto sy = (wh - sh) / 2.0;
        auto wx = [=] {
            if (w->as_window()) {
                return 0;
            } else {
                return w->x();
            }
        }();
        auto wy = [=] {
            if (w->as_window()) {
                return 0;
            } else {
                return w->y();
            }
        }();
        this->resize(sx + wx, sy + wy, sw, sh);
        this->redraw();
    }

    void center_of_parent() {
        auto p = this->parent();
        if (p) {
            center_of(p);
        }
    }

    void center_x(Fl_Widget *w) {
        assert(w->w() != 0 && w->h() != 0);
        double sw = this->w();
        double sh = this->h();
        double ww = w->w();
        double wh = w->h();
        auto sx = (ww - sw) / 2.0;
        auto sy = this->y();
        auto wx = [=] {
            if (w->as_window()) {
                return 0;
            } else {
                return w->x();
            }
        }();
        this->resize(sx + wx, sy, sw, sh);
        this->redraw();
    }

    void center_y(Fl_Widget *w) {
        assert(w->w() != 0 && w->h() != 0);
        double sw = this->w();
        double sh = this->h();
        double ww = w->w();
        double wh = w->h();
        auto sx = this->x();
        auto sy = (wh - sh) / 2.0;
        auto wy = [=] {
            if (w->as_window()) {
                return 0;
            } else {
                return w->y();
            }
        }();
        this->resize(sx, sy + wy, sw, sh);
        this->redraw();
    }

    void below_of(const Fl_Widget *wid, int padding) {
        auto w = this->w();
        auto h = this->h();
        assert(w != 0 && h != 0);
        this->resize(wid->x(), wid->y() + wid->h() + padding, w, h);
    }

    void above_of(const Fl_Widget *wid, int padding) {
        auto w = this->w();
        auto h = this->h();
        assert(w != 0 && h != 0);
        this->resize(wid->x(), wid->y() - padding - h, w, h);
    }

    void right_of(const Fl_Widget *wid, int padding) {
        auto w = this->w();
        auto h = this->h();
        assert(w != 0 && h != 0);
        this->resize(wid->x() + wid->w() + padding, wid->y(), w, h);
    }

    void left_of(const Fl_Widget *wid, int padding) {
        auto w = this->w();
        auto h = this->h();
        assert(w != 0 && h != 0);
        this->resize(wid->x() - w - padding, wid->y(), w, h);
    }

    void size_of(const Fl_Widget *wid) { this->resize(this->x(), this->y(), wid->w(), wid->h()); }

    void size_of_parent() {
        auto p = this->parent();
        if (p) {
            size_of(p);
        }
    }
};

} // namespace flmh

#endif
