#include <asio.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <bits/stdc++.h>
#include <cstddef>
#include <ctime>
#include <exception>
#include <system_error>
#include <thread>

// constants & defines

// -----------------------
// server
class Server {
public:
  // a function to create a message

  // accept connections from clients
  Server(asio::io_context &io)
      : io_context(io),
        acceptorObj(asio::ip::tcp::acceptor(
            io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 12345))) {}
  void listen() {
    try {
      asio::ip::tcp::socket socket(io_context);

      acceptorObj.accept(socket);
      while (true) {
        std::string m = messageGeneratot();
        std::error_code ignored_error;
        asio::write(socket, asio::buffer(m), ignored_error);
      }
    } catch (std::exception &e) {
      std::cout << "Error:" << e.what() << std::endl;
    }
  }
  std::string messageGeneratot() {
    std::string str;
    std::time_t now = time(0);
    return ctime(&now);
  }
  void run() { listen(); }
  // forward message wait for more requests or close connection
private:
  asio::io_context &io_context;
  asio::ip::tcp::acceptor acceptorObj;
};
// -------------------------
// client
class Client {

public:
  Client(asio::io_context &io)
      : io_context(io), resolver(asio::ip::tcp::resolver(io)) {}
  void connectToSocket() {
    try {
      asio::ip::tcp::resolver::results_type endpoints =
          resolver.resolve("127.0.0.1", "12345");
      asio::ip::tcp::socket socket(io_context);
      asio::connect(socket, endpoints);
      while (true) {
        std::error_code error;
        size_t len = socket.read_some(asio::buffer(arr), error);
        if (error == asio::error::eof)
          break;
        else if (error)
          throw std::system_error(error);
        std::cout.write(arr.data(), len);
      }

    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }
  }
  void run() { connectToSocket(); }

private:
  asio::io_context &io_context;
  asio::ip::tcp::resolver resolver;
  std::array<char, 128> arr;
};
// -----------------------------
int main() {
  asio::io_context io1;
  asio::io_context io2;
  Server s1(io1);
  Client c1(io2);
  std::thread server_thread([&] { s1.run(); });
  std::thread client_thread([&] { c1.run(); });
  server_thread.join();
  client_thread.join();
  return 0;
}
