#ifndef COMMON_H
#define COMMON_H

#include <boost/asio/ip/tcp.hpp>

void connectSockets(boost::asio::ip::tcp::socket *s1, boost::asio::ip::tcp::socket *s2);

#endif
