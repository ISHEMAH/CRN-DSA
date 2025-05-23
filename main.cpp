#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <fstream>
#include <algorithm>
using namespace std;

const int MAX_CITIES = 100;

struct City {
    int index;
    string name;
};

vector<City> cities;
map<string, int> cityIndexMap;
int roadMatrix[MAX_CITIES][MAX_CITIES] = {0};
double budgetMatrix[MAX_CITIES][MAX_CITIES] = {0.0};
int cityCount = 0;

void addCities() {
    int n;
    cout << "Enter the number of cities to add: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; ++i) {
        string name;
        cout << "Enter name for the city " << (i + 1) << ": ";
        getline(cin, name);


        string normalized = name;
        transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);

        if (cityIndexMap.count(normalized)) {
            cout << "Error: City \"" << name << "\" already exists.\n";
            continue; // Skip this city
        }

        cityCount++;
        cities.push_back({cityCount, name});
        cityIndexMap[normalized] = cityCount;
    }
}


void addRoad() {
    string city1, city2;
    cin.ignore();
    cout << "Enter the name of the first city: ";
    getline(cin, city1);
    cout << "Enter the name of the second city: ";
    getline(cin, city2);

    if (cityIndexMap.count(city1) && cityIndexMap.count(city2)) {
        int i = cityIndexMap[city1];
        int j = cityIndexMap[city2];
        roadMatrix[i][j] = roadMatrix[j][i] = 1;
        cout << "Road added between " << city1 << " and " << city2 << "\n";
    } else {
        cout << "One or both cities not found.\n";
    }
}

void addBudget() {
    string city1, city2;
    double budget;
    cin.ignore();
    cout << "Enter the name of the first city: ";
    getline(cin, city1);
    cout << "Enter the name of the second city: ";
    getline(cin, city2);
    cout << "Enter the budget for the road (in Billions RWF): ";
    cin >> budget;

    if (cityIndexMap.count(city1) && cityIndexMap.count(city2)) {
        int i = cityIndexMap[city1];
        int j = cityIndexMap[city2];
        if (roadMatrix[i][j] == 1) {
            budgetMatrix[i][j] = budgetMatrix[j][i] = budget * 1e9;
            cout << "Budget assigned to road between " << city1 << " and " << city2 << "\n";
        } else {
            cout << "No road exists between the cities. Add a road first.\n";
        }
    } else {
        cout << "One or both cities not found.\n";
    }
}

void editCity() {
    int index;
    string newName;
    cout << "Enter the index of the city to edit: ";
    cin >> index;
    cin.ignore();
    if (index >= 1 && index <= cityCount) {
        cout << "Enter new name for city with index " << index << ": ";
        getline(cin, newName);
        cityIndexMap.erase(cities[index - 1].name);
        cities[index - 1].name = newName;
        cityIndexMap[newName] = index;
        cout << "City updated.\n";
    } else {
        cout << "Invalid city index.\n";
    }
}

void searchCity() {
    cout << "Search by (1) Index or (2) Name: ";
    int choice;
    cin >> choice;
    cin.ignore();
    if (choice == 1) {
        int index;
        cout << "Enter index: ";
        cin >> index;
        if (index >= 1 && index <= cityCount) {
            cout << "City: " << cities[index - 1].name << "\n";
        } else {
            cout << "City not found.\n";
        }
    } else if (choice == 2) {
        string name;
        cout << "Enter name: ";
        getline(cin, name);
        if (cityIndexMap.count(name)) {
            cout << "Index: " << cityIndexMap[name] << "\n";
        } else {
            cout << "City not found.\n";
        }
    } else {
        cout << "Invalid choice.\n";
    }
}

void displayCities() {
    cout << left << setw(10) << "Index" << "City Name\n";
    for (auto &city : cities) {
        cout << left << setw(10) << city.index << city.name << "\n";
    }
}

void displayRoads() {
    cout << "\nRoad Adjacency Matrix (1 = road exists):\n";
    cout << setw(15) << "";
    for (int i = 1; i <= cityCount; ++i) {
        cout << setw(15) << cities[i - 1].name;
    }
    cout << "\n";
    for (int i = 1; i <= cityCount; ++i) {
        cout << setw(15) << cities[i - 1].name;
        for (int j = 1; j <= cityCount; ++j) {
            cout << setw(15) << roadMatrix[i][j];
        }
        cout << "\n";
    }

    cout << "\nBudget Matrix (in RWF):\n";
    cout << setw(15) << "";
    for (int i = 1; i <= cityCount; ++i) {
        cout << setw(15) << cities[i - 1].name;
    }
    cout << "\n";
    for (int i = 1; i <= cityCount; ++i) {
        cout << setw(15) << cities[i - 1].name;
        for (int j = 1; j <= cityCount; ++j) {
            cout << setw(15) << fixed << setprecision(0) << budgetMatrix[i][j];
        }
        cout << "\n";
    }
}

void displayAll() {
    displayCities();
    displayRoads();
}

void saveToFile() {
    ofstream cityFile("cities.txt");
    cityFile << "Index  City_name\n";
    for (auto &city : cities) {
        cityFile << city.index << "      " << city.name << "\n";
    }
    cityFile.close();

    ofstream roadFile("roads.txt");
    roadFile << "Nbr  Road                     Budget (Billions RWF)\n";
    int count = 1;
    for (int i = 1; i <= cityCount; ++i) {
        for (int j = i + 1; j <= cityCount; ++j) {
            if (roadMatrix[i][j]) {
                roadFile << left << setw(4) << count++ << setw(25)
                         << cities[i - 1].name + " - " + cities[j - 1].name
                         << fixed << setprecision(2) << budgetMatrix[i][j] / 1e9 << "\n";
            }
        }
    }
    roadFile.close();

    ofstream graphFile("graph.txt");
    for (int i = 1; i <= cityCount; ++i) {
        graphFile << cities[i - 1].name << ": ";
        for (int j = 1; j <= cityCount; ++j) {
            if (roadMatrix[i][j]) {
                graphFile << cities[j - 1].name;
                if (budgetMatrix[i][j] > 0) {
                    graphFile << "(" << fixed << setprecision(2) << budgetMatrix[i][j] / 1e9 << ")";
                }
                graphFile << ", ";
            }
        }
        graphFile << "\n";
    }
    graphFile.close();

    cout << "Data successfully saved to 'cities.txt', 'roads.txt', and 'graph.txt'.\n";
}

int main() {
    int choice;
    do {
        cout << "\nMenu:\n"
             << "1. Add new city(ies)\n"
             << "2. Add roads between cities\n"
             << "3. Add the budget for roads\n"
             << "4. Edit city\n"
             << "5. Search for a city\n"
             << "6. Display cities\n"
             << "7. Display roads\n"
             << "8. Display recorded data on console\n"
             << "9. Save to file and Exit\n"
             << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addCities(); break;
            case 2: addRoad(); break;
            case 3: addBudget(); break;
            case 4: editCity(); break;
            case 5: searchCity(); break;
            case 6: displayCities(); break;
            case 7: displayRoads(); break;
            case 8: displayAll(); break;
            case 9: saveToFile(); cout << "Data saved. Exiting...\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 9);

    return 0;
}