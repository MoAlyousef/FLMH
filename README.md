# FLMH
FLTK Modern Helper

Just a tiny single-header library which provides a modern C++ interface for FLTK widgets where you can use lambdas for callbacks instead of function pointers, and it allows you handle custom events and perform custom drawing without needing to derive/inherit the base class.

## Examples

```c++
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <FL/names.h>

#include "flmh.hpp"

using namespace flmh;

int main() {
    auto wind = new widget<Fl_Window>(100, 100, 500, 400);
    auto but = new widget<Fl_Button>(210, 340, 80, 40, "Click");
    auto frame = new widget<Fl_Box>(0, 0, 500, 400);
    wind->show();
    but->callback([&]() { frame->label("Works!"); });
    return (Fl::run());
}
```
Note that FLTK manages the lifetimes of its widgets automatically, i.e. the Fl_Group inheriting widgets (here the Fl_Window) owns the widget and destroys them when it's destroyed.

An example for custom handling:
```c++
int main() {
    auto wind = new widget<Fl_Window>(100, 100, 500, 400);
    auto frame = new widget<Fl_Box>(0, 0, 500, 400);
    wind->show();
    wind->handle([&](int event) -> bool {
        frame->label(fl_eventnames[event]);
        return true;
    });
    return (Fl::run());
}
```
An example using Fl_Menu_Bar and messaging:
```c++
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>

#include "flmh.hpp"

using namespace flmh;

enum class Message {
    New = 0,
    Open,
    Quit,
};

auto main() -> int {
    auto wind = new widget<Fl_Window>(100, 100, 500, 400);
    auto menu = new widget<Fl_Menu_Bar>(0, 0, 500, 40);
    auto frame = new widget<Fl_Box>(0, 0, 500, 400);
    wind->end();
    wind->show();

    auto [s, r] = channel<Message>();
    menu->add("File/New", 0, [&]() { s.emit(Message::New); }, 0);
    menu->add("File/Open", 0, [&]() { s.emit(Message::Open); }, 0);
    menu->add("File/Quit", 0, [&]() { s.emit(Message::Quit); }, 0);

    Fl::lock();
    while (Fl::wait()) {
        auto msg = r.recv(); // returns a std::optional<T>
        if (msg) {
            switch (static_cast<int>(msg.value())) {
            case 0:
                frame->label("New");
                break;
            case 1:
                frame->label("Open");
                break;
            case 2:
                wind->hide();
                break;
            default:
                break;
            }
        }
    }
}
```
    
## Usage
You can just include the header file directly to your project. There's also a CMakeLists.txt file for CMake projects. An example CMakeLists.txt file for adding flmh:
```cmake
cmake_minimum_required(VERSION 3.14)
project(app)

find_package(FLTK CONFIG REQUIRED)

include(FetchContent)

FetchContent_Declare(
  flmh
  GIT_REPOSITORY https://github.com/MoAlyousef/flmh.git
)

FetchContent_MakeAvailable(flmh)

add_executable(main main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE flmh::flmh fltk fltk_gl fltk_forms fltk_images)
```
