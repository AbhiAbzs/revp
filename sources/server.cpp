#include "server.hpp"

#include <iostream>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "common.hpp"

namespace asio = boost::asio;

Server::Server(short unsigned int client_port, short unsigned int proxy_port)
    : client_port(client_port), proxy_port(proxy_port) { }

    void Server::run() {
        asio::ip::tcp::socket *client_socket = listenForClient();
        std::cout << "Listening for a proxy connections on port " << proxy_port << std::endl;
        for (;;) {
            asio::ip::tcp::socket *proxy_socket = listenForProxy();
            std::cout << "New proxy connection initiated, creating a proxy-client socket" << std::endl;
            asio::ip::tcp::socket *proxy_client_socket = createProxyClientSocket(client_socket);
            std::thread t1(connectSockets, proxy_socket, proxy_client_socket);
            std::thread t2(connectSockets, proxy_client_socket, proxy_socket);
            t1.detach();
            t2.detach();
        }
    }

asio::ip::tcp::socket *Server::listenForClient() {
    asio::io_service ios;
    asio::ip::tcp::endpoint client_ep(asio::ip::address_v4::any(), client_port);
    asio::ip::tcp::acceptor acceptor(ios, client_ep.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(client_ep);
    acceptor.listen(3);
    std::cout << "Listening for a client on port " << client_port << std::endl;
    asio::ip::tcp::socket *client_socket = new asio::ip::tcp::socket(ios);
    acceptor.accept(*client_socket);
    std::cout << "Client connected, stopping listening on " << client_port << std::endl;
    acceptor.close();
    return client_socket;
}

asio::ip::tcp::socket *Server::listenForProxy() {
    asio::io_service ios;
    asio::ip::tcp::endpoint proxy_ep(asio::ip::address_v4::any(), proxy_port);
    asio::ip::tcp::acceptor acceptor(ios, proxy_ep.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(proxy_ep);
    acceptor.listen(3);
    asio::ip::tcp::socket *proxy_socket = new asio::ip::tcp::socket(ios);
    acceptor.accept(*proxy_socket);
    return proxy_socket;
}

asio::ip::tcp::socket *Server::createProxyClientSocket(asio::ip::tcp::socket *client_socket) {
    asio::io_service ios;
    asio::ip::tcp::acceptor proxy_client_connection_acceptor(ios);
    unsigned short port(0);
    asio::ip::tcp::endpoint proxy_client_connection_ep(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    proxy_client_connection_acceptor.open(proxy_client_connection_ep.protocol());
    proxy_client_connection_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    proxy_client_connection_acceptor.bind(proxy_client_connection_ep);
    proxy_client_connection_acceptor.listen();
    port = proxy_client_connection_acceptor.local_endpoint().port();
    asio::ip::tcp::socket *proxy_client_connection_socket = new asio::ip::tcp::socket(ios);
    std::cout << "Proxy-client socket listening for client created on port " << port << std::endl;
    std::stringstream ss;
    ss << port << "\n";
    asio::write(*client_socket, asio::buffer(ss.str()));
    proxy_client_connection_acceptor.accept(*proxy_client_connection_socket);
    std::cout << "Client connected to proxy-client socket at " << port << std::endl;
    return proxy_client_connection_socket;
}

