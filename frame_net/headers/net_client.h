#include "message.h"
#include "net_connection.h"
#include <arpa/inet.h>
#include <asio/error_code.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/tcp.hpp>
#include <memory>
#include <string>
#include <sys/types.h>
#include <system_error>
namespace net_common {

namespace client {
template <typename T> class Client {
public:
  Client() : msocket(ogIo_context) {}
  virtual ~Client() { this->disconnect(); }
  void connect(std::string &host, u_int16_t port) {
    try {
      mconnection =
          std::make_unique<net_connection::Connection<T>>(ogIo_context);
      asio::ip::tcp::resolver resolver(ogIo_context);
      auto endpoints = resolver.resolve(host, std::to_string(port));
      mconnection->connectToServer();
      thrContext = std::thread([this]() { ogIo_context.run(); });
    } catch (std::error_code &err) {
      std::cout << err.message() << std::endl;
    }
  }
  void disconnect() {}
  void contentsofQ() {}
  bool isConnected() {}

protected:
  asio::io_context &ogIo_context;
  asio::ip::tcp::socket msocket;
  std::unique_ptr<net_connection::Connection<T>> mconnection;
  std::thread thrContext;

private:
  ThreadSafe_Queue::TSQueue<owned_message<T>> internalQueue;
};
} // namespace client
} // namespace net_common
