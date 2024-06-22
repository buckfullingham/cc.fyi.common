#include "catch2/catch_all.hpp"

#include <common/io.hpp>

#include <random>
#include <string_view>

#include <dlfcn.h>

namespace ns = common::io;
using namespace std::literals;

namespace {
int close_count = 0;
}

int close(int fd) {
  static auto real_close =
      reinterpret_cast<decltype(&::close)>(::dlsym(RTLD_NEXT, "close"));
  CHECK(fd != -1);
  const auto result = real_close(fd);
  ++close_count;
  return result;
}

TEST_CASE("ring_buffer") {
  ns::ring_buffer rb(1 << 12);

  CHECK(*rb.addr(0) == *(rb.addr(0) + (1 << 12)));
  *rb.addr(0) += 1;
  CHECK(*rb.addr(0) == *(rb.addr(0) + (1 << 12)));
  *rb.addr(1 << 12) += 1;
  CHECK(*rb.addr(0) == *(rb.addr(0) + (1 << 12)));
}

struct ofstreambuf_fixture {
  ns::file_descriptor fd{::memfd_create, "", 0};
  const int sbfd = ::dup(fd.value());
  ns::ofstreambuf sb{ns::file_descriptor([this]() { return sbfd; }), 1 << 12};
  std::ostream os{&sb};
};

TEST_CASE_METHOD(ofstreambuf_fixture, "output one string longer than buffer") {
  auto input = []() {
    std::array<char, 1 << 20> result{};
    std::mt19937 prng(42);
    std::uniform_int_distribution<unsigned char> dist(1, 255);
    std::generate_n(result.begin(), result.size() - 1,
                    [&]() { return dist(prng); });
    return result;
  }();

  decltype(input) output{};

  os << input.begin();
  os.flush();
  CHECK(os.good());

  ns::posix_call(::lseek, fd.value(), 0, SEEK_SET);
  ns::posix_call(::read, fd.value(), output.begin(), output.size());

  CHECK(input == output);
}

TEST_CASE_METHOD(ofstreambuf_fixture, "write an int") {
  os << 42;
  os.flush();
  CHECK(os.good());

  std::array<char, 1 << 10> output{};

  ns::posix_call(::lseek, fd.value(), 0, SEEK_SET);
  ns::posix_call(::read, fd.value(), output.begin(), output.size());

  CHECK("42"sv == output.begin());
}

TEST_CASE_METHOD(ofstreambuf_fixture, "failure to write marks stream bad") {
  ::close(sbfd);
  CHECK(os.good());
  os << 42;
  os.flush();
  CHECK(os.bad());
  CHECK(!os.good());
}

TEST_CASE("construct and destruct file_descriptor") {
  close_count = 0;
  {
    common::io::file_descriptor from(::memfd_create, "", 0);
    CHECK(close_count == 0);
  }
  CHECK(close_count > 0);
}

TEST_CASE("move assignment of file_descriptor") {
  close_count = 0;
  {
    common::io::file_descriptor from(::memfd_create, "", 0);
    common::io::file_descriptor to(::memfd_create, "", 0);
    CHECK(close_count == 0);
    to = std::move(from);
    CHECK(std::exchange(close_count, 0) > 0);
  }
  CHECK(close_count > 0);
}

TEST_CASE("move construction of file_descriptor") {
  close_count = 0;
  {
    common::io::file_descriptor from(::memfd_create, "", 0);
    common::io::file_descriptor to(std::move(from));
    CHECK(close_count == 0);
  }
  CHECK(close_count > 0);
}
