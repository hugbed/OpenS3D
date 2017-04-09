## Project Naming Convention

```
├── build-release
├── build-relwithdebinfo
├── build-debug
└── src
    ├── libsomething
    │   ├── include
    │   │   └── libsomething
    │   │       ├── some_helper_stuff.h
    │   │       └── great_class_here.h
    │   ├── src
    │   │   └── great_class_here.cpp
    │   ├── test
    │   │   ├── test_some_helper_stuff.cpp
    │   │   └── test_great_class_here.cpp
    │   └── CMakeLists.txt
    ├── AppDemo
    │   ├── CMakeLists.txt
    │   ├── resource
    │   │   └── icons 
    │   ├── src
    │   │   ├── app.cpp
    │   │   ├── SomeWidget.h
    │   │   └── SomeWidget.cpp
    │   └── CMakeLists.txt    
    └── CMakeLists.txt
 ```
