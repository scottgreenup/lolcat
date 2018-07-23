

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

class UnknownArgumentException : public std::exception {
public:
   UnknownArgumentException(std::string argument) {
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

namespace flag {

enum FlagSetElementType {
    Boolean,
    Double,
    String,
};

struct FlagSetElement {
    void *Value;

    // TODO use the default value
    void *DefaultValue;

    std::string Usage;
    FlagSetElementType Type;
    bool HasArg;
};

// Each 'type' has a way to cleanup, a way to create one, and a case block in
// SetArg
class FlagSet {
public:
    FlagSet() {}

    ~FlagSet() {
        for (auto [key, val] : this->m_arguments) {
            switch (val.Type) {
            case FlagSetElementType::String:
                delete static_cast<std::string*>(val.Value);
                delete static_cast<std::string*>(val.DefaultValue);
                break;
            case FlagSetElementType::Double:
                delete static_cast<double*>(val.Value);
                delete static_cast<double*>(val.DefaultValue);
                break;
            case FlagSetElementType::Boolean:
                delete static_cast<bool*>(val.Value);
                delete static_cast<bool*>(val.DefaultValue);
                break;
            default:
                break;
            }
        }
    }

    std::string * String(std::string name, std::string value = "", std::string usage = "") {
        auto fv = this->Argument<std::string>(name, value, usage, FlagSetElementType::String);
        this->m_arguments[name].HasArg = true;
        return fv;
    }

    double * Double(std::string name, double value = 0.0, std::string usage = "") {
        auto fv = this->Argument<double>(name, value, usage, FlagSetElementType::Double);
        this->m_arguments[name].HasArg = true;
        return fv;
    }

    bool * Boolean(std::string name, bool value = false, std::string usage = "") {
        auto fv = this->Argument<bool>(name, value, usage, FlagSetElementType::Boolean);
        this->m_arguments[name].HasArg = false;
        return fv;
    }

    template<typename T>
    T* Argument(std::string name, T value, std::string usage, FlagSetElementType type) {
        FlagSetElement element;
        T * flagValue = new T;
        T * defaultValue = new T(value);
        element.Value = static_cast<void*>(flagValue);
        element.DefaultValue = static_cast<void*>(defaultValue);
        element.Type = type;
        element.Usage = usage;
        this->m_arguments[name] = element;
        return flagValue;
    }

    std::vector<std::string> Convert(std::size_t argc, char** argv) {
        std::vector<std::string> arguments(argc - 1);
        for (std::size_t i = 0; i < argc - 1; i++) {
            arguments[i] = std::string(argv[i+1]);
        }
        return arguments;
    }

    void Parse(std::vector<std::string> arguments) {
        for (std::size_t i = 0; i < arguments.size(); i++) {
            std::string arg = arguments[i];
            if (arg[0] == '-' && arg[1] == '-') {
                std::string name = arg.substr(2);

                if (this->m_arguments.count(name) == 0) {
                    std::cout << "name has no pre: " << name << std::endl;
                    throw UnknownArgumentException(arg);
                }

                std::cout << "name: " << name << std::endl;

                if (this->m_arguments[name].HasArg) {
                    this->SetArg(name, arguments[i+1]);
                    i++;
                } else {
                    this->RunArg(name);
                }

            } else {
                throw UnknownArgumentException(arg);
            }
        }
    }

private:
    void RunArg(std::string name) {
        switch (this->m_arguments[name].Type) {
        case FlagSetElementType::Boolean:
            {
                bool * target = static_cast<bool*>(
                        this->m_arguments[name].Value);
                *target = true;
            }
            break;
        default:
            // TODO change this to a better exception
            throw std::logic_error("Unknown FlagSetElementType");
        }
    }

    void SetArg(std::string name, std::string value) {
        switch (this->m_arguments[name].Type) {
        case FlagSetElementType::String:
            {
                std::string * target = static_cast<std::string*>(
                        this->m_arguments[name].Value);
                *target = value;
            }
            break;
        case FlagSetElementType::Double:
            {
                double * target = static_cast<double*>(
                        this->m_arguments[name].Value);
                *target = std::stod(value);
            }
            break;
        default:
            // TODO change this to a better exception
            throw std::logic_error("Unknown FlagSetElementType");
        }
    }

private:
    std::map<std::string, FlagSetElement> m_arguments;
};

}

void ParseArgs(int argc, char **argv) {

    // --help,  -h: Show the help message
    // --force, -f: Force color even when stdout is not a tty
    // We do not support --this="syntax"
    //
    auto fs = flag::FlagSet();

    auto horizontal = fs.Double("horizontal");
    auto vertical = fs.Double("vertical");
    auto force = fs.Boolean("force");

    auto arguments = fs.Convert(argc, argv);
    fs.Parse(arguments);

    std::cout << "horizontal == " << *horizontal << std::endl;
    std::cout << "vertical == " << *vertical << std::endl;
    std::cout << "force == " << *force << std::endl;
}

int main(int argc, char **argv) {
    try {
        ParseArgs(argc, argv);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
