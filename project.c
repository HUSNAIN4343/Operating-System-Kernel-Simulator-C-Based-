#include <stdio.h>
#include <stdlib.h>
#define MAX_PROCESS 10
#define MEMORY_SIZE 100
#define TIME_QUANTUM 2

/* ---------------- PROCESS STATES ---------------- */
typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} State;

/* ---------------- PCB STRUCTURE ---------------- */
typedef struct {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;
    int memory;
    int waitingTime;
    int turnaroundTime;
    State state;
} PCB;

/* ---------------- GLOBAL VARIABLES ---------------- */
PCB process[MAX_PROCESS];
int processCount = 0;
int memory[MEMORY_SIZE];

/* ---------------- FUNCTION DECLARATIONS ---------------- */
void welcomeScreen();
void createProcess();
void displayProcessTable();
void fcfsScheduling();
void sjfScheduling();
void priorityScheduling();
void roundRobinScheduling();
void initializeMemory();
void allocateMemory(int pid, int size);
void displayMemoryMap();
void resetStates();

/* ---------------- MAIN FUNCTION ---------------- */
int main() {
    int choice;

    welcomeScreen(); // Welcome screen runs first
    initializeMemory();

    while (1) {
        printf("\n========== OS KERNEL SIMULATOR MENU ==========\n");
        printf("1. Create Process\n");
        printf("2. Display Process Table\n");
        printf("3. FCFS Scheduling\n");
        printf("4. SJF Scheduling\n");
        printf("5. Priority Scheduling\n");
        printf("6. Round Robin Scheduling\n");
        printf("7. Display Memory Map\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        resetStates();

        switch (choice) {
            case 1: createProcess(); break;
            case 2: displayProcessTable(); break;
            case 3: fcfsScheduling(); break;
            case 4: sjfScheduling(); break;
            case 5: priorityScheduling(); break;
            case 6: roundRobinScheduling(); break;
            case 7: displayMemoryMap(); break;
            case 0: exit(0);
            default: printf("Invalid option!\n");
        }
    }
}

/* ---------------- WELCOME SCREEN ---------------- */
void welcomeScreen() {
    printf("\n====================================================\n");
    printf(" OPERATING SYSTEM KERNEL SIMULATOR\n");
    printf(" Complex Engineering Activity (CEA)\n");
    printf("----------------------------------------------------\n");
    printf(" Project Title : Operating System Kernel Simulation\n");
    printf("====================================================\n");
    printf(" Press ENTER to continue...\n");
    getchar();
    getchar();
}

/* ---------------- RESET STATES ---------------- */
void resetStates() {
    for (int i = 0; i < processCount; i++) {
        if (process[i].state != TERMINATED)
            process[i].state = READY;
        process[i].remainingTime = process[i].burstTime;
    }
}

/* ---------------- PROCESS CREATION ---------------- */
void createProcess() {
    if (processCount >= MAX_PROCESS) {
        printf("Process limit reached!\n");
        return;
    }

    PCB *p = &process[processCount];
    p->pid = processCount + 1;

    printf("Arrival Time: ");
    scanf("%d", &p->arrivalTime);

    printf("Burst Time: ");
    scanf("%d", &p->burstTime);

    printf("Priority (lower value = higher priority): ");
    scanf("%d", &p->priority);

    printf("Memory Required: ");
    scanf("%d", &p->memory);

    p->remainingTime = p->burstTime;
    p->state = NEW;

    allocateMemory(p->pid, p->memory);
    p->state = READY;

    processCount++;
}

/* ---------------- PROCESS TABLE ---------------- */
void displayProcessTable() {
    printf("\nPID AT BT PR MEM STATE\n");
    for (int i = 0; i < processCount; i++) {
        printf("%2d %2d %2d %2d %3d ",
               process[i].pid,
               process[i].arrivalTime,
               process[i].burstTime,
               process[i].priority,
               process[i].memory);

        switch (process[i].state) {
            case NEW: printf("NEW"); break;
            case READY: printf("READY"); break;
            case RUNNING: printf("RUNNING"); break;
            case WAITING: printf("WAITING"); break;
            case TERMINATED: printf("TERMINATED"); break;
        }
        printf("\n");
    }
}

/* ---------------- FCFS SCHEDULING ---------------- */
void fcfsScheduling() {
    int time = 0, totalBurst = 0;
    float avgWT = 0, avgTAT = 0;

    printf("\nGantt Chart: | ");

    for (int i = 0; i < processCount; i++) {
        process[i].state = RUNNING;
        printf("P%d | ", process[i].pid);

        process[i].waitingTime = time - process[i].arrivalTime;
        if (process[i].waitingTime < 0) process[i].waitingTime = 0;

        time += process[i].burstTime;
        process[i].turnaroundTime = time - process[i].arrivalTime;
        totalBurst += process[i].burstTime;

        process[i].state = TERMINATED;

        avgWT += process[i].waitingTime;
        avgTAT += process[i].turnaroundTime;
    }

    printf("\nAverage Waiting Time = %.2f", avgWT / processCount);
    printf("\nAverage Turnaround Time = %.2f", avgTAT / processCount);
    printf("\nCPU Utilization = %.2f%%\n", ((float)totalBurst / time) * 100);
}

/* ---------------- SJF SCHEDULING ---------------- */
void sjfScheduling() {
    int completed = 0, time = 0, totalBurst = 0;
    int visited[MAX_PROCESS] = {0};
    float avgWT = 0, avgTAT = 0;

    printf("\nGantt Chart: | ");

    while (completed < processCount) {
        int idx = -1, minBT = 9999;

        for (int i = 0; i < processCount; i++) {
            if (!visited[i] && process[i].arrivalTime <= time &&
                process[i].burstTime < minBT) {
                minBT = process[i].burstTime;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        visited[idx] = 1;
        process[idx].state = RUNNING;
        printf("P%d | ", process[idx].pid);

        process[idx].waitingTime = time - process[idx].arrivalTime;
        time += process[idx].burstTime;
        process[idx].turnaroundTime = time - process[idx].arrivalTime;
        totalBurst += process[idx].burstTime;
        process[idx].state = TERMINATED;

        avgWT += process[idx].waitingTime;
        avgTAT += process[idx].turnaroundTime;
        completed++;
    }

    printf("\nAverage Waiting Time = %.2f", avgWT / processCount);
    printf("\nAverage Turnaround Time = %.2f", avgTAT / processCount);
    printf("\nCPU Utilization = %.2f%%\n", ((float)totalBurst / time) * 100);
}

/* ---------------- PRIORITY SCHEDULING ---------------- */
void priorityScheduling() {
    for (int i = 0; i < processCount - 1; i++) {
        for (int j = i + 1; j < processCount; j++) {
            if (process[i].priority > process[j].priority) {
                PCB temp = process[i];
                process[i] = process[j];
                process[j] = temp;
            }
        }
    }
    fcfsScheduling();
}

/* ---------------- ROUND ROBIN ---------------- */
void roundRobinScheduling() {
    int time = 0, completed = 0, totalBurst = 0;
    float avgWT = 0, avgTAT = 0;

    printf("\nGantt Chart: | ");

    while (completed < processCount) {
        for (int i = 0; i < processCount; i++) {
            if (process[i].remainingTime > 0) {
                process[i].state = RUNNING;
                printf("P%d | ", process[i].pid);

                if (process[i].remainingTime > TIME_QUANTUM) {
                    time += TIME_QUANTUM;
                    process[i].remainingTime -= TIME_QUANTUM;
                    process[i].state = WAITING;
                } else {
                    time += process[i].remainingTime;
                    process[i].remainingTime = 0;
                    process[i].turnaroundTime = time - process[i].arrivalTime;
                    process[i].waitingTime =
                        process[i].turnaroundTime - process[i].burstTime;
                    process[i].state = TERMINATED;
                    totalBurst += process[i].burstTime;
                    avgWT += process[i].waitingTime;
                    avgTAT += process[i].turnaroundTime;
                    completed++;
                }
            }
        }
    }

    printf("\nAverage Waiting Time = %.2f", avgWT / processCount);
    printf("\nAverage Turnaround Time = %.2f", avgTAT / processCount);
    printf("\nCPU Utilization = %.2f%%\n", ((float)totalBurst / time) * 100);
}

/* ---------------- MEMORY MANAGEMENT ---------------- */
void initializeMemory() {
    for (int i = 0; i < MEMORY_SIZE; i++)
        memory[i] = -1;
}

void allocateMemory(int pid, int size) {
    int count = 0;
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == -1)
            count++;
        else
            count = 0;

        if (count == size) {
            for (int j = i; j > i - size; j--)
                memory[j] = pid;
            return;
        }
    }
    printf("Memory allocation failed for Process %d\n", pid);
}

void displayMemoryMap() {
    printf("\nMemory Map:\n");
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i] == -1)
            printf("[ ]");
        else
            printf("[P%d]", memory[i]);

        if ((i + 1) % 10 == 0)
            printf("\n");
    }
}
