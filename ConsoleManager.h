#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "Scheduler.h"
#include "Process.h"

class ConsoleManager {
public:
    static ConsoleManager* getInstance();

    void run(); // Starts the main menu CLI loop
    void initialize(); // Loads config and initializes the scheduler
    void startScheduler();
    void stopScheduler();
    void createProcess(const std::string& name, int instructionCount);
    void listScreens(); // screen -ls
    void screenAttach(const std::string& name); // screen -s <name>
    void screenReattach(const std::string& name); // screen -r <name>
    void generateReport(); // report-util
    int getCurrentPID() const;

private:
    ConsoleManager();
    static ConsoleManager* instance;
    bool isInitialized = false;
    int currentPID = 0;

    std::unordered_map<std::string, std::shared_ptr<Process>> processTable;
    std::vector<std::shared_ptr<Process>> allProcesses;
    std::unique_ptr<Scheduler> scheduler;

    void processScreen(std::shared_ptr<Process> process);
    void loadConfig();

    int numCPU = 1;
    std::string schedulerAlgo = "fcfs";
    int quantumCycles = 3;
    int batchProcessFreq = 1;
    int minInstructions = 5;
    int maxInstructions = 10;
    int delayPerExec = 0;
};
