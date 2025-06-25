#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include <thread>
#include <sstream>

class Instruction {
public:
    virtual void execute(
        const std::string& processName,
        int coreID,
        std::unordered_map<std::string, uint16_t>& variables,
        std::string& outputLog,
        bool& sleeping,
        int& sleepTicks
    ) = 0;

    virtual ~Instruction() = default;
};

// ally aded this 
class PrintInstruction : public Instruction {
    std::string message;

public:
    PrintInstruction(const std::string& msg) : message(msg) {}

    void execute(
        const std::string& processName,
        int coreID,
        std::unordered_map<std::string, uint16_t>& variables,
        std::string& outputLog,
        bool& sleeping,
        int& sleepTicks
    ) override {
        std::ostringstream oss;
        oss << "Core " << coreID << " | " << processName << ": " << message << "\n";
        outputLog += oss.str();
    }
};
// ally aded this 
