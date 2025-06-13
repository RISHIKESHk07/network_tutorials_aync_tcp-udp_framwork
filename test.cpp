#include <asio.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/detail/type_traits.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <functional>
#include <iostream>
#include <system_error>

class printer {

private:
  asio::steady_timer t;
  int count;

public:
  printer(asio::io_context &io)
      : count(0), t(asio::steady_timer(io, asio::chrono::seconds(1))) {
    t.async_wait(std::bind(&printer::print, this));
  }
  ~printer() { std::cout << "final" << count << std::endl; }
  void print() {
    std::cout << "HW" << std::endl; // appears six times
    if (count < 5) {
      ++count;
      t.expires_from_now(asio::chrono::seconds(1));
      t.async_wait(std::bind(&printer::print, this));
    }
  }
};

int main() {
  asio::io_context io;
  printer p1(io);
  io.run();
  return 0;
}
