#include "Server.hpp"
#include <vector>
#include <unordered_map>
#include <mutex>

class RegistrationCenter {
private:
    // std::string IPAddr; // rigistraction center IP address
    int cport; // server discovery port
    int sport; // service registration port
    std::unordered_map<std::string, Server> servers;
    std::mutex mtx; // client mutex

private:
    void handleRequest(int port, std::string requestType);
    void handleClientRequest(int sockfd);
    void handleServerRequest(int sockfd);
    bool extractMessage(int sockfd, std::string &message);
    std::vector<char> makePacket(std::string message);

public:
    RegistrationCenter();
    void run();
};