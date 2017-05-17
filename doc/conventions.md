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
    │   │   ├── tests_some_helper_stuff.cpp
    │   │   └── tests_great_class_here.cpp
    │   └── CMakeLists.txt
    ├── AppDemo
    │   ├── CMakeLists.txt
    │   ├── resource
    │   │   └── icons 
    │   ├── src
    │   │   ├── app.cpp
    │   │   ├── some_widget.h
    │   │   └── some_widget.cpp
    │   └── CMakeLists.txt    
    └── CMakeLists.txt
 ```

## Code examples

Class definition.

In .h:

```cpp
#include "Eigen/Dense"

#include <vector>
#include <cstdint>

namespace s3d { // lower case with underscores
namespace module_name { // lower case with underscores
class BlobDetectorNaive : public BlobDetector { // camel case with first upper case
 public:
  // camel case with first lower case
  Eigen::Vector2d detectBlob(const Image<uint8_t>& imageData, double threshold) override;

 private:
  double internalVariable_; // camel case with first lower case and underscore at the end
};
}  // namespace module_name // namespace comment
}  // namespace OpenS3D
```

In .cpp:

```cpp
namespace OpenS3D {
namespace module_name {
Eigen::Vector2d BlobDetector::detectBlob()(const Image<uint8_t>& imageData, double threshold) {
  Eigen::Matrix3d m;
  m << 1.0, 2.0, 3.0,  //
      4.0, 5.0, 6.0,   //
      7.0, 8.0, 9.0;

  Vector3d res = m * Vector3d(0.5, 0.5, 1.0);

  return Vector2d(res.x(), res.y());
}
}  // namespace module_name
}  // namespace OpenS3D
```

Another example with mode code:

```cpp
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <cstdlib>
#include <ctime>

// template_utility_function
template <typename T, int size>
bool is_sorted(T (&array)[size]) {
  return std::adjacent_find(array, array + size, std::greater<T>()) == array + size;
}

int main() {
  std::srand(std::time(0));

  int list[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  do {
    std::random_shuffle(list, list + 9);
  } while (is_sorted(list));

  int score = 0;

  do {
    std::cout << "Current list: ";
    std::copy(list, list + 9, std::ostream_iterator<int>(std::cout, " "));

    int rev;
    while (true) {
      std::cout << "\nDigits to reverse? ";
      std::cin >> rev;
      if (rev > 1 && rev < 10)
        break;
      std::cout << "Please enter a value between 2 and 9.";
    }

    ++score;
    std::reverse(list, list + rev);
  } while (!is_sorted(list));

  std::cout << "Congratulations, you sorted the list.\n"
            << "You needed " << score << " reversals." << std::endl;
  return 0;
}
```

