//
// Created by jon on 11/04/17.
//

#ifndef S3D_UTILITIES_STRINGS_H
#define S3D_UTILITIES_STRINGS_H

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

namespace s3d {

template <typename Out>
void split(const std::string& s, char delimiter, Out result) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delimiter)) {
    *(result++) = item;
  }
}

}  // namespace s3d

#endif  // S3D_UTILITIES_STRINGS_H
