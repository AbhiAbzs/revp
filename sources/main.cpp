#include <boost/program_options.hpp>
#include <iostream>

#include "server.hpp"
#include "client.hpp"

namespace po = boost::program_options;

int main(int argc, const char *argv[]) {
    bool server = false;
    bool client = false;
    short unsigned int client_port = 0;
    short unsigned int proxy_port = 0;
    std::string server_address;
    short unsigned int server_port = 0;

    po::options_description desc{"Options"};
    try {
        desc.add_options()
            ("help,h", "Help screen")
            ("server,s", "Starts revp in server mode")
            ("client,c", "Starts revp in client mode")
            ("clientPort,l", po::value<short unsigned int>(), "[Server mode] Port that listens for client")
            ("proxyPort,p", po::value<short unsigned int>(), "[Server mode] Port that listens for proxy connections")
            ("serverAddress,a", po::value<std::string>(), "[Client mode] Address of server")
            ("serverPort,z", po::value<short unsigned int>(), "[Client mode] Port of server");

        po::variables_map vm;
        po::store(parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(EXIT_SUCCESS);
        }
        if (vm.count("server"))
            server = true;
        if (vm.count("client"))
            client = true;
        if (vm.count("clientPort"))
            client_port = vm["clientPort"].as<short unsigned int>();
        if (vm.count("proxyPort"))
            proxy_port = vm["proxyPort"].as<short unsigned int>();
        if (vm.count("serverAddress"))
            server_address = vm["serverAddress"].as<std::string>();
        if (vm.count("serverPort"))
            server_port = vm["serverPort"].as<short unsigned int>();
    }
    catch (const po::error &ex) {
        std::cerr << ex.what() << std::endl;
    }

    if (server && client) {
        std::cout << "Both -s and -c options are provided, only one is required" << std::endl;
        std::cout << desc << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!server && !client) {
        std::cout << "Either -s or -c option is required" << std::endl;
        std::cout << desc << std::endl;
        exit(EXIT_FAILURE);
    }

    if (server) {
        if (client_port == 0 || proxy_port == 0) {
            std::cout << "Both -l and -p options are required in server mode" << std::endl;
            exit(EXIT_FAILURE);
        }
        Server server(client_port, proxy_port);
        server.run();
    }

    if (client) {
        if (server_address.empty() || server_port == 0) {
            std::cout << "Both -a and -z options are required in client mode" << std::endl;
            exit(EXIT_FAILURE);
        }
        Client client(server_address, server_port);
        client.run();
    }
}

