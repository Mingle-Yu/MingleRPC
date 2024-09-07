#include "RPCServer.hpp"
#include <thread>
#include <csignal>

// OK
RPCServer* RPCServer::instance = nullptr;

RPCServer::RPCServer(std::string rIPAddr, int rport) {
    IPAddr = getEns33();
    if (IPAddr.size() == 0) {
        std::cout << "Failed to obtain the local IPv4 address" << std::endl;
        exit(1);
    }
    gport = 20000;
    cport = 20001;
    this->rIPAddr = rIPAddr;
    this->rport = rport;

    instance = this;
    std::signal(SIGINT, RPCServer::signalHandler);
}
// OK
void RPCServer::run() {
    // Service Registration
    if (registe() == false) {
        std::cout << "Service registration failed" << std::endl;
        close(rsockfd);
        exit(1);
    }
    close(rsockfd);
    // Waiting for client request
    if (acceptConnection() == false) {
        close(csockfd);
        exit(1);
    }

    while (true) {
        int clientSockfd = accept(csockfd, nullptr, nullptr);
        if (clientSockfd < 0) {
            std::cout << "Accept failed" << std::endl;
            continue;
        }
        std::thread clientThread([this, sockfd = clientSockfd] { handleRequest(sockfd); });
        clientThread.detach();
    } 
}
// OK
std::string RPCServer::getEns33() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, "ens33", IFNAMSIZ - 1); // 指定网卡接口名
    std::string ip;

    if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0) {
        struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr->sin_addr, ipstr, sizeof(ipstr));
        ip = std::string(ipstr);
    }

    close(sockfd);
    return ip;
}

void RPCServer::handleSignal(int signal) {
    bool res = establishConnection(rsockfd, rIPAddr, rport, gport);
    if (res == false) {
        std::cout << "Failed to connect to the registry" << std::endl;
        close(rsockfd);
    } else {
        std::cout << "Connected to the registry successfully" << std::endl;
        // send request
        std::string request = std::string("ServiceCancellation ") + IPAddr + " " + std::to_string(cport);
        std::vector<char> packet = makePacket(request);
        int n = write(rsockfd, packet.data(), packet.size());
        if (n < 0) {
            std::cout << "Failed to send service cancellation request" << std::endl;
            close(rsockfd);
        }
        // receive response
        std::string response;
        if (extractMessage(rsockfd, response) == false) {
            close(rsockfd);
        }
    }
    close(csockfd);
    exit(0);
}
// OK
bool RPCServer::registe() {
    bool res = establishConnection(rsockfd, rIPAddr, rport, gport);
    if (res == false) {
        std::cout << "Failed to connect to the registry" << std::endl;
        close(rsockfd);
        return false;
    } else {
        std::cout << "Connected to the registry successfully" << std::endl;
        // send request
        std::string request = std::string("ServiceRegistration ") + IPAddr + " " + std::to_string(cport);
        std::vector<char> packet = makePacket(request);
        int n = write(rsockfd, packet.data(), packet.size());
        if (n < 0) {
            std::cout << "Failed to send service registration request" << std::endl;
            close(rsockfd);
            return false;
        }
        // receive response
        std::string response;
        if (extractMessage(rsockfd, response) == false) {
            close(rsockfd);
            return false;
        }
        return processRResponse(response);
    }
}
// OK
bool RPCServer::establishConnection(int &sockfd, std::string sip, int sport, int cport) {
    // 创建 socket 文件描述符
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cout << "Socket creation error" << std::endl;
        return false;
    }

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(cport);

    // 绑定客户端 socket 到特定的本地端口
    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return false;
    }


    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; // 设置地址族为 IPv4
    serv_addr.sin_port = htons(sport); // 设置端口号并将其转换为网络字节顺序

    // 将IP地址转换为二进制格式并存储在 serv_addr.sin_addr 中
    if (inet_pton(AF_INET, sip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        return false;
    }

    // 尝试连接到服务器
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }

    return true;
}
// OK
std::vector<char> RPCServer::makePacket(std::string message) {
    int msgLen = message.size();

    std::vector<char> packet(sizeof(int) + msgLen);
    std::memcpy(packet.data(), &msgLen, sizeof(int));
    std::memcpy(packet.data() + sizeof(int), message.data(), msgLen);

    return packet;
}
// OK
bool RPCServer::extractMessage(int sockfd, std::string &message) {
    // 读取消息头，消息头是 4 字节，表示消息体的长度
    int msgLen;
    int n = read(sockfd, &msgLen, sizeof(msgLen));
    if (n < 0) {
        std::cout << "ERROR reading message length" << std::endl;
        return false;
    }
    // msgLen = ntohl(msgLen); // 将消息长度从网络字节序转换为主机字节序 no need
    
    // 根据消息长度读取消息体
    std::vector<char> buffer(msgLen);
    n = read(sockfd, buffer.data(), msgLen);
    if (n < 0) {
        std::cout << "ERROR reading message body" << std::endl;
        return false;
    }

    message = std::string(buffer.begin(), buffer.end());
    return true;
}
// OK
bool RPCServer::processRResponse(std::string &response) {
    return response == std::string("OK");
}

// OK
bool RPCServer::acceptConnection() {
        csockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (csockfd < 0) {
            std::cout << "Socket creation error" << std::endl;
            return false;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(cport);

        if (bind(csockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            std::cout << "Bind failed" << std::endl;
            close(csockfd);
            return false;
        }

        if (listen(csockfd, 10) < 0) {
            std::cout << "Listen failed" << std::endl;
            close(csockfd);
            return false;
        }

        std::cout << "Server is listening on port " << cport << std::endl;
        return true;
}

void RPCServer::handleRequest(int clientSockfd) {
    // std::cout << "handleRequest1" << std::endl; // debug
    std::string request;
    std::string result;
    std::string funcName;
    std::string arguments;
    if (extractMessage(clientSockfd, request) == false) {
        close(clientSockfd);
        return;
    }
    // std::cout << request << std::endl; // debug
    // std::cout << "handleRequest2" << std::endl; // debug
    int idx = request.find(' ');
    if (idx != std::string::npos) {
        funcName = request.substr(0, idx);
        arguments = request.substr(idx + 1);
    }
    
    // std::cout << funcName << " " << arguments << std::endl; // debug
    if (funcName == std::string("add")) {
        Add add(arguments);
        result = add();
    } else if (funcName == std::string("sub")) {
        Sub sub(arguments);
        result = sub();
    } else if (funcName == std::string("mul")) {
        Mul mul(arguments);
        result = mul();
    } else if (funcName == std::string("div")) {
        Div div(arguments);
        result = div();
    } else if (funcName == std::string("sqr")) {
        Sqr sqr(arguments);
        result = sqr();
    } else if (funcName == std::string("sqrt")) {
        Sqrt sqrt(arguments);
        result = sqrt();
    } else if (funcName == std::string("cube")) {
        Cube cube(arguments);
        result = cube();
    } else if (funcName == std::string("echo")) {
        Echo echo(arguments);
        result = echo();
    } else if (funcName == std::string("reverse")) {
        Reverse reverse(arguments);
        result = reverse();
    } else if (funcName == std::string("fact")) {
        Fact fact(arguments);
        result = fact();
    } else if (funcName == std::string("quit")) {
        close(clientSockfd);
        return;
    } else {
        result = "No such function or invalid request";
    }
    // std::cout << "handleRequest3" << std::endl; // debug
    std::vector<char> packet = makePacket(result);
    int n = write(clientSockfd, packet.data(), packet.size());
    if (n < 0) {
        std::cout << "Failed to send rpc result" << std::endl;
        close(clientSockfd);
    }
    // std::cout << "handleRequest4" << std::endl; // debug
}