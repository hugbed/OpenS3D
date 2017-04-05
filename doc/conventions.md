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
    │   │       └── GreatClassHere.h
    │   ├── src
    │   │   └── GreatClassHere.cpp
    │   ├── test
    │   │   ├── test_some_helper_stuff.cpp
    │   │   └── test_GreatClassHere.cpp
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
