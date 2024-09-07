#include "CheckArguments.hpp"
#include <cstring>
#include <arpa/inet.h>
#include <iostream>

bool isValidIPv4AndPort(const std::string &ip, const int port) {
    struct sockaddr_in sa;
    bool flag = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) == 1 && (0 < port && port <= 65535);
    return flag;
}

bool isValidIPv6AndPort(const std::string &ip, const int port) {
    struct sockaddr_in6 sa;
    bool flag = inet_pton(AF_INET, ip.c_str(), &(sa.sin6_addr)) == 1 && (0 < port && port <= 65535);
    return flag;
}

int checkIPAddressAndPort(const std::string &ip, const int port) {
    if (isValidIPv4AndPort(ip, port)) {
        return VALID_IPV4_AND_PORT;
    } else if (isValidIPv6AndPort(ip, port)) {
        return VALID_IPV6_AND_PORT;
    } else {
        return INVALID_IP_ADDR_OR_PORT;
    }
}

int checkArguments(int argc, char* argv[]) {
    if (argc != 2 && argc != 5) {
        std::cout << "Wrong number of parameters!" << std::endl;
        std::cout << "Input \"./Server -h\" for help!" << std::endl;
        return WRONG_NUMBER_OF_PARAMETERS;
    }

    if (argc == 2 && std::strcmp(argv[1], "-h") != 0) {
        std::cout << "The number of parameters is correct, but the parameters are invalid" << std::endl;
        std::cout << "Input \"./Server -h\" for help!" << std::endl;
        return INVALID_PARAMETERS;
    }

    if (argc == 5 && (std::strcmp(argv[1], "-i") != 0 || std::strcmp(argv[3], "-p") != 0)) {
        std::cout << "The number of parameters is correct, but the parameters are invalid" << std::endl;
        std::cout << "Input \"./Server -h\" for help!" << std::endl;
        return INVALID_PARAMETERS;
    }

    if (argc == 2 && std::strcmp(argv[1], "-h") == 0) {
        std::cout << "Input \"./Server -i ipaddress -p port\" to start Client!" << std::endl;
        std::cout << "ipaddress is the IP address of the registration center, port is the port of the registration center" << std::endl;
        return OK;
    }

    if (argc == 5 && std::strcmp(argv[1], "-i") == 0 && std::strcmp(argv[3], "-p") == 0) {
        std::string ipaddr = argv[2];
        int port;
        try {
            port = std::stoi(argv[4]);
        } catch (const std::invalid_argument& e) {
            std::cout << "Invalid argument: " << e.what() << std::endl;
            return INVALID_IP_ADDR_OR_PORT;
        } catch (const std::out_of_range& e) {
            std::cout << "Out of range: " << e.what() << std::endl;
            INVALID_IP_ADDR_OR_PORT;
        }

        return checkIPAddressAndPort(ipaddr, port);
    }

    return OK;
}