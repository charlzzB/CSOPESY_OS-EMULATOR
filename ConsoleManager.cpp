#include "ConsoleManager.h"
#include "Instruction.h"
#include "InstructionsTypes.h"
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
        std::cout << "\nRoot:\\> ";
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

    scheduler = std::make_unique<Scheduler>(numCPU, schedulerAlgo, quantumCycles);
    // For simulation
    for (int i = 0; i < numCPU; ++i) {
        std::string procName = "p" + std::to_string(currentPID + 1);

        int instCount = minInstructions + (rand() % (maxInstructions - minInstructions + 1));
        createProcess(procName, instCount);
        scheduler->addProcess(allProcesses.back());
    }

    // create scheduler
    scheduler = std::make_unique<Scheduler>(numCPU, schedulerAlgo, quantumCycles);
    for(const auto& proc : allProcesses){
        scheduler->addProcess(proc);
    }

    //ticking thread
    ticking = true;
    schedulerThread = std::thread([this]() {
        while (ticking) {
        //    std::cout << "[TICK]\n";
            scheduler->tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec));
        }
    });
    std::cout<<"Scheduler started\n"; 

}

void ConsoleManager::stopScheduler() {
    if (!ticking) {
        std::cout << "Scheduler is not running.\n";
        return;
    }

    std::cout << "Stopping scheduler...\n";
    ticking = false;

    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }

    std::cout << "Scheduler stopped.\n";
}

void ConsoleManager::createProcess(const std::string& name, int instructionCount) {
   auto proc = std::make_shared<Process>(++currentPID, name, instructionCount);

    // Generate dummy instructions
    std::vector<std::shared_ptr<Instruction>> insts;
    for (int i = 0; i < instructionCount; ++i) {
        if (i % 4 == 0)
            insts.push_back(std::make_shared<DeclareInstruction>("x", i));
        else if (i % 4 == 1)
            insts.push_back(std::make_shared<AddInstruction>("x", "x", "1"));
        else if (i % 4 == 2)
            insts.push_back(std::make_shared<SubtractInstruction>("x", "x", "1"));
        else
            insts.push_back(std::make_shared<PrintInstruction>("Instruction executed."));
    }

    proc->setInstructions(insts);

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

// screen -s make process 
void ConsoleManager::screenAttach(const std::string& name) {
    // If process does not exist, create it
    if (processTable.count(name) == 0) {
        int instructionCount = minInstructions + (rand() % (maxInstructions - minInstructions + 1));
        createProcess(name, instructionCount);

        if (scheduler) {
            scheduler->addProcess(processTable[name]);
        } else {
            std::cout << "Scheduler not started yet. Process will be idle until scheduler starts.\n";
        }
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
            if (process->isFinished()) {
                std::cout << "Finished at: " << process->getFinishTimeString() << "\n";
            }
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
                << " Progress: " << proc->getCommandCounter() << " / " << proc->getLinesOfCode();
        if(proc->isFinished()){
            outFile << " [" << proc->getFinishTimeString() << "]";
        }
        outFile <<"\n";
            }
    outFile.close();
    std::cout << "Report saved to csopesy-log.txt.\n";
}

int ConsoleManager::getCurrentPID() const {
    return currentPID;
}
