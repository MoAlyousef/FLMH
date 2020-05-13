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

template <typename Widget,
          typename = std::enable_if_t<std::is_base_of_v<Fl_Widget, Widget>>>
class widget final : public Widget {

  using handler = int (*)(int, void *);

  using drawer = void (*)(void *);

  handler inner_handler_ = nullptr;

  drawer inner_drawer_ = nullptr;

  void *ev_data_ = nullptr;

  void *draw_data_ = nullptr;

  std::function<void()> *callback_handle_ = nullptr;

  std::function<bool(int)> *handler_handle_ = nullptr;

  std::function<void()> *drawer_handle_ = nullptr;

  void set_handler_(handler h) { inner_handler_ = h; }

  void set_handler_data_(void *data) { ev_data_ = data; }

  void set_drawer_(drawer h) { inner_drawer_ = h; }

  void set_drawer_data_(void *data) { draw_data_ = data; }

  int handle(int event) override {
    int ret = Widget::handle(event);
    int local = 0;
    if (ev_data_ && inner_handler_) {
      local = inner_handler_(event, ev_data_);
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
    if (draw_data_ && inner_drawer_)
      inner_drawer_(draw_data_);
  }

public:
  widget(int x, int y, int w, int h, const char *title = 0)
      : Widget(x, y, w, h, title) {}

  operator Widget *() { return (Widget *)this; }

  widget(const widget &) = delete;

  widget(widget &&) = delete;

  ~widget() {
    delete callback_handle_;
    delete handler_handle_;
    delete drawer_handle_;
  }

  void callback(std::function<void()> &&cb) {
    if (!cb)
      return;
    auto shim = [](Fl_Widget *, void *data) {
      if (!data)
        return;
      auto d = (std::function<void()> *)data;
      (*d)();
    };
    callback_handle_ = new std::function<void()>(cb);
    Widget::callback(shim, (void *)callback_handle_);
  }

  void handle(std::function<bool(int)> &&cb) {
    if (!cb)
      return;
    auto shim = [](int _ev, void *data) -> int {
      if (!data)
        return 0;
      auto d = (std::function<bool(int)> *)data;
      return (*d)(_ev);
    };
    set_handler_(shim);
    handler_handle_ = new std::function<bool(int)>(cb);
    set_handler_data_((void *)handler_handle_);
  }

  void draw(std::function<void()> &&cb) {
    if (!cb)
      return;
    auto shim = [](void *data) {
      if (!data)
        return 0;
      auto d = (std::function<void()> *)data;
      (*d)();
    };
    set_drawer_(shim);
    drawer_handle_ = new std::function<void()>(cb);
    set_drawer_data_((void *)drawer_handle_);
  }

  void add(const char *name, int shortcut, std::function<void()> &&cb,
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
      auto temp = new std::function<void()>(cb); // currently leaks
      Widget::add(name, shortcut, shim, (void *)temp, flag);
    } else {
      return;
    }
  }

  void insert(int index, const char *name, int shortcut,
              std::function<void()> &&cb, int flag) {
    if constexpr (std::is_base_of_v<Fl_Menu_, Widget>) {
      if (!cb)
        return;
      auto shim = [](Fl_Widget *, void *data) {
        if (!data)
          return;
        auto d = (std::function<void()> *)data;
        (*d)();
      };
      auto temp = new std::function<void()>(cb); // currently leaks
      Widget::insert(index, name, shortcut, shim, (void *)temp, flag);
    } else {
      return;
    }
  }
};

} // namespace flmh

#endif
