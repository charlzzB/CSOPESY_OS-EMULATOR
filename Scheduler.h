#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <string>
#include <unordered_map>
#include "Process.h"

class Scheduler {
public:
    Scheduler(int numCores, const std::string& algorithm, int quantum = 1);

    void addProcess(std::shared_ptr<Process> process);
    void tick(); // Simulates one CPU cycle
    void stop(); // Stops the scheduler loop
    void resume(); // Resumes the scheduler loop

    std::string getAlgorithm() const;
    int getAvailableCores() const;

private:
    int numCores;
    std::string schedulingAlgorithm; // "fcfs" or "rr"
    int quantum;

    struct Core {
        std::shared_ptr<Process> currentProcess = nullptr;
        int remainingQuantum = 0;
    };

    std::vector<Core> cores;
    std::queue<std::shared_ptr<Process>> readyQueue;

    bool isRunning;

    void assignProcessesToCores();
    void executeProcesses();
};
