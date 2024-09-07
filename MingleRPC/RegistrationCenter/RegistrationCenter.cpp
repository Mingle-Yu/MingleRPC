#include "RegistrationCenter.hpp"
#include <thread>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

RegistrationCenter::RegistrationCenter() {
    // IPAddr = "127.0.0.1";
    cport = 10000;
    sport = 10001;

    // std::string ip = "192.168.177.1"; // debug
    // int port = 2222;
    // servers[ip] = Server(ip, port);
    // ip = "192.168.177.2";
    // port = 3333;
    // servers[ip] = Server(ip, port);
    // ip = "192.168.177.3";
    // port = 4444;
    // servers[ip] = Server(ip, port);
}

void RegistrationCenter::run() {
    std::cout << "Registration center start..." << std::endl;
    std::thread serverThread([this, port = sport] { handleRequest(port, "Server Request"); });
    std::thread clientThread([this, port = cport] { handleRequest(port, "Client Request"); });
    serverThread.join();
    clientThread.join();
}

void RegistrationCenter::handleRequest(int port, std::string requestType) {
    int serverSockfd, clientSockfd;
    socklen_t clientLen;
    struct sockaddr_in serverAddr, clientAddr;

    serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSockfd < 0) {
        std::cout << requestType << ": Socket creation failed" << std::endl;
        close(serverSockfd);
        exit(1);
    }

    memset((char *) &serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << requestType << ": Failed to bind to port " << port << std::endl;
        close(serverSockfd);
        exit(1);
    }

    listen(serverSockfd, 5);
    clientLen = sizeof(clientAddr);
    
    while (true) {
        clientSockfd = accept(serverSockfd, (struct sockaddr *) &clientAddr, &clientLen);
        if (clientSockfd < 0) {
            close(clientSockfd);
            continue;
        }

        if (requestType == std::string("Server Request")) {
            handleServerRequest(clientSockfd);
            // std::cout << servers.size() << std::endl; // debug
            // for (auto &element: servers) {
            //     std::cout << element.second.toString();
            // }
        } else {
            handleClientRequest(clientSockfd);
        }
    }

    close(serverSockfd);
}

bool RegistrationCenter::extractMessage(int sockfd, std::string &message) {
    // 读取消息头，消息头是 4 字节，表示消息体的长度
    int msgLen;
    int n = read(sockfd, &msgLen, sizeof(msgLen));
    if (n < 0) {
        std::cout << "ERROR reading message length" << std::endl;
        close(sockfd);
        return false;
    }
    // msgLen = ntohl(msgLen); // 将消息长度从网络字节序转换为主机字节序 no need

    // 根据消息长度读取消息体
    std::vector<char> buffer(msgLen);
    n = read(sockfd, buffer.data(), msgLen);
    if (n < 0) {
        std::cout << "ERROR reading message body" << std::endl;
        close(sockfd);
        return false;
    }

    message = std::string(buffer.begin(), buffer.end());
    return true;
}

void RegistrationCenter::handleServerRequest(int sockfd) {
    std::string message;
    if (extractMessage(sockfd, message) == false) {
        std::cout << "Failed to read a server request" << std::endl;
        close(sockfd);
        return;
    }

    int idx = message.find(' ');
    std::string request = message.substr(0, idx);
    message = message.substr(idx + 1);
    idx = message.find(' ');
    std::string ip = message.substr(0, idx);
    message = message.substr(idx + 1);
    int port = std::stoi(message);
    // std::cout << request << std::endl; // debug
    
    mtx.lock();
    if (request == std::string("ServiceRegistration")) {
        servers[ip] = Server(ip, port);
        std::vector<char> packet = makePacket("OK");
        int n = write(sockfd, packet.data(), packet.size());
        if (n < 0) {
            std::cout << "Failed to send response to server" << std::endl;
            close(sockfd);
            return;
        }
        // std::cout << servers.size() << std::endl; // debug
    } else if (request == std::string("ServiceCancellation") && servers.find(ip) != servers.end()) {
        std::cout << "ServiceCancellation" << std::endl;
        servers.erase(ip);
    } else {
        std::cout << "An illegal request from the server" << std::endl;
    }
    mtx.unlock();

    close(sockfd);
}

std::vector<char> RegistrationCenter::makePacket(std::string message) {
    int msgLen = message.size();

    std::vector<char> packet(sizeof(int) + msgLen);
    std::memcpy(packet.data(), &msgLen, sizeof(int));
    std::memcpy(packet.data() + sizeof(int), message.data(), msgLen);

    return packet;
}

void RegistrationCenter::handleClientRequest(int sockfd) {
    std::string message;
    if (extractMessage(sockfd, message) == false) {
        std::cout << "Failed to read a client request" << std::endl;
        close(sockfd);
        return;
    }

    
    if (message != std::string("ServiceDiscovery")) {
        std::cout << message.size() << std::endl;
        std::cout << std::string("ServiceDiscovery").size() << std::endl;
        std::cout << "An illegal request from the client" << std::endl;
        close(sockfd);
        return;
    }

    std::string response;
    for (auto& element : servers) {
        response += element.second.toString();
    }

    if (response.size() == 0) {
        response = "0";
    }

    std::vector<char> packet = makePacket(response);
    int n = write(sockfd, packet.data(), packet.size());
    if (n < 0) {
        std::cout << "Failed to send response to client" << std::endl;
        close(sockfd);
        return;
    }

    close(sockfd);
}