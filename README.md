# FLMH
FLTK Modern Helper

Just a tiny single-header library which provides a modern C++ interface for FLTK widgets where you can use lambdas for callbacks instead of function pointers. 

```c++
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <FL/names.h>

#include "flmh.hpp"

using namespace flmh;

int main() {
    auto wind = new widget<Fl_Window>(100, 100, 500, 400, "");
    auto but = new widget<Fl_Button>(210, 340, 80, 40, "Click");
    auto frame = new widget<Fl_Box>(0, 0, 500, 400, "");
    wind->show();
    but->callback([&]() { frame->label("Works!"); });
    return(Fl::run());
}
```

It also allows you to handle custom events and perform custom drawing without needing to derive/inherit the base class:

```c++
int main() {
    auto wind = new widget<Fl_Window>(100, 100, 500, 400, "");
    auto frame = new widget<Fl_Box>(0, 0, 500, 400, "");
    wind->show();
    wind->handle([&](int event) -> bool {
        frame->label(fl_eventnames[event]);
        return true;
    });
    return(Fl::run());
}
```

## Usage
You can just include the header file directly to your project.
