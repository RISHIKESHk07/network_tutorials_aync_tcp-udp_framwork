// in this program the size of the info we fetch and the time it takes to go
// back and forth makes it give different sometimes , inorder to solve that we
// use async version of it ,
//
// Answer:
// here we have a code structure which executes synv for the connect and request
// part and the oflloades the handlinf of reading to io_context.run() in a
// separate thread which does not block the current thread from doing smtg else
// , replicated using a thread sleep at the end
//
#include <asio.hpp>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <iostream>
#include <system_error>
#include <thread>

/* int main() {
  asio::io_context io;
  std::error_code ec;
  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec),
                                   80);
  asio::ip::tcp::socket socket(io);
  std::error_code err = socket.connect(endpoint, ec);

  if (!err) {
    std::cout << "COnnection estb ..." << std::endl;
  } else {
    std::cerr << ec.message() << std::endl;
  }

  if (socket.is_open()) {
    std::string sreq = "GET /index.html HTTP/1.1\r\n"
                       "Host: david-barr.co.uk\r\n"
                       "Connection: close\r\n\r\n";

    socket.write_some(asio::buffer(sreq.data(), sreq.size()), err);
    socket.wait(socket.wait_read);
    auto bytes = socket.available();

    std::cout << bytes << std::endl;

    if (bytes > 0) {
      std::vector<char> v1(bytes);
      socket.read_some(asio::buffer(v1.data(), v1.size()), err);
      for (auto i : v1)
        std::cout << i;
    }
  }

  return 0;
} */
std::vector<char> vec(1024 * 10);
void AsynCall(asio::ip::tcp::socket &socket) {
  socket.async_read_some(asio::buffer(vec.data(), vec.size()),
                         [&](std::error_code err, std::size_t bytes) {
                           if (!err) {
                             for (auto i : vec) {
                               std::cout << i;
                             }
                             AsynCall(socket);
                           } else {
                             std::cout << "Err:" << err.message() << std::endl;
                           }
                         });
}
int main() {
  asio::io_context io;
  std::error_code ec;
  asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec),
                                   80);
  asio::ip::tcp::socket socket(io);
  std::error_code err = socket.connect(endpoint, ec);
  std::thread thread_con = std::thread([&] { io.run(); });
  if (!ec) {
    std::cout << "COnnection estb ..." << std::endl;
  } else {
    std::cerr << ec.message() << std::endl;
  }

  if (socket.is_open()) {
    AsynCall(socket);
    std::string sreq = "GET /index.html HTTP/1.1\r\n"
                       "Host: david-barr.co.uk\r\n"
                       "Connection: close\r\n\r\n";

    socket.write_some(asio::buffer(sreq.data(), sreq.size()), ec);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2000ms);
  }

  return 0;
}
