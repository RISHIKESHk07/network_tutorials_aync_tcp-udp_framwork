#include "../headers/olc_net.h"
#include <sys/types.h>

enum class GAME : u_int32_t { FIRE, MOVEMENT };

net_common::message<GAME> mess1;

int main() {
  mess1.message_h.id = GAME::MOVEMENT;
  std::cout << mess1 << std::endl;
  int a = 2;
  float b = 3.2;

  mess1 << a << b;

  std::cout << mess1 << std::endl;
  float c;
  int d;

  mess1 >> c >> d;

  std::cout << c << " -- " << d << std::endl;
  return 0;
}
