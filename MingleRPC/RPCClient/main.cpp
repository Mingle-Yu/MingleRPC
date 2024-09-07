#include "CheckArguments.hpp"
#include "RPCClient.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    int res = checkArguments(argc, argv);

    if (res == VALID_IPV4_AND_PORT) {
        std::string ip = argv[2];
        int port = std::stoi(argv[4]);
        RPCClient client = RPCClient(ip, port);
        client.run();
    } else if (res == VALID_IPV6_AND_PORT) {
        std::cout << "Does not support IPv6 addresses!" << std::endl;
    }

    return res;
}