#include "ConsoleManager.h"
#include "Instruction.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <random>

ConsoleManager* ConsoleManager::instance = nullptr;

ConsoleManager* ConsoleManager::getInstance() {
    if (!instance) {
        instance = new ConsoleManager();
    }
    return instance;
}

ConsoleManager::ConsoleManager() {}

void ConsoleManager::run() {
    std::string input;
    std::cout << "Welcome to the OS Emulator Shell\n";

    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);

        if (input == "exit") break;

        if (!isInitialized && input != "initialize") {
            std::cout << "Please initialize the system first using `initialize` command.\n";
            continue;
        }

        if (input == "initialize") {
            initialize();
        } else if (input == "screen -ls") {
            listScreens();
        } else if (input.rfind("screen -s ", 0) == 0) {
            std::string name = input.substr(10);
            screenAttach(name);
        } else if (input.rfind("screen -r ", 0) == 0) {
            std::string name = input.substr(10);
            screenReattach(name);
        } else if (input == "scheduler-start") {
            startScheduler();
        } else if (input == "scheduler-stop") {
            stopScheduler();
        } else if (input == "report-util") {
            generateReport();
        } else {
            std::cout << "Unknown command.\n";
        }
    }
}

void ConsoleManager::initialize() {
    loadConfig();
    isInitialized = true;
    std::cout << "System initialized successfully.\n";
}

void ConsoleManager::loadConfig() {
    std::ifstream file("config.txt");
    if (!file.is_open()) {
        std::cout << "Failed to open config.txt. Using defaults.\n";
        return;
    }

    std::string key;
    while (file >> key) {
        if (key == "num-cpu") file >> numCPU;
        else if (key == "scheduler") file >> schedulerAlgo;
        else if (key == "quantum-cycles") file >> quantumCycles;
        else if (key == "batch-process-freq") file >> batchProcessFreq;
        else if (key == "min-ins") file >> minInstructions;
        else if (key == "max-ins") file >> maxInstructions;
        else if (key == "delay-per-exec") file >> delayPerExec;
    }

    std::cout << "Config loaded: " << numCPU << " CPUs, Scheduler = " << schedulerAlgo
              << ", Quantum = " << quantumCycles << ", Min/Max Instructions = "
              << minInstructions << "/" << maxInstructions << ", Delay = " << delayPerExec << "\n";
}

void ConsoleManager::startScheduler() {
    std::cout << "Starting process generation...\n";

    // For simulation: generate 5 dummy processes
    for (int i = 0; i < 5; i++) {
        std::string procName = "p" + std::to_string(currentPID + 1);

        int instCount = minInstructions + (rand() % (maxInstructions - minInstructions + 1));
        createProcess(procName, instCount);
    }
}

void ConsoleManager::stopScheduler() {
    std::cout << "Process generation stopped.\n";
    // No thread for now, so no-op
}

void ConsoleManager::createProcess(const std::string& name, int instructionCount) {
    auto proc = std::make_shared<Process>(++currentPID, name, instructionCount);
    processTable[name] = proc;
    allProcesses.push_back(proc);
    std::cout << "Process " << name << " created with " << instructionCount << " instructions.\n";
}

void ConsoleManager::listScreens() {
    std::cout << "Currently running processes:\n";
    for (auto& pair : processTable) {
        if (!pair.second->isFinished()) {
            std::cout << "  " << pair.first << "\n";
        }
    }
}

void ConsoleManager::screenAttach(const std::string& name) {
    if (processTable.count(name) == 0) {
        std::cout << "Process " << name << " not found.\n";
        return;
    }
    processScreen(processTable[name]);
}

void ConsoleManager::screenReattach(const std::string& name) {
    screenAttach(name);
}

void ConsoleManager::processScreen(std::shared_ptr<Process> process) {
    std::string input;
    while (true) {
        std::cout << "[screen:" << process->getName() << "] > ";
        std::getline(std::cin, input);

        if (input == "exit") break;
        else if (input == "process-smi") {
            std::cout << "Name: " << process->getName() << "\n";
            std::cout << "PID: " << process->getPID() << "\n";
            std::cout << "Progress: " << process->getCommandCounter() << " / " << process->getLinesOfCode() << "\n";
            std::cout << "Core ID: " << process->getCoreID() << "\n";
            std::cout << "Logs: " << process->getOutput() << "\n";
            if (process->isFinished()) std::cout << "Finished!\n";
        } else {
            std::cout << "Unknown screen command.\n";
        }
    }
}

void ConsoleManager::generateReport() {
    std::ofstream outFile("csopesy-log.txt");
    if (!outFile.is_open()) {
        std::cout << "Failed to open report file.\n";
        return;
    }

    outFile << "=== CPU Utilization Report ===\n";
    for (auto& proc : allProcesses) {
        outFile << "Process: " << proc->getName()
                << " PID: " << proc->getPID()
                << " Progress: " << proc->getCommandCounter() << " / " << proc->getLinesOfCode() << "\n";
    }
    outFile.close();
    std::cout << "Report saved to csopesy-log.txt.\n";
}

int ConsoleManager::getCurrentPID() const {
    return currentPID;
}
