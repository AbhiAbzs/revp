#include "common.hpp"

#include <boost/asio/ip/tcp.hpp>

namespace asio = boost::asio;

void connectSockets(asio::ip::tcp::socket *s1, asio::ip::tcp::socket *s2) {
    char buffer[128];
    for (;;) {
        try {
            std::size_t read = s1->read_some(asio::buffer(buffer, 128));
            s2->write_some(asio::buffer(buffer, read));
        } catch (boost::system::system_error ex) {
            break;
        }
    }
}

