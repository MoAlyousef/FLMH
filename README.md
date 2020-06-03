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
    auto wind = make_widget<Fl_Window>(100, 100, 500, 400, "");
    auto but = make_widget<Fl_Button>(210, 340, 80, 40, "Click");
    auto frame = make_widget<Fl_Box>(0, 0, 500, 400, "");
    wind->show();
    but->callback([&]() { frame->label("Works!"); });
    return(Fl::run());
}
```
An example for custom handling:
```c++
int main() {
    auto wind = make_widget<Fl_Window>(100, 100, 500, 400, "");
    auto frame = make_widget<Fl_Box>(0, 0, 500, 400, "");
    wind->show();
    wind->handle([&](int event) -> bool {
        frame->label(fl_eventnames[event]);
        return true;
    });
    return(Fl::run());
}
```
An example using Fl_Menu_Bar:
```c++
#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Window.H>

#include "flmh.hpp"

using namespace flmh;

int main() {
  auto wind = make_widget<Fl_Window>(100, 100, 500, 400);
  auto menu = make_widget<Fl_Menu_Bar>(0, 0, 500, 40);
  wind->show();
  menu->add("File/new", 0, [&]() { wind->label("Hello"); }, 0);
  return (Fl::run());
}
```

```c++
auto main() -> int {
  auto wind = make_widget<Fl_Window>(100, 100, 500, 400, "");
  auto but = make_widget<Fl_Button>(210, 340, 80, 40, "Click");
  auto frame = make_widget<Fl_Box>(0, 0, 500, 400, "");
  wind->show();
  Fl::lock();
  int i = 1;
  but->callback([&]() { Fl::awake(&i); });
  while (Fl::wait()) {
    auto msg = Fl::thread_message();
    if (msg) {
      frame->label(std::to_string(*static_cast<int *>(msg)).c_str());
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
