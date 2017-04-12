//
// Created by jon on 19/03/17.
//

#ifndef S3D_UTILITIES_FILE_IO_H
#define S3D_UTILITIES_FILE_IO_H

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <vector>

namespace s3d {
namespace file_io {

template <class Size_t>
void push_back_n_bytes(std::istream& stream,
                       Size_t n,
                       std::vector<uint8_t>& outContainer) {
  std::copy_n(std::istreambuf_iterator<char>{stream}, n,
              back_inserter(outContainer));
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

template <class Container>
bool write_bytes(const std::string& filename, Container c) {
  std::ofstream out{filename, std::ios::binary};
  if (out.is_open()) {
    std::copy(std::begin(c), std::end(c), std::ostreambuf_iterator<char>{out});
    return true;
  }
  return false;
}

template <class Size_t>
std::vector<uint8_t> load_n_bytes(const std::string& filename, Size_t n) {
  auto bytes = std::vector<uint8_t>{};
  std::ifstream in{filename, std::ios::binary};
  std::copy_n(std::istreambuf_iterator<char>{in}, n, back_inserter(bytes));
  return bytes;
}

class ifchunkstream {
 public:
  ifchunkstream(const std::string& filename, size_t chunkSize)
      : stream_{filename, std::ios::binary}, chunkSize_{chunkSize} {}

  friend ifchunkstream& operator>>(ifchunkstream& input,
                                   std::vector<uint8_t>& v) {
    input.push_back_chunk(v);
    return input;
  }

  explicit operator bool() const { return stream_.eof(); }

  // private:
  void push_back_chunk(std::vector<uint8_t>& v) {
    push_back_n_bytes(stream_, chunkSize_, v);
  }

  std::ifstream stream_;
  size_t chunkSize_;
};
}  // namespace file_io
}  // namespace s3d

#endif  // S3D_UTILITIES_FILE_IO_H
