#include "s3d/utilities/file_io.h"
#include "s3d/video/compression/yuv.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "gsl/gsl"

int main(int argc, char* argv[]) {
  auto input_args = gsl::span<char*>(argv, argc);

  if (input_args.size() != 6) {
    std::cerr << "Correct usage is: " << input_args[0]
              << " file_in.uyvy file_out.pbm width height max_color_value" << std::endl;
    return -1;
  }

  std::string uyvyFilename{input_args[1]};
  std::string rgbFilename{input_args[2]};

  std::vector<uint8_t> uyvyBytes, rgbBytes;
  {
    std::ifstream in_fs{uyvyFilename, std::ios::binary};
    s3d::file_io::read_bytes(in_fs, back_inserter(uyvyBytes));
  }

  s3d::compression::color_conversion<s3d::compression::UYVY, s3d::compression::RGB> cvt;
  cvt(std::begin(uyvyBytes), std::end(uyvyBytes), back_inserter(rgbBytes));
  {
    std::ofstream out_fs{rgbFilename, std::ios::binary};
    s3d::file_io::write_bytes(out_fs, std::string("P6 "));
    s3d::file_io::write_bytes(out_fs, std::string(input_args[3]).append(" "));  // width (i.e, 1920)
    s3d::file_io::write_bytes(out_fs,
                              std::string(input_args[4]).append(" "));  // height (i.e, 1080)
    s3d::file_io::write_bytes(out_fs,
                              std::string(input_args[5]).append("\n"));  // max value (i.e, 255)
    s3d::file_io::write_bytes(out_fs, std::begin(rgbBytes), std::end(rgbBytes));
  }

  std::cout << "yeah" << std::endl;

  return 0;
}