#ifndef S3D_VIDEO_CAPTURE_DECKLINK_H
#define S3D_VIDEO_CAPTURE_DECKLINK_H

#include <decklink_sdk/DeckLinkAPI.h>

#include <memory>

// based on:
// https://stackoverflow.com/questions/42488497/using-stl-smart-pointers-with-com-interfaces

// Utilities for smart handling of DeckLink pointers
struct DecklinkPtrDeleter {
  template <typename T>
  void operator()(T* ptr) {
    if (ptr != nullptr) {
      ptr->Release();
    }
  }
};

namespace DecklinkHelpers {
template <class T>
struct decklink_iid_t {
  using type = T;
  constexpr decklink_iid_t() = default;
};
template <class T>
constexpr decklink_iid_t<T> decklink_iid{};

template <class T>
constexpr void get_iid(decklink_iid_t<T>) = delete;  // overload this for your particular types

template <class T>
constexpr REFIID interface_iid = get_iid(decklink_iid<T> /*unused*/);
}  // namespace DecklinkHelpers

constexpr REFIID get_iid(DecklinkHelpers::decklink_iid_t<IDeckLinkInput> /*unused*/) {
  return IID_IDeckLinkInput;
}

constexpr REFIID get_iid(DecklinkHelpers::decklink_iid_t<IDeckLink> /*unused*/) {
  return IID_IDeckLink;
}

constexpr REFIID get_iid(DecklinkHelpers::decklink_iid_t<IDeckLinkConfiguration> /*unused*/) {
  return IID_IDeckLinkConfiguration;
}

constexpr REFIID get_iid(
    DecklinkHelpers::decklink_iid_t<IDeckLinkVideoFrame3DExtensions> /*unused*/) {
  return IID_IDeckLinkVideoFrame3DExtensions;
}

template <class U, class T>
std::unique_ptr<U, DecklinkPtrDeleter> make_decklink_ptr(T const& src) {
  if (!src) {
    return {};
  }
  void* r = nullptr;
  if (src->QueryInterface(DecklinkHelpers::interface_iid<U>, &r) != S_OK) {
    return {};
  }
  return {static_cast<U*>(r), {}};
}

template <class T>
using SmartDecklinkPtr = std::unique_ptr<T, DecklinkPtrDeleter>;

// use it this way
// auto decklLink = make_decklink_ptr<IDeckLinkInput>(deckLink);

#endif  // S3D_VIDEO_CAPTURE_DECKLINK_H
