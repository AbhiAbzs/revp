#ifndef REVP_SERVER_H
#define REVP_SERVER_H

#include <boost/asio/ip/tcp.hpp>

class Server {
    public:
        Server(short unsigned int client_port, short unsigned int proxy_port);
        void run();
    private:
        short unsigned int client_port;
        short unsigned int proxy_port;
        boost::asio::ip::tcp::socket *listenForClient();
        boost::asio::ip::tcp::socket *listenForProxy();
        boost::asio::ip::tcp::socket *createProxyClientSocket(boost::asio::ip::tcp::socket *client_socket);
};

#endif
