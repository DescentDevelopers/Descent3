#ifndef STRHASH_H
#define STRHASH_H

// System
#include <cstdint>
#include <string>
#include <string_view>

constexpr uint64_t fnv_offset = 0xCBF29CE484222325;
constexpr uint64_t fnv_prime = 0x00000100000001B3;

template <typename string_type> constexpr uint64_t fnv1_64(string_type data) {
  uint64_t hash = fnv_offset;
  for (auto i : data) {
    hash *= fnv_prime;
    hash ^= i;
  }
  return hash;
}

static inline uint64_t hash(const char *str, std::size_t sz) noexcept {
  return fnv1_64<std::string>(std::string(str, sz));
}

static inline uint64_t hash(const char16_t *str, std::size_t sz) noexcept {
  return fnv1_64<std::u16string>(std::u16string(str, sz));
}

constexpr uint64_t operator"" _hash(const char *str, const std::size_t sz) noexcept {
  return fnv1_64<std::string_view>(std::string_view(str, sz));
}

constexpr uint64_t operator"" _hash(const char16_t *str, const std::size_t sz) noexcept {
  return fnv1_64<std::u16string_view>(std::u16string_view(str, sz));
}

#endif
