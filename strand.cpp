#include <asio.hpp>
#include <asio/bind_executor.hpp>
#include <asio/detail/chrono.hpp>
#include <asio/execution_context.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <asio/strand.hpp>
#include <iostream>
#include <thread>

class printer {
private:
  asio::steady_timer timer1_;
  asio::steady_timer timer2_;
  int count_;
  asio::strand<asio::io_context::executor_type> strand_;

public:
  printer(asio::io_context &io)
      : count_(0), timer1_(io, asio::chrono::seconds(1)),
        timer2_(io, asio::chrono::seconds(1)), strand_(asio::make_strand(io)) {
    timer1_.async_wait(
        asio::bind_executor(strand_, std::bind(&printer::print1, this)));
    timer2_.async_wait(
        asio::bind_executor(strand_, std::bind(&printer::print2, this)));
  }
  void print1() {
    if (count_ < 10) {
      std::cout << "Timer 1: " << count_ << std::endl;
      ++count_;

      timer1_.expires_at(timer1_.expiry() + asio::chrono::seconds(1));

      timer1_.async_wait(
          asio::bind_executor(strand_, std::bind(&printer::print1, this)));
    }
  }

  void print2() {
    if (count_ < 10) {
      std::cout << "Timer 2: " << count_ << std::endl;
      ++count_;

      timer2_.expires_at(timer2_.expiry() + asio::chrono::seconds(1));

      timer2_.async_wait(
          asio::bind_executor(strand_, std::bind(&printer::print2, this)));
    }
  }
};

int main() {
  asio::io_context io;
  printer p1(io);
  std::thread t([&] { io.run(); });
  t.join();
}
