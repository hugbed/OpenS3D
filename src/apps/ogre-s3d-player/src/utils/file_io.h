//
// Created by jon on 19/03/17.
//

#ifndef OGRE_SAMPLE_FILE_IO_H
#define OGRE_SAMPLE_FILE_IO_H

#include <stddef.h>
#include <fstream>
#include <vector>
#include <algorithm>

template<class Size_t>
static void push_back_n_bytes(std::istream& stream, Size_t n, std::vector<uint8_t>& v)
{
    std::copy_n(std::istreambuf_iterator<char>{stream},
                n,
                back_inserter(v));
    stream.get(); // to prevent reading twice the last character
}

template <class OIt, typename Size_t>
static bool read_n_bytes(std::istream& stream, Size_t n, OIt it)
{
    if (stream.peek() == EOF) return false;
    std::copy_n(std::istreambuf_iterator<char>{stream},
                n,
                it);
    stream.get();
    return !stream.eof();
}

std::vector<uint8_t> load_bytes(const std::string& filename)
{
    auto bytes = std::vector<uint8_t>{};
    std::ifstream in{filename, std::ios::binary};
    std::copy(std::istreambuf_iterator<char>{in},
              std::istreambuf_iterator<char>{},
              back_inserter(bytes));
    return bytes;
}

template<class Container>
bool write_bytes(const std::string& filename, Container c)
{
    std::ofstream out{filename, std::ios::binary};
    if (out.is_open()) {
        std::copy(std::begin(c), std::end(c), std::ostreambuf_iterator<char>{out});
        return true;
    }
    return false;
}

template<class Size_t>
std::vector<uint8_t> load_n_bytes(const std::string& filename, Size_t n)
{
    auto bytes = std::vector<uint8_t>{};
    std::ifstream in{filename, std::ios::binary};
    std::copy_n(std::istreambuf_iterator<char>{in},
                n,
                back_inserter(bytes));
    return bytes;
}

class ifchunkstream {
public:
    ifchunkstream(std::string filename, size_t chunkSize)
            :stream_{filename, std::ios::binary}, chunkSize_{chunkSize}
    {
    }

    friend ifchunkstream& operator>>(ifchunkstream& input, std::vector<uint8_t>& v)
    {
        input.push_back_chunk(v);
        return input;
    }

    operator bool() const
    {
        return stream_.eof();
    }

//private:
    void push_back_chunk(std::vector<uint8_t>& v)
    {
        push_back_n_bytes(stream_, chunkSize_, v);
    }

    size_t chunkSize_;
    std::ifstream stream_;
};

#endif //OGRE_SAMPLE_FILE_IO_H
