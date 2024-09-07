#include <string>
#include <stdexcept>
#include <cmath>
#include <algorithm>

class Add {
private:
    std::string arguments;

public:
    Add(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx == std::string::npos) {
            return "Invalid arguments";
        }

        int op1, op2;
        try {
            op1 = std::stoi(arguments.substr(0, idx));
            arguments = arguments.substr(idx);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        try {
            op2 = std::stoi(arguments.substr(0));
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        return std::to_string(op1 + op2);
    }
};

class Sub {
private:
    std::string arguments;

public:
    Sub(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx == std::string::npos) {
            return "Invalid arguments";
        }

        int op1, op2;
        try {
            op1 = std::stoi(arguments.substr(0, idx));
            arguments = arguments.substr(idx);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        try {
            op2 = std::stoi(arguments.substr(0));
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        return std::to_string(op1 - op2);
    }
};

class Mul {
private:
    std::string arguments;

public:
    Mul(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx == std::string::npos) {
            return "Invalid arguments";
        }

        int op1, op2;
        try {
            op1 = std::stoi(arguments.substr(0, idx));
            arguments = arguments.substr(idx);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        try {
            op2 = std::stoi(arguments.substr(0));
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        return std::to_string(op1 * op2);
    }
};

class Div {
private:
    std::string arguments;

public:
    Div(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx == std::string::npos) {
            return "Invalid arguments";
        }

        int op1, op2;
        try {
            op1 = std::stoi(arguments.substr(0, idx));
            arguments = arguments.substr(idx);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        try {
            op2 = std::stoi(arguments.substr(0));
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        if (op2 == 0) {
            return "Invalid arguments"; 
        }

        return std::to_string(op1 / op2);
    }
};

class Sqr {
private:
    std::string arguments;

public:
    Sqr(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx != std::string::npos) {
            return "Invalid arguments";
        }

        int op;
        try {
            op = std::stoi(arguments);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        return std::to_string(op * op);
    }
};

class Sqrt {
private:
    std::string arguments;

public:
    Sqrt(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx != std::string::npos) {
            return "Invalid arguments";
        }

        int op;
        try {
            op = std::stoi(arguments);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        if (op < 0) {
            return "Invalid arguments"; 
        }

        return std::to_string(sqrt(op));
    }
};

class Cube {
private:
    std::string arguments;

public:
    Cube(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx != std::string::npos) {
            return "Invalid arguments";
        }

        int op;
        try {
            op = std::stoi(arguments);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        return std::to_string(op * op * op);
    }
};

class Echo {
private:
    std::string arguments;

public:
    Echo(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        return arguments;
    }
};

class Reverse {
private:
    std::string arguments;

public:
    Reverse(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        std::reverse(arguments.begin(), arguments.end());
        return arguments;
    }
};

class Fact {
private:
    std::string arguments;

public:
    Fact(std::string arguments) {
        this->arguments = arguments;
    }

    std::string operator()() {
        int idx = arguments.find(' ');
        if (idx != std::string::npos) {
            return "Invalid arguments";
        }

        int op;
        try {
            op = std::stoi(arguments);
        } catch(const std::exception& e) {
            return "Invalid arguments";
        }

        if (op < 0) {
            return "Invalid arguments";
        }

        int result = 1;
        for (int i = 1; i <= op; ++i) {
            result *= i;
        }

        return std::to_string(result);
    }
};