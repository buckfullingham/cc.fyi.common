#include "catch2/catch_all.hpp"

#include <common/safe_ptr_cast.hpp>

namespace {

struct some_struct {
  unsigned long ul;
  char c;
  short s;
};

} // namespace

TEST_CASE("valid safe_ptr_cast") {
  some_struct s{};

  char *const ptr = reinterpret_cast<char *>(&s);
  const std::size_t len = sizeof(some_struct);

  static_assert(
      std::is_same_v<some_struct *,
                     decltype(common::safe_ptr_cast<some_struct>(ptr, len))>);

  CHECK(common::safe_ptr_cast<some_struct>(ptr, len));
}

TEST_CASE("invalid size safe_ptr_cast") {
  some_struct s{};

  char *const ptr = reinterpret_cast<char *>(&s);
  const std::size_t len = sizeof(some_struct) - 1;

  CHECK_THROWS(common::safe_ptr_cast<some_struct>(ptr, len));
}

TEST_CASE("invalid alignment safe_ptr_cast") {
  some_struct s{};
  char buf[sizeof(some_struct) + 1];
  std::memcpy(&buf[1], &s, sizeof(s));
  char *const ptr = reinterpret_cast<char *>(&buf[1]);
  const std::size_t len = sizeof(buf) - 1;

  CHECK_THROWS(common::safe_ptr_cast<some_struct>(ptr, len));
}
