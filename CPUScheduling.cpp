#include <queue>
#include <vector>
#include <iostream>


struct Process {
    int pid;
    std::vector<int> cpuBursts;
    std::vector<int> ioTimes;
    int currentBurstIndex = 0;
    int arrivalTime = 0;
    int initialArrivalTime = 0; 
    int waitingTime = 0;
    int turnaroundTime = 0;
    int responseTime = -1;
    bool isCompleted = false;

    int remainingCpuTime = 0;
    int remainingIoTime = 0;

    int queueLevel = 1;      
    int timeSliceUsed = 0;   

    Process(int id, std::vector<int> cpu, std::vector<int> io)
        : pid(id), cpuBursts(cpu), ioTimes(io) {}
};



void schedulerFunction(std::vector<Process*>& processes) {
    int currentTime = 0;
    int totalCpuTime = 0;          // Total CPU time used
    int totalSimulationTime = 0;   // Total time from start to completion of all processes

    std::vector<Process*> readyQueue;
    std::vector<Process*> ioQueue;

    // Initialize all processes in the ready queue
    for (auto& process : processes) {
        process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
        readyQueue.push_back(process);
    }

    while (!readyQueue.empty() || !ioQueue.empty()) {
        // If ready queue is empty, advance time to the next I/O completion
        if (readyQueue.empty()) {
            int nextIoCompletionTime = INT_MAX;
            for (auto& process : ioQueue) {
                int completionTime = process->arrivalTime + process->remainingIoTime;
                if (completionTime < nextIoCompletionTime) {
                    nextIoCompletionTime = completionTime;
                }
            }
            currentTime = nextIoCompletionTime;

            // Move processes that have completed I/O back to the ready queue
            for (auto it = ioQueue.begin(); it != ioQueue.end();) {
                Process* process = *it;
                int completionTime = process->arrivalTime + process->remainingIoTime;
                if (completionTime <= currentTime) {
                    process->currentBurstIndex++;
                    if (process->currentBurstIndex < process->cpuBursts.size()) {
                        process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
                        process->arrivalTime = currentTime;
                        readyQueue.push_back(process);
                        it = ioQueue.erase(it);
                    }
                    else {
                        // Process has completed all CPU bursts
                        process->isCompleted = true;
                        process->turnaroundTime = currentTime - process->initialArrivalTime;
                        it = ioQueue.erase(it);
                    }
                }
                else {
                    ++it;
                }
            }

            continue; // Go back to the beginning of the loop
        }

        // Schedule the next process from the ready queue (FCFS)
        Process* currentProcess = readyQueue.front();
        readyQueue.erase(readyQueue.begin());

        // Record response time if not already set
        if (currentProcess->responseTime == -1) {
            currentProcess->responseTime = currentTime - currentProcess->initialArrivalTime;
        }

        // Update waiting time
        currentProcess->waitingTime += currentTime - currentProcess->arrivalTime;

        // Run the CPU burst
        std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
            << " is running CPU burst of " << currentProcess->remainingCpuTime << " units.\n";
        currentTime += currentProcess->remainingCpuTime;
        totalCpuTime += currentProcess->remainingCpuTime; // Accumulate CPU time

        // Check if there is a corresponding I/O burst
        if (currentProcess->currentBurstIndex < currentProcess->ioTimes.size()) {
            // Set remaining I/O time
            currentProcess->remainingIoTime = currentProcess->ioTimes[currentProcess->currentBurstIndex];
            currentProcess->arrivalTime = currentTime;
            ioQueue.push_back(currentProcess);

            std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                << " is performing I/O for " << currentProcess->remainingIoTime << " units.\n";
        }
        else {
            // Process has completed all CPU bursts
            currentProcess->isCompleted = true;
            currentProcess->turnaroundTime = currentTime - currentProcess->initialArrivalTime;

            std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                << " has completed execution.\n";
        }

        // Move processes that have completed I/O back to the ready queue
        for (auto it = ioQueue.begin(); it != ioQueue.end();) {
            Process* process = *it;
            int completionTime = process->arrivalTime + process->remainingIoTime;
            if (completionTime <= currentTime) {
                process->currentBurstIndex++;
                if (process->currentBurstIndex < process->cpuBursts.size()) {
                    process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
                    process->arrivalTime = completionTime;
                    readyQueue.push_back(process);
                    it = ioQueue.erase(it);
                }
                else {
                    // Process has completed all CPU bursts
                    process->isCompleted = true;
                    process->turnaroundTime = completionTime - process->initialArrivalTime;
                    it = ioQueue.erase(it);
                }
            }
            else {
                ++it;
            }
        }
    }

    totalSimulationTime = currentTime;

    // Calculate CPU utilization
    double cpuUtilization = (static_cast<double>(totalCpuTime) / totalSimulationTime) * 100.0;

    // Output CPU utilization
    std::cout << "\nCPU Utilization: " << cpuUtilization << "%\n";

}

void SJFschedulerFunction(std::vector<Process*>& processes) {
    int currentTime = 0;
    int totalCpuTime = 0;
    int totalSimulationTime = 0;

    std::vector<Process*> readyQueue;
    std::vector<Process*> ioQueue;

    // Initialize all processes in the ready queue
    for (auto& process : processes) {
        process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
        readyQueue.push_back(process);
    }

    while (!readyQueue.empty() || !ioQueue.empty()) {
        // Move processes that have completed I/O back to the ready queue
        for (auto it = ioQueue.begin(); it != ioQueue.end();) {
            Process* process = *it;
            int ioCompletionTime = process->arrivalTime + process->remainingIoTime;
            if (ioCompletionTime <= currentTime) {
                // I/O is complete
                process->currentBurstIndex++;
                if (process->currentBurstIndex < process->cpuBursts.size()) {
                    process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
                    process->arrivalTime = ioCompletionTime;
                    readyQueue.push_back(process);
                    it = ioQueue.erase(it);
                }
                else {
                    // Process has completed all CPU bursts
                    process->isCompleted = true;
                    process->turnaroundTime = ioCompletionTime - process->arrivalTime + process->waitingTime;
                    it = ioQueue.erase(it);
                }
            }
            else {
                ++it;
            }
        }

        // If ready queue is empty, advance time to the next I/O completion
        if (readyQueue.empty()) {
            // Find the earliest I/O completion time
            int nextIoCompletionTime = std::numeric_limits<int>::max();
            for (auto& process : ioQueue) {
                int completionTime = process->arrivalTime + process->remainingIoTime;
                if (completionTime < nextIoCompletionTime) {
                    nextIoCompletionTime = completionTime;
                }
            }
            // Advance currentTime
            currentTime = nextIoCompletionTime;
            continue;
        }

        // Select the process with the shortest next CPU burst
        auto shortestProcessIt = std::min_element(readyQueue.begin(), readyQueue.end(),
            [](Process* a, Process* b) {
                int burstA = a->remainingCpuTime;
                int burstB = b->remainingCpuTime;
                if (burstA != burstB) {
                    return burstA < burstB;
                }
                else {
                    // Tie-breaker: select process with lower PID
                    return a->pid < b->pid;
                }
            });
        Process* currentProcess = *shortestProcessIt;
        readyQueue.erase(shortestProcessIt);

        // Record response time if not already set
        if (currentProcess->responseTime == -1) {
            currentProcess->responseTime = currentTime - currentProcess->arrivalTime;
        }

        // Update waiting time
        currentProcess->waitingTime += currentTime - currentProcess->arrivalTime;

        // Run the CPU burst
        std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
            << " is running CPU burst of " << currentProcess->remainingCpuTime << " units.\n";
        currentTime += currentProcess->remainingCpuTime;
        totalCpuTime += currentProcess->remainingCpuTime;

        // Check if there is a corresponding I/O burst
        if (currentProcess->currentBurstIndex < currentProcess->ioTimes.size()) {
            // Set remaining I/O time
            currentProcess->remainingIoTime = currentProcess->ioTimes[currentProcess->currentBurstIndex];
            currentProcess->arrivalTime = currentTime;
            ioQueue.push_back(currentProcess);

            std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                << " is performing I/O for " << currentProcess->remainingIoTime << " units.\n";
        }
        else {
            // Process has completed all CPU bursts
            currentProcess->isCompleted = true;
            currentProcess->turnaroundTime = currentTime - currentProcess->arrivalTime + currentProcess->waitingTime;

            std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                << " has completed execution.\n";
        }
    }

    totalSimulationTime = currentTime;

    // Calculate CPU utilization
    double cpuUtilization = (static_cast<double>(totalCpuTime) / totalSimulationTime) * 100.0;

    // Output CPU utilization
    std::cout << "\nCPU Utilization: " << cpuUtilization << "%\n";
}

void mlfqScheduler(std::vector<Process*>& processes) {
    int currentTime = 0;
    int totalCpuTime = 0;
    int totalSimulationTime = 0;

    // Ready queues for each level
    std::queue<Process*> queue1; // Highest priority, Tq = 5
    std::queue<Process*> queue2; // Middle priority, Tq = 10
    std::queue<Process*> queue3; // Lowest priority, FCFS

    std::vector<Process*> ioQueue; // Processes performing I/O

    // Initialize processes
    for (auto& process : processes) {
        process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
        process->arrivalTime = 0; // All processes arrive at time 0
        process->queueLevel = 1;  // Start at highest priority queue
        queue1.push(process);
    }

    Process* currentProcess = nullptr;
    int timeQuantum = 0;
    int timeSlice = 0; // Time used in the current time quantum

    while (!queue1.empty() || !queue2.empty() || !queue3.empty() || !ioQueue.empty() || currentProcess != nullptr) {
        // Move processes that have completed I/O back to the appropriate queue
        for (auto it = ioQueue.begin(); it != ioQueue.end();) {
            Process* process = *it;
            int ioCompletionTime = process->arrivalTime + process->remainingIoTime;
            if (ioCompletionTime <= currentTime) {
                // I/O is complete
                process->currentBurstIndex++;
                if (process->currentBurstIndex < process->cpuBursts.size()) {
                    process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
                    process->arrivalTime = ioCompletionTime;
                    process->timeSliceUsed = 0; // Reset time slice used
                    // Place the process back into its current queue level
                    if (process->queueLevel == 1) {
                        queue1.push(process);
                    }
                    else if (process->queueLevel == 2) {
                        queue2.push(process);
                    }
                    else {
                        queue3.push(process);
                    }
                    it = ioQueue.erase(it);
                }
                else {
                    // Process has completed all CPU bursts
                    process->isCompleted = true;
                    process->turnaroundTime = ioCompletionTime - process->arrivalTime + process->waitingTime;
                    it = ioQueue.erase(it);
                }
            }
            else {
                ++it;
            }
        }

        // Check for preemption
        bool preempt = false;
        if (currentProcess != nullptr && currentProcess->queueLevel > 1) {
            // Check if a higher priority process is ready
            if (!queue1.empty()) {
                preempt = true;
            }
        }

        // If preemption occurs or no process is running, select the next process
        if (currentProcess == nullptr || preempt) {
            if (preempt) {
                // Preempt the current process and place it back into its queue
                std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                    << " is preempted and moved back to queue level " << currentProcess->queueLevel << ".\n";
                if (currentProcess->queueLevel == 2) {
                    queue2.push(currentProcess);
                }
                else if (currentProcess->queueLevel == 3) {
                    queue3.push(currentProcess);
                }
                currentProcess = nullptr;
            }

            // Select the next process from the highest priority non-empty queue
            if (!queue1.empty()) {
                currentProcess = queue1.front();
                queue1.pop();
                timeQuantum = 5;
            }
            else if (!queue2.empty()) {
                currentProcess = queue2.front();
                queue2.pop();
                timeQuantum = 10;
            }
            else if (!queue3.empty()) {
                currentProcess = queue3.front();
                queue3.pop();
                timeQuantum = currentProcess->remainingCpuTime; // For FCFS, run until completion
            }
            else {
                // No process is ready; advance time to next I/O completion
                int nextIoCompletionTime = INT_MAX;
                for (auto& process : ioQueue) {
                    int completionTime = process->arrivalTime + process->remainingIoTime;
                    if (completionTime < nextIoCompletionTime) {
                        nextIoCompletionTime = completionTime;
                    }
                }
                currentTime = nextIoCompletionTime;
                continue;
            }

            // Record response time if not already set
            if (currentProcess->responseTime == -1) {
                currentProcess->responseTime = currentTime - currentProcess->arrivalTime;
            }

            // Update waiting time
            currentProcess->waitingTime += currentTime - currentProcess->arrivalTime;
            currentProcess->timeSliceUsed = 0; // Reset time slice used
        }

        // Determine execution time for the current time slice
        int executionTime = std::min({ currentProcess->remainingCpuTime, timeQuantum - currentProcess->timeSliceUsed });

        // Simulate execution
        std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
            << " is running for " << executionTime << " units (Queue Level " << currentProcess->queueLevel << ").\n";

        currentTime += executionTime;
        totalCpuTime += executionTime;
        currentProcess->remainingCpuTime -= executionTime;
        currentProcess->timeSliceUsed += executionTime;

        // Move processes that have completed I/O back to the appropriate queue during execution
        for (auto it = ioQueue.begin(); it != ioQueue.end();) {
            Process* process = *it;
            int ioCompletionTime = process->arrivalTime + process->remainingIoTime;
            if (ioCompletionTime <= currentTime) {
                // I/O is complete
                process->currentBurstIndex++;
                if (process->currentBurstIndex < process->cpuBursts.size()) {
                    process->remainingCpuTime = process->cpuBursts[process->currentBurstIndex];
                    process->arrivalTime = ioCompletionTime;
                    process->timeSliceUsed = 0; // Reset time slice used
                    if (process->queueLevel == 1) {
                        queue1.push(process);
                    }
                    else if (process->queueLevel == 2) {
                        queue2.push(process);
                    }
                    else {
                        queue3.push(process);
                    }
                    it = ioQueue.erase(it);
                }
                else {
                    // Process has completed all CPU bursts
                    process->isCompleted = true;
                    process->turnaroundTime = ioCompletionTime - process->arrivalTime + process->waitingTime;
                    it = ioQueue.erase(it);
                }
            }
            else {
                ++it;
            }
        }

        // Check if the current process has completed its CPU burst
        if (currentProcess->remainingCpuTime == 0) {
            // CPU burst is complete
            std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                << " has completed its CPU burst.\n";
            if (currentProcess->currentBurstIndex < currentProcess->ioTimes.size()) {
                // Process has an I/O burst
                currentProcess->remainingIoTime = currentProcess->ioTimes[currentProcess->currentBurstIndex];
                currentProcess->arrivalTime = currentTime;
                ioQueue.push_back(currentProcess);

                std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                    << " is performing I/O for " << currentProcess->remainingIoTime << " units.\n";
            }
            else {
                // Process has completed all CPU bursts
                currentProcess->isCompleted = true;
                currentProcess->turnaroundTime = currentTime - currentProcess->initialArrivalTime;

                std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                    << " has completed execution.\n";
            }
            currentProcess = nullptr; // CPU is now idle
        }
        else if (currentProcess->timeSliceUsed == timeQuantum) {
            // Time quantum used up
            if (currentProcess->queueLevel < 3) {
                // Demote to next lower queue
                currentProcess->queueLevel++;
                std::cout << "[Time " << currentTime << "] Process P" << currentProcess->pid
                    << " is demoted to queue level " << currentProcess->queueLevel << ".\n";
            }
            // Reset time slice used
            currentProcess->timeSliceUsed = 0;
            currentProcess->arrivalTime = currentTime; // Update arrival time
            // Place the process back into the appropriate queue
            if (currentProcess->queueLevel == 2) {
                queue2.push(currentProcess);
            }
            else if (currentProcess->queueLevel == 3) {
                queue3.push(currentProcess);
            }
            currentProcess = nullptr; // CPU is now idle
        }
        // Else, continue running current process in the next iteration
    }

    totalSimulationTime = currentTime;

    // Calculate CPU utilization
    double cpuUtilization = (static_cast<double>(totalCpuTime) / totalSimulationTime) * 100.0;

    // Output CPU utilization
    std::cout << "\nCPU Utilization: " << cpuUtilization << "%\n";
}

int main() {
    // Create processes and initialize them with data
    Process p1(1, std::vector<int>{5, 3, 5, 4, 6, 4, 3, 4}, std::vector<int>{27, 31, 43, 18, 22, 26, 24});
    Process p2(2, std::vector<int>{4, 5, 7, 12, 9, 4, 9, 7, 8}, std::vector<int>{48, 44, 42, 37, 76, 41, 31, 43});
    Process p3(3, std::vector<int>{8, 12, 18, 14, 4, 15, 14, 5, 6}, std::vector<int>{33, 41, 65, 21, 61, 18, 26, 31});
    Process p4(4, std::vector<int>{3, 4, 5, 3, 4, 5, 6, 5, 3}, std::vector<int>{35, 41, 45, 51, 61, 54, 82, 77});
    Process p5(5, std::vector<int>{16, 17, 5, 16, 7, 13, 11, 6, 3, 4}, std::vector<int>{24, 21, 36, 26, 31, 28, 21, 13, 11});
    Process p6(6, std::vector<int>{11, 4, 5, 6, 7, 9, 12, 15, 8}, std::vector<int>{22, 8, 10, 12, 14, 18, 24, 30});
    Process p7(7, std::vector<int>{14, 17, 11, 15, 4, 7, 16, 10}, std::vector<int>{46, 41, 42, 21, 32, 19, 33});
    Process p8(8, std::vector<int>{4, 5, 6, 14, 16, 6}, std::vector<int>{14, 33, 51, 73, 87});
    
    std::vector<Process*> processes = {&p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8};

    int input = 0;
    std::cout << "Enter an input 1-3. 1.FCFS 2. SJF 3. MLFQ." << std::endl;
    std::cin >> input;
    
    //Function to make the user choose which
    switch (input) {
        case 3:
            mlfqScheduler(processes);
            std::cout << "MLFQ Algorithim" << std::endl;
            break;
        case 2:
            SJFschedulerFunction(processes);
            std::cout << "SJF Algorithimn" << std::endl;
            break;
        case 1:
            schedulerFunction(processes);
            std::cout << "FCFS Algorithimn" << std::endl;
            break;
        default:
            std::cout << "Incorrect input." << std::endl;
            exit(1);
    }


    // Output results
    for (const auto& process : processes) {
        std::cout << "Process P" << process->pid << ":\n";
        std::cout << "  Waiting Time (Tw): " << process->waitingTime << "\n";
        std::cout << "  Turnaround Time (Ttr): " << process->turnaroundTime << "\n";
        std::cout << "  Response Time (Tr): " << process->responseTime << "\n";
    }


    return 0;
} 