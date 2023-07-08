# FLMH
FLTK Modern Helper

Just a tiny single-header library which provides a modern C++ interface for FLTK widgets where you can use lambdas for callbacks instead of function pointers, and it allows you to handle custom events and perform custom drawing without needing to derive/inherit the base class.
It also provides:
- Several anchoring/positioning methods such as center_of, center_of_parent, size_of, right_of...etc.
- Channels, which return a sender and a receiver.

## Examples

```c++
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Window.H>

#include "flmh.hpp"

using flmh::make_widget;

int main() {
    auto *wind = make_widget<Fl_Window>(300, 200);
    auto *col = make_widget<Fl_Flex>(100, 100);
    col->type(Fl_Flex::COLUMN);
    col->center_of_parent();
    auto *box = make_widget<Fl_Box>();
    box->box(FL_DOWN_BOX);
    auto *but = make_widget<Fl_Button>("Click");
    col->fixed(but, 30);
    col->end();
    wind->end();
    wind->show();
    but->callback([=](auto) { box->label("Works!"); });
    return Fl::run();
}
```
Note that FLTK manages the lifetimes of its widgets automatically, i.e. the Fl_Group inheriting widgets (here the Fl_Window) owns the widget and destroys them when it's destroyed.

An example for custom handling:
```c++
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/names.h>

#include "flmh.hpp"

using flmh::make_widget;

int main() {
    auto *wind = make_widget<Fl_Window>(400, 300, "Event names");
    auto *box = make_widget<Fl_Box>(200, 100);
    box->center_of(wind);
    box->box(FL_DOWN_BOX);
    wind->end();
    wind->show();
    box->handle([](auto *b, int event) -> bool {
        b->label(fl_eventnames[event]);
        return true;
    });
    return Fl::run();
}
```
An example using Fl_Menu_Bar and messaging:
```c++
#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>
#include <string>
#include <string_view>

#include "flmh.hpp"

using flmh::channel;
using flmh::make_widget;

enum class Message {
    New,
    Open,
    Quit,
};

auto main() -> int {
    Fl::scheme("gtk+");
    auto *wind = make_widget<Fl_Window>(500, 400);
    auto *menu = make_widget<Fl_Menu_Bar>(500, 30);
    auto *box = make_widget<Fl_Box>(200, 100);
    box->center_of_parent();
    box->box(FL_DOWN_BOX);
    wind->end();
    wind->show();

    auto [s, r] = channel<Message>();

    menu->add("File/New");
    menu->add("File/Open");
    menu->add("File/Quit");

    // [s = s] to avoid clang's: captured structured bindings are a C++20 extension
    menu->callback([s = s](auto *m) {
        std::string name(250, '\0');
        if (m->item_pathname(name.data(), name.length()) == 0) {
            auto n = std::string_view(&name[0], name.find('\0'));
            if (n == "File/New") s.emit(Message::New);
            if (n == "File/Open") s.emit(Message::Open);
            if (n == "File/Quit") s.emit(Message::Quit);
        }
    });

    Fl::lock();
    while (Fl::wait()) {
        auto msg = r.recv(); // returns a std::optional<T>
        if (msg) {
            switch (static_cast<Message>(msg.value())) {
            break; case Message::New:
                box->label("New");
            break; case Message::Open:
                box->label("Open");
            break; case Message::Quit:
                wind->hide();
            }
        }
    }
}
```
    
## Usage
You can just include the header file directly to your project. 

## Requirements
C++17 or above.