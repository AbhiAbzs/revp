#ifndef REVP_CLIENT_H
#define REVP_CLIENT_H

#include <string>
#include <boost/asio/ip/tcp.hpp>

class Client {
    public:
        Client(std::string server_address, short unsigned int server_port);
        void run();
    private:
        std::string server_address;
        short unsigned int server_port;
        static boost::asio::ip::tcp::socket *connect(std::string address, std::string port);
        static void handleProxyConnection(boost::asio::ip::tcp::socket *proxy_socket);
};

#endif
