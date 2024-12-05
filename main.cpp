#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

// Function to display the current resource allocation state
void displayState(const vector<vector<int>>& MAX, const vector<vector<int>>& ALLOC,
                  const vector<vector<int>>& NEED, const vector<int>& AVAIL) {
    cout << "Current Resource State:\n";
    cout << "==================================================\n";

    cout << "MAX:\n";
    for (const auto& row : MAX) {
        for (int value : row) cout << value << " ";
        cout << "\n";
    }

    cout << "\nALLOCATED:\n";
    for (const auto& row : ALLOC) {
        for (int value : row) cout << value << " ";
        cout << "\n";
    }

    cout << "\nNEED:\n";
    for (const auto& row : NEED) {
        for (int value : row) cout << value << " ";
        cout << "\n";
    }

    cout << "\nAVAILABLE:\n";
    for (int value : AVAIL) cout << value << " ";
    cout << "\n==================================================\n";
}

// Function to check if the system is in a safe state
bool isSafe(const vector<vector<int>>& ALLOC, const vector<vector<int>>& NEED, const vector<int>& AVAIL) {
    vector<int> work = AVAIL; // Simulate available resources
    vector<bool> done(ALLOC.size(), false); // Track finished processes
    int completed = 0;

    while (completed < ALLOC.size()) {
        bool progress = false;

        for (int i = 0; i < ALLOC.size(); ++i) {
            if (!done[i]) {
                bool satisfy = true;

                for (int j = 0; j < work.size(); ++j) {
                    if (NEED[i][j] > work[j]) {
                        satisfy = false;
                        break;
                    }
                }

                if (satisfy) {
                    for (int j = 0; j < work.size(); ++j) {
                        work[j] += ALLOC[i][j];
                    }
                    done[i] = true;
                    progress = true;
                    completed++;
                }
            }
        }

        if (!progress) return false; // Deadlock
    }

    return true; // All finish
}

// handle resource request
bool processRequest(int proc, const vector<int>& req, vector<vector<int>>& ALLOC,
                    vector<vector<int>>& NEED, vector<int>& AVAIL) {
    for (int i = 0; i < req.size(); ++i) {
        if (req[i] > NEED[proc][i]) {
            cout << "Request exceeds need. Request denied.\n";
            return false;
        }
        if (req[i] > AVAIL[i]) {
            cout << "Request exceeds available resources. Request denied.\n";
            return false;
        }
    }

    // Pretend to allocate resources
    for (int i = 0; i < req.size(); ++i) {
        AVAIL[i] -= req[i];
        ALLOC[proc][i] += req[i];
        NEED[proc][i] -= req[i];
    }

    // Check for safety
    if (isSafe(ALLOC, NEED, AVAIL)) {
        cout << "Request approved.\n";
        return true;
    } else {
        // Rollback
        for (int i = 0; i < req.size(); ++i) {
            AVAIL[i] += req[i];
            ALLOC[proc][i] -= req[i];
            NEED[proc][i] += req[i];
        }
        cout << "Request leads to unsafe state. Request denied.\n";
        return false;
    }
}

int main() {
    int numProcesses, numResources;
    vector<vector<int>> MAX, ALLOC, NEED;
    vector<int> AVAIL;

    ifstream inputFile("file.txt");
    if (!inputFile) {
        cerr << "Error: File not found.\n";
        return 1;
    }

    inputFile >> numProcesses >> numResources;
    MAX.resize(numProcesses, vector<int>(numResources));
    ALLOC.resize(numProcesses, vector<int>(numResources, 0));
    NEED.resize(numProcesses, vector<int>(numResources));
    AVAIL.resize(numResources);

    for (int i = 0; i < numProcesses; ++i)
        for (int j = 0; j < numResources; ++j)
            inputFile >> MAX[i][j];

    for (int i = 0; i < numResources; ++i)
        inputFile >> AVAIL[i];

    inputFile.close();

    for (int i = 0; i < numProcesses; ++i)
        for (int j = 0; j < numResources; ++j)
            NEED[i][j] = MAX[i][j] - ALLOC[i][j];

    while (true) {
        displayState(MAX, ALLOC, NEED, AVAIL);

        cout << "\nEnter process number (-1 to exit): ";
        int process;
        cin >> process;

        if (process == -1) break;

        if (process < 0 || process >= numProcesses) {
            cout << "Invalueid process number. Try again.\n";
            continue;
        }

        vector<int> request(numResources);
        cout << "Enter request vector:\n";
        for (int i = 0; i < numResources; ++i) cin >> request[i];

        processRequest(process, request, ALLOC, NEED, AVAIL);
    }

    cout << "Exiting program. Final state:\n";
    displayState(MAX, ALLOC, NEED, AVAIL);

    return 0;
}
