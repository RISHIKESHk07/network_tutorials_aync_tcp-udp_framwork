#pragma once
#include "asio.hpp"
#include "message.h"
#include "net_queue.h"
#include <asio/io_context.hpp>
#include <sys/types.h>
namespace net_common {
namespace net_connection {
template <typename T>
class Connection : public std::enable_shared_from_this<Connection<T>> {
public:
  enum class owner { server, client };
  Connection(owner parent, asio::io_context &asioContext,
             asio::ip::tcp::socket socketI,
             ThreadSafe_Queue::TSQueue<owned_message<T>> &InwardQ)
      : m_asioContext(asioContext), m_socket(std::move(socketI)),
        InWardMessages(InwardQ) {
    // constructor
    m_ownerType = parent;
  }
  virtual ~Connection() {
    // Disconnecting properly
  }
  u_int32_t getid() { return id; }
  void connectToClient(u_int32_t uid = 0) {
    if (m_ownerType == owner::server) {
      if (m_socket.is_open())
        id = uid;
    }
  }
  bool connectToServer();
  bool disconnect();
  bool isConnected();
  bool send(message<T> &message);

protected:
  asio::io_context &m_asioContext;
  asio::ip::tcp::socket m_socket;
  u_int32_t id;
  ThreadSafe_Queue::TSQueue<message<T>> OutWardMessages;
  ThreadSafe_Queue::TSQueue<owned_message<T>> &InWardMessages;
  owner m_ownerType = owner::server;
};
//
} // namespace net_connection
} // namespace net_common
