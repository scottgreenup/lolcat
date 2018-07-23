

#include <exception>
#include <iostream>
#include <sstream>

class ArgumentException : public std::exception {
public:
   ArgumentException(std::string argument) {
        std::stringstream ss;
        ss << "unknown argument: " << argument;
        this->m_message = ss.str();
   }

    const char * what () const throw () {
        return this->m_message.c_str();
    }

private:
    std::string m_message;
};

void ParseArgs(int argc, char **argv) {

    // --help,  -h: Show the help message
    // --force, -f: Force color even when stdout is not a tty
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];

        if (arg[0] == '-') {
            if (arg[1] == '-') {
                // TODO long-argument
            } else {
                if (arg.length() != 2) {
                    throw ArgumentException(arg);
                }
            }
        }

    }
}

int main(int argc, char **argv) {
    try {
        ParseArgs(argc, argv);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
