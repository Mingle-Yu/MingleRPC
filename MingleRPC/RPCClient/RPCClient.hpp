#include "Server.hpp"
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class RPCClient {
private:
    int dport; // port for service discovery
    int cport; // port for service calling
    std::string rIPAddr; // Registration center IP and port
    int rport;
    int dsockfd; // sockfd for TCP communication
    int csockfd;
    std::vector<Server> servers; // Discovered Services
    int selection; // The server receiving the request

private:
    bool discovery();
    void call();
    bool establishConnection(int &sockfd, std::string sip, int sport, int cport);
    std::vector<char> makePacket(std::string message);
    bool extractMessage(int sockfd, std::string &message);
    void processRResponse(std::string &response);
    void processSResponse(std::string &response);
    int balance();

public:
    RPCClient(std::string rIPAddr, int rport);
    void run();
};