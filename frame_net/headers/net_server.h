#include "message.h"
#include "net_connection.h"
#include <algorithm>
#include <asio/io_context.hpp>
#include <deque>
#include <memory>
#include <pstl/glue_algorithm_defs.h>
#include <sys/types.h>
#include <system_error>
#include <thread>
namespace net_common {
namespace Server {
template <typename T> class server {
public:
  server(u_int16_t port)
      : acceptorObj(m_io_context,
                    asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}
  virtual ~server() { Stop(); }
  void Start() {

    try {

      WaitForClientConnection();
      ServerThread = std::thread([this]() { m_io_context.run(); });
    } catch (std::exception &err) {
      std::cout << "SERVER:" << err.what() << std::endl;
    }
    std::cout << "SERVER: STARTED ... " << std::endl;
  }
  void Stop() {
    m_io_context.stop();
    if (ServerThread.joinable())
      ServerThread.join(); // join make sure child thread completes its work
  }
  void WaitForClientConnection() {
    acceptorObj.async_accept(
        [this](std::error_code &ec, asio::ip::tcp::socket socket) {
          if (!ec) {
            std::cout << "SERVER: New connection..." << socket.remote_endpoint()
                      << "\n";
            std::shared_ptr<connection<T>> newConnection =
                std::make_shared<T>(connection<T>::owner::server, m_io_context,
                                    std::move(socket), internalServerQ);
            if (onClientConnect(newConnection)) {
              m_deqconnections.push_back(newConnection);
              m_deqconnections.back()->ConnectToClient(nIdcounter++);
              std::cout << "[" << m_deqconnections.back()->getID()
                        << "]Connection estb \n";
            } else {

              std::cout << "SERVER:Client denied access .." << std::endl;
            }
            WaitForClientConnection();
          }
        });
  }
  void MessageClient(std::shared_ptr<connection<T>> conn, message<T> &mess) {
    if (conn & conn->isconnected()) {
      conn->send();
    } else {
      OnClientDisconnect(conn);
      conn.reset();
      m_deqconnections.erase(
          std::remove(m_deqconnections.begin(), m_deqconnections.end(), conn),
          m_deqconnections.end());
    }
  }
  void MessageAllClients(const message<T> &msg,
                         std::shared_ptr<connection<T>> pIgnored = nullptr) {
    bool bInvalidExists = false;
    for (auto conn : m_deqconnections) {
      if (conn && conn->isconnected()) {
        if (conn != pIgnored)
          conn->send(msg);
      } else {
        OnClientDisconnect(conn);
        conn.reset();
        bInvalidExists = true;
      }
    }
  }
  void Update(size_t messageCount = -1) {
    size_t nCount = 0;
    while (nCount < messageCount && !internalServerQ.empty()) {
      auto msg = internalServerQ.pop_front();
      onMessage(msg.remote,
                msg.msg); // process set number of messages , so that server
                          // does not get stuck upon only push messages to q
      nCount++;
    }
  }

protected:
  virtual bool OnClientConnect(std::shared_ptr<connection<T>> client) {
    return false;
  }
  virtual void OnClientDisconnect(net_connection::Connection<T> client) {}
  virtual void onMessage(std::shared_ptr<net_connection::Connection<T>> client,
                         message<T> &message) {}

  asio::io_context &m_io_context;
  std::deque<std::shared_ptr<net_connection::Connection<T>>> m_deqconnections;
  ThreadSafe_Queue::TSQueue<T> internalServerQ;
  std::thread ServerThread;
  asio::ip::tcp::acceptor acceptorObj;
  u_int32_t IdCouter = 10000;
};
} // namespace Server
} // namespace net_common
