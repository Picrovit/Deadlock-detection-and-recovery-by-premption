
#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

const int MAX_PROCESSES = 5;
const int MAX_RESOURCES = 5;

bool graph[MAX_PROCESSES][MAX_RESOURCES];
mutex resourceMutex[MAX_RESOURCES];

// Function declarations
void addEdge(int process, int resource);
bool hasEdge(int process, int resource);
void removeEdge(int process, int resource);
bool detectDeadlock();
void recoverFromDeadlock();
vector<int> findCycle();
int chooseVictim(vector<int> cycle);
void preemptResources(int victim, vector<int> cycle);
void updateGraph(int victim, vector<int> cycle);
int getPriority(int process);
void initializeGraph();
void displayGraph(HWND hwnd);
void simulateDeadlock();

// Function definitions
void addEdge(int process, int resource) {
    graph[process][resource] = true;
    graph[resource][process] = true;
}

bool hasEdge(int process, int resource) {
    return graph[process][resource];
}

void removeEdge(int process, int resource) {
    graph[process][resource] = false;
    graph[resource][process] = false;
}

bool detectDeadlock() {
    for (int process = 0; process < MAX_PROCESSES; process++) {
        for (int resource = 0; resource < MAX_RESOURCES; resource++) {
            if (hasEdge(process, resource) && hasEdge(resource, process)) {
                return true;
            }
        }
    }
    return false;
}

void recoverFromDeadlock() {
    while (detectDeadlock()) {
        // Find a cycle in the graph
        vector<int> cycle = findCycle();

        // Choose a victim process in the cycle
        int victim = chooseVictim(cycle);

        // Preempt resources from the victim process
        preemptResources(victim, cycle);

        // Update the graph
        updateGraph(victim, cycle);
    }
}

vector<int> findCycle() {
    vector<int> cycle;
    for (int process = 0; process < MAX_PROCESSES; process++) {
        for (int resource = 0; resource < MAX_RESOURCES; resource++) {
            if (hasEdge(process, resource) && hasEdge(resource, process)) {
                cycle.push_back(process);
                cycle.push_back(resource);
                break;
            }
        }
        if (!cycle.empty()) {
            break;
        }
    }
    return cycle;
}

int chooseVictim(vector<int> cycle) {
    // Choose the process with the lowest priority
    int victim = cycle[0];
    for (int i = 1; i < cycle.size(); i++) {
        if (getPriority(cycle[i]) < getPriority(victim)) {
            victim = cycle[i];
        }
    }
    return victim;
}

void preemptResources(int victim, vector<int> cycle) {
    for (int i = 0; i < cycle.size(); i++) {
        if (cycle[i] != victim) {
            removeEdge(victim, cycle[i]);
        }
    }
}

void updateGraph(int victim, vector<int> cycle) {
    for (int i = 0; i < cycle.size(); i++) {
        if (cycle[i] != victim) {
            graph[victim][cycle[i]] = false;
            graph[cycle[i]][victim] = false;
        }
    }
}

int getPriority(int process) {
    // For simplicity, assume the priority is the process number
    return process;
}

void initializeGraph() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            graph[i][j] = false;
        }
    }
}

void displayGraph(HWND hwnd) {
    string graphStr = "Graph State:\n";
    for (int i = 0; i < MAX_PROCESSES; i++) {
        for (int j = 0; j < MAX_RESOURCES; j++) {
            if (graph[i][j]) {
                graphStr += "P" + to_string(i) + " -> R" + to_string(j) + "\n";
            }
        }
    }
    MessageBox(hwnd, graphStr.c_str(), "Graph State", MB_OK);
}

void simulateDeadlock() {
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 4);
    addEdge(4, 0);
}

// GUI Code
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE:
            CreateWindow("button", "Add Edge P0->R1", WS_VISIBLE | WS_CHILD, 50, 50, 150, 30, hwnd, (HMENU) 1, NULL, NULL);
            CreateWindow("button", "Add Edge P1->R0", WS_VISIBLE | WS_CHILD, 50, 100, 150, 30, hwnd, (HMENU) 2, NULL, NULL);
            CreateWindow("button", "Add Edge P2->R2", WS_VISIBLE | WS_CHILD, 50, 150, 150, 30, hwnd, (HMENU) 3, NULL, NULL);
            CreateWindow("button", "Add Edge P3->R3", WS_VISIBLE | WS_CHILD, 50, 200, 150, 30, hwnd, (HMENU) 4, NULL, NULL);
            CreateWindow("button", "Add Edge P4->R4", WS_VISIBLE | WS_CHILD, 50, 250, 150, 30, hwnd, (HMENU) 5, NULL, NULL);
            CreateWindow("button", "Check Deadlock", WS_VISIBLE | WS_CHILD, 250, 50, 150, 30, hwnd, (HMENU) 6, NULL, NULL);
            CreateWindow("button", "Recover Deadlock", WS_VISIBLE | WS_CHILD, 250, 100, 150, 30, hwnd, (HMENU) 7, NULL, NULL);
            CreateWindow("button", "Display Graph", WS_VISIBLE | WS_CHILD, 250, 150, 150, 30, hwnd, (HMENU) 8, NULL, NULL);
            CreateWindow("button", "Simulate Deadlock", WS_VISIBLE | WS_CHILD, 250, 200, 150, 30, hwnd, (HMENU) 9, NULL, NULL);
            break;
        case WM_COMMAND:
            switch (LOWORD(wp)) {
                case 1:
                    addEdge(0, 1);
                    MessageBox(hwnd, "Edge P0 -> R1 added.", "Info", MB_OK);
                    break;
                case 2:
                    addEdge(1, 0);
                    MessageBox(hwnd, "Edge P1 -> R0 added.", "Info", MB_OK);
                    break;
                case 3:
                    addEdge(2, 2);
                    MessageBox(hwnd, "Edge P2 -> R2 added.", "Info", MB_OK);
                    break;
                case 4:
                    addEdge(3, 3);
                    MessageBox(hwnd, "Edge P3 -> R3 added.", "Info", MB_OK);
                    break;
                case 5:
                    addEdge(4, 4);
                    MessageBox(hwnd, "Edge P4 -> R4 added.", "Info", MB_OK);
                    break;
                case 6:
                    if (detectDeadlock()) {
                        MessageBox(hwnd, "Deadlock detected!", "Result", MB_OK);
                    } else {
                        MessageBox(hwnd, "No deadlock detected.", "Result", MB_OK);
                    }
                    break;
                case 7:
                    recoverFromDeadlock();
                    MessageBox(hwnd, "Recovery by preemption done. All processes resources are released.", "Result", MB_OK);
                    break;
                case 8:
                    displayGraph(hwnd);
                    break;
                case 9:
                    simulateDeadlock();
                    MessageBox(hwnd, "Simulated a deadlock scenario.", "Result", MB_OK);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = "DeadlockDetection";
    wc.lpfnWndProc = WindowProcedure;

    if (!RegisterClass(&wc)) return -1;

    CreateWindow("DeadlockDetection", "Deadlock Detection and Recovery", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 400, NULL, NULL, hInst, NULL);

    MSG msg = {0};

    initializeGraph();

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
