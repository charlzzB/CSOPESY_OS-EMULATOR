#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include "Instruction.h"

class Process {
public:
    enum ProcessState {
        READY, RUNNING, WAITING, FINISHED
    };

    Process(int pid, const std::string& name, int lines);
    void executeNextInstruction(int coreID);
    bool isFinished() const;

    std::string getName() const;
    int getPID() const;
    int getCommandCounter() const;
    int getLinesOfCode() const;
    int getCoreID() const;
    ProcessState getState() const;
    std::string getOutput() const;

    void setCoreID(int coreID);
    void setState(ProcessState newState);
    void setInstructions(const std::vector<std::shared_ptr<Instruction>>& insts);

private:
    int pid;
    std::string name;
    int commandCounter;
    int linesOfCode;
    int coreID;
    ProcessState currentState;

    std::vector<std::shared_ptr<Instruction>> instructions;
    std::unordered_map<std::string, uint16_t> variables;
    std::string outputLog;

    bool sleeping = false;
    int sleepTicks = 0;
};
