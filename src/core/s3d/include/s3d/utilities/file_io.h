#ifndef S3D_UTILITIES_FILE_IO_H
#define S3D_UTILITIES_FILE_IO_H

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <vector>

namespace s3d {
namespace file_io {

template <class Size_t>
void push_back_n_bytes(std::istream& stream, Size_t n, std::vector<uint8_t>& outContainer) {
  std::copy_n(std::istreambuf_iterator<char>{stream}, n, back_inserter(outContainer));
  stream.get();  // to prevent reading twice the last character
}

template <class OIt, typename Size_t>
bool read_n_bytes(std::istream& stream, Size_t n, OIt it) {
  if (stream.peek() == EOF || n == 0) {
    return false;
  }
  std::copy_n(std::istreambuf_iterator<char>{stream}, n, it);
  stream.get();
  return !stream.eof();
}

// to write to binary file
// std::ofstream outputStream{filename, std::ios::binary};
template <class Container>
void write_bytes(std::ostream& outputStream, Container c) {
  std::copy(std::begin(c), std::end(c), std::ostreambuf_iterator<char>{outputStream});
}

template <class Size_t>
std::vector<uint8_t> load_n_bytes(const std::string& filename, Size_t n) {
  auto bytes = std::vector<uint8_t>{};
  std::ifstream in{filename, std::ios::binary};
  std::copy_n(std::istreambuf_iterator<char>{in}, n, back_inserter(bytes));
  return bytes;
}

}  // namespace file_io
}  // namespace s3d

#endif  // S3D_UTILITIES_FILE_IO_H
