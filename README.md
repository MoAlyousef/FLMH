# FLMH
FLTK Modern Helper

Just a tiny single-header library which provides a modern C++ interface for FLTK widgets where you can use lambdas for callbacks instead of function pointers, and it allows you to handle custom events and perform custom drawing without needing to derive/inherit the base class.
It also provides:
- Overloading of label and widget constructors to accept std::string.
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

#include <memory>

#include "flmh.hpp"

using flmh::make_widget;

int main() {
    auto count = std::make_shared<int>(0);

    auto *wind = make_widget<Fl_Window>(300, 200);
    auto *col = make_widget<Fl_Flex>(100, 100);
    col->type(Fl_Flex::COLUMN);
    col->center_of_parent();
    auto *box = make_widget<Fl_Box>(std::to_string(*count));
    box->box(FL_DOWN_BOX);
    auto *but = make_widget<Fl_Button>("Click");
    col->fixed(but, 30);
    col->end();
    wind->end();
    wind->show();
    
    but->callback([=](auto) { 
        *count += 1;
        box->label(std::to_string(*count)); 
    });
    
    return Fl::run();
}
```

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
#include <FL/Fl_Flex.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Window.H>

#include <memory>
#include <string>
#include <string_view>

#include "flmh.hpp"

using flmh::channel;
using flmh::make_widget;

enum class Message {
    New,
    Open,
    Quit,
    Undo,
};

auto main() -> int {
    Fl::scheme("gtk+");
    auto *wind = make_widget<Fl_Window>(500, 400);
    auto *col = make_widget<Fl_Flex>(500, 400);
    auto *menu = make_widget<Fl_Menu_Bar>();
    col->fixed(menu, 30);
    auto buf = std::make_shared<Fl_Text_Buffer>();
    auto *ed = make_widget<Fl_Text_Editor>();
    ed->buffer(buf.get());
    col->end();
    wind->end();
    wind->show();
    wind->label(std::string("Hello World"));

    auto [s, r] = channel<Message>();

    auto idx = 0;
    idx = menu->add("File/New");
    menu->shortcut(idx, FL_CTRL | 'n');
    idx = menu->add("File/Open");
    menu->shortcut(idx, FL_CTRL | 'o');
    menu->mode(idx, FL_MENU_DIVIDER);
    idx = menu->add("File/Quit");
    menu->shortcut(idx, FL_CTRL | 'q');
    idx = menu->add("Edit/Undo");
    menu->shortcut(idx, FL_CTRL | 'z');

    // [s = s] to avoid clang's: captured structured bindings are a C++20 extension
    menu->callback([s = s](auto *m) {
        std::string name(250, '\0');
        if (m->item_pathname(name.data(), name.length()) == 0) {
            auto n = std::string_view(&name[0], name.find('\0'));
            if (n == "File/New") s.emit(Message::New);
            if (n == "File/Open") s.emit(Message::Open);
            if (n == "File/Quit") s.emit(Message::Quit);
            if (n == "Edit/Undo") s.emit(Message::Undo);
        }
    });

    Fl::lock();
    while (Fl::wait()) {
        if (auto msg = r.recv()) { // r.recv() returns a std::optional<T>
            switch (static_cast<Message>(msg.value())) {
                break; case Message::New: buf->text("");
                break; case Message::Open: {
                    Fl_Native_File_Chooser dlg;
                    dlg.title("Choose a file to open!");
                    dlg.type(Fl_Native_File_Chooser::BROWSE_FILE);
                    dlg.filter("*.txt\n");
                    dlg.show();
                    auto fname = dlg.filename();
                    buf->loadfile(fname);
                }
                break; case Message::Quit: wind->hide();
                break; case Message::Undo: Fl_Text_Editor::kf_undo(0, ed);
            }
        }
    }
}
```
    
## Usage
You can just include the header file directly to your project. 

## Requirements
C++17 or above.
