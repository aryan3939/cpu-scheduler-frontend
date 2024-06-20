#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <string>
#include <sstream>

using namespace std;

struct Process {
    int id, arrivalTime, burstTime, startTime, remainingTime, completionTime, waitingTime, turnaroundTime, responseTime, priority;
};

struct Output {
    string algorithmName;
    vector<Process> processes;
    double averageWaitingTime;
    double averageTurnaroundTime;
    double cpuUtilization;
    vector<pair<int, int>> ganttChart;
};

vector<Process> inputProcesses;
int quantumTime;

struct AllData {
    int totProcesses;
    int QuantumNo;
    vector<vector<int>> processData;
};

// Function to get data
AllData getData() {
    AllData data;
    string line;
    getline(cin, line);
    stringstream ss(line);
    ss >> data.totProcesses >> data.QuantumNo;
    for (int i = 0; i < data.totProcesses; i++) {
        vector<int> temp;
        getline(cin, line);
        stringstream ss(line);
        for (int j = 0; j < 3; j++) {
            int x;
            ss >> x;
            temp.push_back(x);
        }
        data.processData.push_back(temp);
    }
    return data;
}

// AllData getData() {
//     // Simulating data retrieval. Replace with actual implementation.
//     return {5, 12, {{1, 0, 10}, {2, 1, 5}, {3, 2, 8}, {4, 3, 6}, {5, 4, 4}}};
// }

void calculateMetrics(vector<Process>& processes, Output& output) {
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int totalIdleTime = 0;
    int currentTime = 0;
    
    for (auto& p : processes) {
        p.waitingTime = p.turnaroundTime - p.burstTime;
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
    }
    
    output.averageWaitingTime = (double)totalWaitingTime / processes.size();
    output.averageTurnaroundTime = (double)totalTurnaroundTime / processes.size();
    
    for (size_t i = 0; i < output.ganttChart.size(); ++i) {
        if (i == 0) totalIdleTime += output.ganttChart[i].first;
        else totalIdleTime += output.ganttChart[i].first - output.ganttChart[i-1].second;
    }
    
    output.cpuUtilization = ((currentTime - totalIdleTime) / (double)currentTime) * 100;
}

Output fcfs() {
    Output output;
    output.algorithmName = "FCFS";
    output.processes = inputProcesses;
    sort(output.processes.begin(), output.processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    for (auto& p : output.processes) {
        if (currentTime < p.arrivalTime) currentTime = p.arrivalTime;
        p.startTime = currentTime;
        p.completionTime = p.startTime + p.burstTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        currentTime += p.burstTime;
        output.ganttChart.push_back({p.startTime, p.completionTime});
    }

    calculateMetrics(output.processes, output);
    return output;
}

Output sjf() {
    Output output;
    output.algorithmName = "SJF";
    output.processes = inputProcesses;
    sort(output.processes.begin(), output.processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    vector<Process> readyQueue;
    vector<Process> completedProcesses;
    int n = output.processes.size();
    
    while (completedProcesses.size() < n) {
        for (auto& p : output.processes) {
            if (p.arrivalTime <= currentTime && p.remainingTime > 0) {
                readyQueue.push_back(p);
            }
        }
        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }
        sort(readyQueue.begin(), readyQueue.end(), [](const Process& a, const Process& b) {
            return a.burstTime < b.burstTime;
        });
        Process p = readyQueue.front();
        readyQueue.erase(readyQueue.begin());
        p.startTime = currentTime;
        p.completionTime = p.startTime + p.burstTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        currentTime += p.burstTime;
        p.remainingTime = 0;
        completedProcesses.push_back(p);
        output.ganttChart.push_back({p.startTime, p.completionTime});
    }
    output.processes = completedProcesses;
    calculateMetrics(output.processes, output);
    return output;
}

Output priorityScheduling() {
    Output output;
    output.algorithmName = "Priority Scheduling";
    output.processes = inputProcesses;

    // Sort processes by arrival time
    sort(output.processes.begin(), output.processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    vector<Process> readyQueue;
    vector<Process> completedProcesses;
    int n = output.processes.size();

    while (completedProcesses.size() < n) {
        // Add all processes that have arrived to the ready queue
        for (auto& p : output.processes) {
            if (p.arrivalTime <= currentTime && p.remainingTime > 0) {
                readyQueue.push_back(p);
            }
        }

        // If the ready queue is empty, move the time forward
        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        // Sort the ready queue by priority (higher priority comes first)
        sort(readyQueue.begin(), readyQueue.end(), [](const Process& a, const Process& b) {
            return a.priority < b.priority; // Assuming lower numerical value means higher priority
        });

        // Pick the process with the highest priority
        Process p = readyQueue.front();
        readyQueue.erase(readyQueue.begin());

        // Process the selected process
        p.startTime = currentTime;
        p.completionTime = p.startTime + p.burstTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        currentTime += p.burstTime;
        p.remainingTime = 0;
        completedProcesses.push_back(p);
        output.ganttChart.push_back({p.startTime, p.completionTime});
    }

    output.processes = completedProcesses;
    calculateMetrics(output.processes, output);
    return output;
}

Output rr() {
    Output output;
    output.algorithmName = "RR";
    output.processes = inputProcesses;

    // Sort processes by arrival time
    sort(output.processes.begin(), output.processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    queue<Process> readyQueue;
    vector<Process> completedProcesses;
    int n = output.processes.size();
    int processesLeft = n;
    vector<bool> inQueue(n, false);

    while (completedProcesses.size() < n) {
        // Add all processes that have arrived to the ready queue and are not yet in the queue
        for (int i = 0; i < n; ++i) {
            if (output.processes[i].arrivalTime <= currentTime && output.processes[i].remainingTime > 0 && !inQueue[i]) {
                readyQueue.push(output.processes[i]);
                inQueue[i] = true;
            }
        }

        // If the ready queue is empty, move the time forward
        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        // Pick the first process in the ready queue
        Process p = readyQueue.front();
        readyQueue.pop();

        // Calculate the execution time
        int execTime = min(quantumTime, p.remainingTime);
        p.startTime = (p.remainingTime == p.burstTime) ? currentTime : p.startTime; // Only set start time if the process starts now
        currentTime += execTime;
        p.remainingTime -= execTime;

        // Update the process details
        if (p.remainingTime > 0) {
            readyQueue.push(p);
        } else {
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            completedProcesses.push_back(p);
            inQueue[p.id - 1] = false; // Mark as completed and remove from inQueue
        }

        output.ganttChart.push_back({p.startTime, currentTime});
    }

    output.processes = completedProcesses;
    calculateMetrics(output.processes, output);
    return output;
}


void findBestAlgorithm(vector<Output>& outputs) {
    sort(outputs.begin(), outputs.end(), [](const Output& a, const Output& b) {
        return a.averageWaitingTime < b.averageWaitingTime;
    });
    cout << "Best Algorithm based on Average Waiting Time:\n";
    cout << "Algorithm: " << outputs[0].algorithmName << "\n";
    for (const auto& p : outputs[0].processes) {
        cout << "Process " << p.id << ": Waiting Time = " << p.waitingTime 
             << ", Turnaround Time = " << p.turnaroundTime << endl;
    }
    cout << "Average Waiting Time: " << outputs[0].averageWaitingTime << endl;
    cout << "Average Turnaround Time: " << outputs[0].averageTurnaroundTime << endl;
    cout << "CPU Utilization: " << outputs[0].cpuUtilization << "%\n";
    cout << "Gantt Chart:\n";
    for (const auto& g : outputs[0].ganttChart) {
        cout << "[" << g.first << " - " << g.second << "] ";
    }
    cout << endl;
}

int main() {
    // AllData data = getData();
    // quantumTime = data.QuantumNo;

    // for (const auto& p : data.processData) {
    //     inputProcesses.push_back({p[0], p[1], p[2], 0, p[2], 0, 0, 0, -1, -1});
    // }

    int n;
    cin >> n;
    inputProcesses.resize(n);
    for (int i = 0; i < n; ++i) {
        inputProcesses[i].id = i + 1;
        cin >> inputProcesses[i].arrivalTime >> inputProcesses[i].burstTime >> inputProcesses[i].priority;
    }
    cin >> quantumTime;

    vector<Output> outputs;
    outputs.push_back(fcfs());
    outputs.push_back(sjf());
    outputs.push_back(priorityScheduling());
    outputs.push_back(rr());

    for (const auto& output : outputs) {
        cout << "Algorithm: " << output.algorithmName << "\n";
        for (const auto& p : output.processes) {
            cout << "Process " << p.id << ": Waiting Time = " << p.waitingTime 
                 << ", Turnaround Time = " << p.turnaroundTime << endl;
        }
        cout << "Average Waiting Time: " << output.averageWaitingTime << endl;
        cout << "Average Turnaround Time: " << output.averageTurnaroundTime << endl;
        cout << "CPU Utilization: " << output.cpuUtilization << "%\n";
        cout << "Gantt Chart:\n";
        for (const auto& g : output.ganttChart) {
            cout << "[" << g.first << " - " << g.second << "] ";
        }
        cout << endl;
    }

    findBestAlgorithm(outputs);

    return 0;
}
