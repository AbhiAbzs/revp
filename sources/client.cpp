#include "client.hpp"

#include <iostream>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/regex.hpp>
#include <boost/asio/write.hpp>

#include "common.hpp"

namespace asio = boost::asio;

Client::Client(std::string server_address, short unsigned int server_port)
    : server_address(server_address), server_port(server_port) { }

    void Client::run() {
        asio::ip::tcp::socket *server_socket = connect(server_address, std::to_string(server_port));
        asio::streambuf buffer;
        std::string message;
        for (;;) {
            asio::read_until(*server_socket, buffer, '\n');
            std::istream input_stream(&buffer);
            std::getline(input_stream, message);
            asio::ip::tcp::socket *proxy_socket = connect(server_address, message);
            std::thread t1(handleProxyConnection, proxy_socket);
            t1.detach();
        }
    }

asio::ip::tcp::socket *Client::connect(std::string address, std::string port) {
    try {
        asio::io_service ios;
        asio::ip::tcp::resolver::query resolver_query(address, port, asio::ip::tcp::resolver::query::numeric_service);
        asio::ip::tcp::resolver resolver(ios);
        boost::system::error_code ec;
        asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query, ec);
        asio::ip::tcp::resolver::iterator it_end;
        for (; it != it_end; ++it) {
            asio::ip::tcp::endpoint ep = it->endpoint();
            asio::ip::tcp::socket *socket = new asio::ip::tcp::socket(ios, ep.protocol());
            socket->connect(ep);
            if (socket->is_open())
                return socket;
            delete socket;
        }
        return nullptr;
    } catch (boost::system::system_error ex) {
        std::cout << "Attempt to connect to " << address << " failed" << std::endl;
        return nullptr;
    }
}

void Client::handleProxyConnection(asio::ip::tcp::socket *proxy_socket) {
    asio::streambuf buffer;
    std::string message;
    asio::read_until(*proxy_socket, buffer, boost::regex("HTTP/..."));
    std::istream input_stream2(&buffer);
    std::getline(input_stream2, message);
    std::cout << message << std::endl;
    boost::smatch what;
    if (boost::regex_match(message, what, boost::regex("CONNECT (.*):(.*) (HTTP/...).*"))) {
        asio::ip::tcp::socket *remote_socket = connect(what[1], what[2]);
        if (remote_socket == nullptr) {
            asio::write(*proxy_socket, asio::buffer("HTTP/1.1 404 Not Found\r\n\r\n"));
            return;
        }
        std::thread t1(connectSockets, proxy_socket, remote_socket);
        std::thread t2(connectSockets, remote_socket, proxy_socket);
        t1.detach();
        t2.detach();
        asio::write(*proxy_socket, asio::buffer("HTTP/1.1 200 OK\r\n\r\n"));
    } else {
        std::cout << "Proxy connection sent first message that is not a HTTP CONNECT statement, ignoring" << std::endl;
        asio::write(*proxy_socket, asio::buffer("HTTP/1.1 400 Bad Request\r\n\r\n"));
        return;
    }
}
