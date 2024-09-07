#include "RPCClient.hpp"
// OK
RPCClient::RPCClient(std::string rIPAddr, int rport) {
    dport = 0;
    cport = 0;
    this->rIPAddr = rIPAddr;
    this->rport = rport;
    selection = 0;
    servers.push_back(Server("127.0.0.1", 20001));
}

void RPCClient::run() {
    // Service Discovery
    if (discovery() == false) {
        std::cout << "Service discovery failed" << std::endl;
        exit(1);
    }
    // Remote Procedure Call
    call();
}
// OK
bool RPCClient::discovery() {
    bool res = establishConnection(dsockfd, rIPAddr, rport, dport);
    if (res == false) {
        std::cout << "Failed to connect to the registry" << std::endl;
        close(dsockfd);
        return false;
    } else {
        std::cout << "Connected to the registry successfully\n";
        // send request
        std::vector<char> request = makePacket("ServiceDiscovery");
        int n = write(dsockfd, request.data(), request.size());
        if (n < 0) {
            std::cout << "Failed to send response to client" << std::endl;
            close(dsockfd);
            return false;
        }
        // receive response
        std::string response;
        if (extractMessage(dsockfd, response) == false) {
            close(dsockfd);
            return false;
        }
        if (response == std::string("0")) {
            std::cout << "No registered service" << std::endl;
            close(dsockfd);
            return false;
        }
        processRResponse(response);
        close(dsockfd);

        return true;
    }
}

void RPCClient::call() {
    int counter1 = 0;
    int counter2 = 0;
    while (true) {
        // Load balancing, server selection
        int idx = balance();
        if (idx == -1) {
            std::cout << "Serverless service provision" << std::endl;
            exit(1);
        }
        // Establish a connection with the server
        std::string serverIP = servers[idx].IPAddress;
        int sport = servers[idx].port;
        bool res = establishConnection(csockfd, serverIP, sport, cport);
        if (res == false) {
            std::cout << "Failed to connect to the server " << serverIP << std::endl;
            close(csockfd);
            ++counter1;
            if (counter1 >= 3) {
                counter1 = 0;
                ++counter2;
                if (counter2 >= 2) {
                    std::cout << "The remote procedure call failed 6 times in a row, exiting" << std::endl;
                    exit(1);
                }
                std::cout << "The connection establishment failed 3 times in a row. Update the service" << std::endl;
                if (discovery() == false) {
                    std::cout << "Service update failed" << std::endl;
                    exit(1);
                }
            }
            continue;
        }
        counter1 = 0;
        counter2 = 0;
        // Waiting for customer input
        std::cout << "Call the remote procedure like this: funcName parameter1 parameter2..." << std::endl;
        std::string request;
        getline(std::cin, request);
        if (request == std::string("quit")) {
            std::vector<char> packet = makePacket(request);
            int n = write(csockfd, packet.data(), packet.size());
            close(csockfd);
            close(dsockfd);
            exit(0);
        }
        // send request
        std::vector<char> packet = makePacket(request);
        int n = write(csockfd, packet.data(), packet.size());
        if (n < 0) {
            std::cout << "Failed to send remote procedure call request" << std::endl;
            close(csockfd);
            continue;
        }
        // Receive Response
        std::string response;
        if (extractMessage(csockfd, response) == false) {
            close(csockfd);
            continue;
        }
        // std::cout << response << std::endl; // debug
        close(csockfd);
        processSResponse(response);
    }
}

// OK
bool RPCClient::establishConnection(int &sockfd, std::string sip, int sport, int cport) {
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

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cout << "Failed to set SO_REUSEADDR" << std::endl;
        return false;
    }

    // 绑定客户端 socket 到特定的本地端口
    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        std::cout << "Bind failed" << std::endl;
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
        std::cout << "Connection Failed" << std::endl;
        return false;
    }

    return true;
}
// OK
std::vector<char> RPCClient::makePacket(std::string message) {
    int msgLen = message.size();

    std::vector<char> packet(sizeof(int) + msgLen);
    std::memcpy(packet.data(), &msgLen, sizeof(int));
    std::memcpy(packet.data() + sizeof(int), message.data(), msgLen);

    return packet;
}
// OK
bool RPCClient::extractMessage(int sockfd, std::string &message) {
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
void RPCClient::processRResponse(std::string &response) {
    servers.clear();
    int idx1 = response.find(' ');
    int idx2 = response.find('\n');
    while (idx1 != std::string::npos && idx2 != std::string::npos) {
        Server server(response.substr(0, idx1), std::stoi(response.substr(idx1 + 1, idx2 - idx1 - 1)));
        servers.push_back(server);
        response = response.substr(idx2 + 1);
        idx1 = response.find(' ');
        idx2 = response.find('\n');
    }
}

void RPCClient::processSResponse(std::string &response) {
    // std::cout << "This is Client" << std::endl; // debug
    std::cout << "Result: " << response << std::endl;
}

int RPCClient::balance() {
    int idx = -1;
    if (servers.size() == 0) {
        return idx;
    }
    idx = selection;
    selection = (selection + 1) % servers.size();
    return idx;
}