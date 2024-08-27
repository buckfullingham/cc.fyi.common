#ifndef COMMON_SAFE_CAST_HPP
#define COMMON_SAFE_CAST_HPP

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace common {

// representation type, one of permitted by strict aliasing rule
// https://timsong-cpp.github.io/cppwp/n4659/basic.lval#8.8
template <typename T>
concept safe_ptr_castable_from =
    std::is_same_v<T, char> || std::is_same_v<T, unsigned char> ||
    std::is_same_v<T, std::byte>;

template <typename T>
concept safe_ptr_castable_to = std::is_trivially_copyable_v<T>;

/**
 * c.f.
 * https://arech.github.io/2024-08-17-reinterpret_cast-ub-and-a-pointer-casting-in-c++
 */
template <safe_ptr_castable_to To, safe_ptr_castable_from From>
To *safe_ptr_cast(From *const data, const std::size_t size) {
  // verify buffer size
  if (size < sizeof(To))
    throw std::runtime_error("safe_ptr_cast: size too small");

  // verify the pointer is properly aligned to contain To underneath
  if (reinterpret_cast<std::uintptr_t>(data) % alignof(To) != 0)
    throw std::runtime_error("safe_ptr_cast: misaligned data");

  // just a byte array, a temporary storage to backup the original mem
  // representation, since it can be distorted later
  From buf[sizeof(To)];
  std::memcpy(&buf[0], data, sizeof(To));

  // starting lifetime of To inside the data and perform non-vacuous
  // initialization https://timsong-cpp.github.io/cppwp/n4659/basic.life#1.2
  // that could change the underlying memory. Previous content is implicitly
  // destroyed https://timsong-cpp.github.io/cppwp/n4659/basic.life#5
  // Starting lifetime of To is mandatory because
  // https://timsong-cpp.github.io/cppwp/n4659/basic.life#4
  To *ptr = new (data) To;

  // replacing byte representation
  std::memcpy(ptr, &buf[0], sizeof(To));

  // now *ptr is created in data memory buffer, it has its lifetime started and
  // it has a proper byte representation. Since To is trivially copyable, it has
  // a trivial destructor, so there's no special need to call a destructor to
  // end its lifetime.
  return ptr;
}

} // namespace common

#endif // COMMON_SAFE_CAST_HPP
