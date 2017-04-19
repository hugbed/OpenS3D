//
// Created by jon on 18/04/17.
//

#ifndef PROJECT_DECKLINK_H
#define PROJECT_DECKLINK_H

#include <decklink_sdk/DeckLinkAPI.h>

// based on: http://en.cppreference.com/w/cpp/language/constexpr

// Utilities for smart handling of DeckLink pointers
struct DecklinkPtrDeleter {
  template <typename T>
  void operator()(T* ptr) {
    if (ptr)
      ptr->Release();
  }
};

namespace DecklinkHelpers {
template <class T>
struct decklink_iid_t {
  using type = T;
  constexpr decklink_iid_t(){};
};
template <class T>
constexpr decklink_iid_t<T> decklink_iid{};

template <class T>
constexpr void get_iid(decklink_iid_t<T>) = delete;  // overload this for your particular types

template <class T>
constexpr REFIID interface_iid = get_iid(decklink_iid<T>);
}

constexpr REFIID get_iid(DecklinkHelpers::decklink_iid_t<IDeckLinkInput>) {
  return IID_IDeckLinkInput;
}

constexpr REFIID get_iid(DecklinkHelpers::decklink_iid_t<IDeckLink>) {
  return IID_IDeckLink;
}

constexpr REFIID get_iid(DecklinkHelpers::decklink_iid_t<IDeckLinkConfiguration>) {
  return IID_IDeckLinkConfiguration;
}

template <class U, class T>
std::unique_ptr<U, DecklinkPtrDeleter> make_decklink_ptr(T const& src) {
  if (!src)
    return {};
  U* r = nullptr;
  if (src->QueryInterface(DecklinkHelpers::interface_iid<U>, (void**)&r) != S_OK)
    return {};
  return {r, {}};
}

template <class T>
using SmartDecklinkPtr = std::unique_ptr<T, DecklinkPtrDeleter>;

// use it this way
// auto decklLink = make_decklink_ptr<IDeckLinkInput>(deckLink);

#endif  // PROJECT_DECKLINK_H
