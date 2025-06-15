#pragma once
#include "net_common.h"
#include <cstddef>
#include <cstring>
#include <memory>
#include <ostream>
namespace net_common {
template <typename t> struct message_header {
  t id;
  u_int32_t size_of_total_message = 0;
};

template <typename t> struct message {
  message_header<t> message_h;
  std::vector<u_int8_t> body;

  size_t size_of_message() { return (sizeof(message_h) + body.size()); }
  friend std::ostream &operator<<(std::ostream &os, message<t> &mess) {
    os << "ID:" << int(mess.message_h.id) << " -- "
       << "size of total m:" << mess.size_of_message()
       << " -- "
       // << "body content:" << int(mess.body.data()) << " -- "
       << "body size:" << mess.body.size() << std::endl;
    return os;
  }
  template <typename varT>
  friend message<t> &operator<<(message<t> &mess, varT &var) {
    static_assert(std::is_standard_layout<varT>::value,
                  "Data is too complex to use .");
    size_t size_of_input = sizeof(varT);
    size_t i = mess.body.size();
    mess.body.resize(mess.body.size() + size_of_input);
    memcpy(mess.body.data() + i, &var, size_of_input);
    mess.message_h.size_of_total_message = sizeof(message_h) + mess.body.size();
    return mess;
  }

  template <typename varT>
  friend message<t> &operator>>(message<t> &mess, varT &var) {
    static_assert(std::is_standard_layout<varT>::value,
                  "Data is too complex to use .");

    size_t size_of_output = sizeof(varT);
    size_t j = mess.body.size();
    memcpy(&var, mess.body.data() + j - size_of_output, size_of_output);
    mess.body.resize(mess.body.size() - size_of_output);
    mess.message_h.size_of_total_message = sizeof(message_h) + mess.body.size();
    return mess;
  }
};
template <typename T> struct connection {};
template <typename T> struct owned_message {
  message<T> mess;
  std::shared_ptr<connection<T>> remote =
      nullptr; // shared pointer for managing the ownership of the resourec it
               // points to , and efficiently deal with it / remove it when all
               // work related to it is done , or myabe async loops
  friend std::ostream &operator<<(std::ostream &os, owned_message<T> &ownMess) {
    os << ownMess.mess << std::endl;
    return os;
  };
};

} // namespace net_common
