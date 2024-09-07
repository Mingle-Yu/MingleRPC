#include <string>

#define OK 0
#define WRONG_NUMBER_OF_PARAMETERS 1
#define INVALID_PARAMETERS 2
#define INVALID_IP_ADDR_OR_PORT 3
#define VALID_IPV4_AND_PORT 4
#define VALID_IPV6_AND_PORT 5

bool isValidIPv4AndPort(const std::string &ip, const int port);
bool isValidIPv6AndPort(const std::string &ip, const int port);
int checkIPAddressAndPort(const std::string &ip);
int checkArguments(int argc, char* argv[]);