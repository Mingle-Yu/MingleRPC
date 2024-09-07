#include <string>
#include <vector>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include "Function.hpp"

class RPCServer {
private:
    static RPCServer* instance;
    std::string IPAddr;
    int gport; // registe
    int cport; //respoinse rpc
    std::string rIPAddr;
    int rport;
    int rsockfd;
    int csockfd;

private:
    std::string getEns33();
    static void signalHandler(int signal) {
        if (instance != nullptr) {
            instance->handleSignal(signal);
        }
    }
    void handleSignal(int signal);
    bool registe();
    void handleRequest();
    bool establishConnection(int &sockfd, std::string sip, int sport, int cport);
    std::vector<char> makePacket(std::string message);
    bool extractMessage(int sockfd, std::string &message);
    bool processRResponse(std::string &response);
    bool acceptConnection();
    void handleRequest(int clientSockfd);

public:
    RPCServer(std::string rIPAddr, int rport);
    void run();
};