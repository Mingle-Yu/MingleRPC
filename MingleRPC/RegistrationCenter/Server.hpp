#include <string>

class Server {
private:
    std::string IPAddress;
    int port;

public:
    Server() {}

    Server(std::string IPAddress, int port) {
        this->IPAddress = IPAddress;
        this->port = port;
    }

    std::string toString() {
        return IPAddress + " " + std::to_string(port) + "\n";
    }
};