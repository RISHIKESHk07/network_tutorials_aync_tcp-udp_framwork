#pragma once
#include "../headers/message.h"
#include "../headers/net_common.h"
#include "../headers/net_server.h"

enum class Game { FIRE, WATER, EARTH };
net_common::message<Game> mess;
class CustomServer : public net_common::Server::server<Game> {};
