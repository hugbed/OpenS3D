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

template <typename T>
struct decklink_iid {
  // general case not defined
};

template <>
struct decklink_iid<IDeckLinkInput> {
  constexpr static const REFIID value = IID_IDeckLinkInput;
};

template <>
struct decklink_iid<IDeckLink> {
  constexpr static const REFIID value = IID_IDeckLink;
};

template <>
struct decklink_iid<IDeckLinkConfiguration> {
  constexpr static const REFIID value = IID_IDeckLinkConfiguration;
};

template <class U, class T>
std::unique_ptr<U, DecklinkPtrDeleter> make_decklink_ptr(T const& src) {
  if (!src)
    return {};
  U* r = nullptr;
  if (src->QueryInterface(decklink_iid<U>::value, (void**)&r) != S_OK)
    return {};
  return {r, {}};
}

template <class T>
using SmartDecklinkPtr = std::unique_ptr<T, DecklinkPtrDeleter>;

// use it this way
// auto decklLink = make_decklink_ptr<IDeckLinkInput>(deckLink);

#endif //PROJECT_DECKLINK_H
