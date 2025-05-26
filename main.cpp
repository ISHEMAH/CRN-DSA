#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <cctype>    // for ::tolower
#include <algorithm> // for std::transform


using namespace std;

class City {
public:
    int index;
    string name;
    City() = default;
    City(int i, const string& n) : index(i), name(n) {}
};

class Road {
public:
    int index;
    string name;
    double budget;
    Road() = default;
    Road(int i, const string& n, double b) : index(i), name(n), budget(b) {}
};

class Application {
public:
    vector<City> cities;
    vector<Road> roads;
    vector<vector<int>> roadMatrix;
    vector<vector<double>> budgetMatrix;
    const int MAX_CITIES = 100;

    static bool isFileEmpty(const string& file_name) {
        ifstream file(file_name, ios::ate);
        return file.tellg() == 0;
    }

    static int generateNextIndex(const string& fileName) {
        ifstream file(fileName);
        if (!file.is_open()) return 1;

        string line;
        int maxIndex = 0;
        getline(file, line); // Skip header
        while (getline(file, line)) {
            istringstream iss(line);
            string indexStr;
            getline(iss, indexStr, '\t');
            int currIndex = stoi(indexStr);
            if (currIndex > maxIndex) maxIndex = currIndex;
        }
        file.close();
        return maxIndex + 1;
    }

    // Helper function to get canonical road name (alphabetically ordered)
    string getCanonicalRoadName(const string& city1, const string& city2) {
        return (city1 < city2) ? (city1 + "-" + city2) : (city2 + "-" + city1);
    }

public:
    Application() {
        roadMatrix = vector<vector<int>>(MAX_CITIES, vector<int>(MAX_CITIES, 0));
        budgetMatrix = vector<vector<double>>(MAX_CITIES, vector<double>(MAX_CITIES, 0.0));
        loadCityData();
        loadRoadData();
    }

    void writeCityData(const City& city) {
        ofstream file("cities.txt", ios_base::app);
        if (!file.is_open()) {
            cerr << "Error: Unable to open cities.txt" << endl;
            return;
        }
        if (isFileEmpty("cities.txt")) {
            file << "Index\tCity_Name\n";
        }
        file << city.index << "\t" << city.name << "\n";
        file.close();
    }

    void writeRoadData(const Road& road) {
        ofstream file("roads.txt", ios_base::app);
        if (!file.is_open()) {
            cerr << "Error: Unable to open roads.txt" << endl;
            return;
        }
        if (isFileEmpty("roads.txt")) {
                file << "Nbr\tRoad\tBudget\n";
        }
        file << road.index << "\t" << road.name << "\t" << road.budget << "\n";
        file.close();
    }

    void loadCityData() {
        ifstream file("cities.txt");
        if (!file.is_open()) return;

        string line;
        getline(file, line); // Skip header
        while (getline(file, line)) {
            City temp;
            istringstream iss(line);
            string index, name;
            getline(iss, index, '\t');
            getline(iss, name);
            temp.index = stoi(index);
            temp.name = name;
            cities.push_back(temp);
        }
        file.close();
    }

    void loadRoadData() {
        ifstream file("roads.txt");
        if (!file.is_open()) return;

        string line;
        getline(file, line); // Skip header
        while (getline(file, line)) {
            Road temp;
            istringstream iss(line);
            string index, name, budget;
            getline(iss, index, '\t');
            getline(iss, name, '\t');
            getline(iss, budget);
            temp.index = stoi(index);
            temp.name = name;
            temp.budget = stod(budget);
            roads.push_back(temp);

            // Update matrices
            size_t pos = name.find('-');
            string city1 = name.substr(0, pos);
            string city2 = name.substr(pos + 1);
            int idx1 = findCityIndex(city1);
            int idx2 = findCityIndex(city2);
            if (idx1 != -1 && idx2 != -1) {
                roadMatrix[idx1][idx2] = roadMatrix[idx2][idx1] = 1;
                budgetMatrix[idx1][idx2] = budgetMatrix[idx2][idx1] = temp.budget;
            }
        }
        file.close();
    }

    int findCityIndex(const string& name) {
        for (const auto& city : cities) {
            if (city.name == name) return city.index - 1;
        }
        return -1;
    }

    City findCityByName(const string& name) {
        for (const auto& city : cities) {
            if (city.name == name) return city;
        }
        return City();
    }

    City findCityByIndex(int index) {
        for (const auto& city : cities) {
            if (city.index == index) return city;
        }
        return City();
    }

    bool roadExists(const string& city1, const string& city2) {
        string roadName = getCanonicalRoadName(city1, city2);
        for (const auto& road : roads) {
            if (road.name == roadName) return true;
        }
        return false;
    }

    void addCity(const City& city) {
        cities.push_back(city);
        writeCityData(city);
    }

    void addRoad(const string& city1, const string& city2) {
        if (city1 == city2) {
            cout << "Error: Cities cannot be the same\n";
            return;
        }

        if (findCityByName(city1).index == 0 || findCityByName(city2).index == 0) {
            cout << "Error: One or both cities don't exist\n";
            return;
        }
        if (roadExists(city1, city2)) {
            cout << "Error: Road already exists between these cities\n";
            return;
        }

        Road road;
        road.index = generateNextIndex("roads.txt");
        road.name = getCanonicalRoadName(city1, city2); // Use canonical name
        road.budget = 0.0;
        roads.push_back(road);
        writeRoadData(road);

        int idx1 = findCityIndex(city1);
        int idx2 = findCityIndex(city2);
        roadMatrix[idx1][idx2] = roadMatrix[idx2][idx1] = 1;

        cout << "Road added between " << city1 << " and " << city2 << endl;
    }

    void addBudget(const string& city1, const string& city2, double budget) {
        if (!roadExists(city1, city2)) {
            cout << "Error: Road doesn't exist between these cities\n";
            return;
        }

        int idx1 = findCityIndex(city1);
        int idx2 = findCityIndex(city2);
        budgetMatrix[idx1][idx2] = budgetMatrix[idx2][idx1] = budget;

        // Update road budget in file
        string roadName = getCanonicalRoadName(city1, city2);
        for (auto& road : roads) {
            if (road.name == roadName) {
                road.budget = budget;
                break;
            }
        }

        // Rewrite roads file
        ofstream file("roads.txt");
        file << "Nbr\tRoad\tBudget\n";
        for (const auto& road : roads) {
            file << road.index << "\t" << road.name << "\t" << road.budget << "\n";
        }
        file.close();

        cout << "Budget of " << budget << " added for road between " << city1 << " and " << city2 << endl;
    }

    void editCity(int index, const string& newName) {
        if (findCityByIndex(index).index == 0) {
            cout << "Error: City with index " << index << " doesn't exist\n";
            return;
        }

        // Update city name in roads
        string oldName;
        for (auto& city : cities) {
            if (city.index == index) {
                oldName = city.name;
                city.name = newName;
                break;
            }
        }

        // Update road names containing the old city name
        for (auto& road : roads) {
            size_t pos = road.name.find('-');
            string city1 = road.name.substr(0, pos);
            string city2 = road.name.substr(pos + 1);
            if (city1 == oldName) {
                road.name = getCanonicalRoadName(newName, city2);
            } else if (city2 == oldName) {
                road.name = getCanonicalRoadName(city1, newName);
            }
        }

        // Rewrite cities file
        ofstream cityFile("cities.txt");
        cityFile << "Index\tCity_Name\n";
        for (const auto& city : cities) {
            cityFile << city.index << "\t" << city.name << "\n";
        }
        cityFile.close();

        // Rewrite roads file
        ofstream roadFile("roads.txt");
        roadFile << "Index\tRoad\tBudget\n";
        for (const auto& road : roads) {
            roadFile << road.index << "\t" << road.name << "\t" << road.budget << "\n";
        }
        roadFile.close();

        cout << "City updated successfully\n";
    }

    void displayCities() {
        cout << "Cities:\n";
        for (const auto& city : cities) {
            cout << city.index << ": " << city.name << "\n";
        }
    }

    void displayRoads() {
        displayCities();
        cout << "\nRoads Adjacency Matrix:\n";
        for (size_t i = 0; i < cities.size(); i++) {
            for (size_t j = 0; j < cities.size(); j++) {
                cout << roadMatrix[i][j] << " ";
            }
            cout << "\n";
        }
    }

    void displayAllData() {
        displayCities();
        cout << "\nRoads Adjacency Matrix:\n";
        for (size_t i = 0; i < cities.size(); i++) {
            for (size_t j = 0; j < cities.size(); j++) {
                cout << roadMatrix[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\nBudgets Adjacency Matrix:\n";
        for (size_t i = 0; i < cities.size(); i++) {
            for (size_t j = 0; j < cities.size(); j++) {
                cout << setw(8) << fixed << setprecision(2) << budgetMatrix[i][j] << " ";
            }
            cout << "\n";
        }
    }
};

bool getValidatedInteger(int& number) {
    string input;
    getline(cin, input);
    try {
        size_t pos;
        number = stoi(input, &pos);
        return pos == input.length();
    } catch (...) {
        return false;
    }
}

bool getValidatedDouble(double& number) {
    string input;
    getline(cin, input);
    try {
        size_t pos;
        number = stod(input, &pos);
        return pos == input.length();
    } catch (...) {
        return false;
    }
}

int main() {
    Application app;

    // Initialize with provided data
    vector<pair<string, int>> initialCities = {
        {"Kigali", 1}, {"Huye", 2}, {"Muhanga", 3}, {"Musanze", 4},
        {"Nyagatare", 5}, {"Rubavu", 6}, {"Rusizi", 7}
    };

    vector<tuple<string, string, double>> initialRoads = {
        {"Kigali", "Muhanga", 28.6}, {"Kigali", "Musanze", 28.6},
        {"Kigali", "Nyagatare", 70.84}, {"Muhanga", "Huye", 56.7},
        {"Musanze", "Rubavu", 33.7}, {"Huye", "Rusizi", 80.96},
        {"Muhanga", "Rusizi", 117.5}, {"Musanze", "Nyagatare", 96.14},
        {"Muhanga", "Musanze", 66.3}
    };

    for (const auto& city : initialCities) {
        app.addCity(City(city.second, city.first));
    }

    for (const auto& road : initialRoads) {
        app.addRoad(get<0>(road), get<1>(road));
        app.addBudget(get<0>(road), get<1>(road), get<2>(road));
    }

    while (true) {
        cout << "\nMenu:\n"
             << "1. Add Cities\n"
             << "2. Add road between cities\n"
             << "3. Add budget for cities\n"
             << "4. Edit city\n"
             << "5. Search for city using its index or name\n"
             << "6. Display cities\n"
             << "7. Display roads\n"
             << "8. Display recorded data on console\n"
             << "9. Exit\n"
             << "Enter your choice: ";

        int choice;
        if (!getValidatedInteger(choice)) {
            cout << "Invalid input\n";
            continue;
        }

        switch (choice) {
            case 1: {
                int numCities;
                cout << "Enter the number of Cities to add: ";
                if (!getValidatedInteger(numCities) || numCities < 0) {
                    cout << "Invalid number of cities\n";
                    break;
                }
                for (int i = 0; i < numCities; i++) {
                    City temp;
                    cout << "Enter the name for city " << i + 1 << ": ";
                    getline(cin, temp.name);
                    temp.index = app.generateNextIndex("cities.txt");
                    app.addCity(temp);
                }
                cout << "Cities added successfully\n";
                break;
            }
            case 2: {
                string city1, city2;
                cout << "Enter the name of first city: ";
                getline(cin, city1);
                cout << "Enter the name of second city: ";
                getline(cin, city2);
                app.addRoad(city1, city2);
                break;
            }
            case 3: {
                string city1, city2;
                double budget;
                cout << "Enter the name of first city: ";
                getline(cin, city1);
                cout << "Enter the name of second city: ";
                getline(cin, city2);
                cout << "Enter the budget for the road: ";
                if (!getValidatedDouble(budget) || budget < 0) {
                    cout << "Invalid budget\n";
                    break;
                }
                app.addBudget(city1, city2, budget);
                break;
            }
            case 4: {
                int index;
                string newName;
                cout << "Enter index of the city to edit: ";
                if (!getValidatedInteger(index)) {
                    cout << "Invalid index\n";
                    break;
                }
                cout << "Enter the new name for the city: ";
                getline(cin, newName);
                app.editCity(index, newName);
                break;
            }
            case 5: {
                cout << "1. Search by index\n2. Search by name\nEnter choice: ";
                int searchChoice;
                if (!getValidatedInteger(searchChoice)) {
                    cout << "Invalid choice\n";
                    break;
                }
                if (searchChoice == 1) {
                    int index;
                    cout << "Enter city index: ";
                    if (!getValidatedInteger(index)) {
                        cout << "Invalid index\n";
                        break;
                    }
                    City city = app.findCityByIndex(index);
                    if (city.index != 0) {
                        cout << "City found: " << city.index << ": " << city.name << "\n";
                    } else {
                        cout << "No city with that index found\n";
                    }
                } else if (searchChoice == 2) {
                    string name;
                    cout << "Enter city name: ";
                    getline(cin, name);
                    City city = app.findCityByName(name);
                    if (city.index != 0) {
                        cout << "City found: " << city.index << ": " << city.name << "\n";
                    } else {
                        cout << "No city with that name found\n";
                    }
                } else {
                    cout << "Invalid search choice\n";
                }
                break;
            }
            case 6:
                app.displayCities();
                break;
            case 7:
                app.displayRoads();
                break;
            case 8:
                app.displayAllData();
                break;
            case 9:
                return 0;
            default:
                cout << "Invalid choice\n";
        }
    }
}