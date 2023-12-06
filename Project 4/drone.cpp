// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0

#include <getopt.h>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <stdio.h>
#include <string.h>

using namespace std;

//LocationType class
enum class LocationType { Medical, Border, Normal, Empty};

class Location {
    
public:
    
    Location();
    Location(int x_coord_in, int y_coord_in, 
        int location_num_in, char mode_in);
    
    int get_x_coord();
    int get_y_coord();
    LocationType get_location_type();
    int get_location_num();
    
private:
    
    int xCoord;
    int yCoord;
    int locationNum;
    
    LocationType locationType;
};

//drone class

class Drone {
    
public:
    
    Drone();
    
    void get_options(int argc, char** argv);
    char get_mode();
    double get_distance(Location &l1, Location &l2);
    void read_input();
    
    // PART A: MST //
    
    void run_MST();
    void prim_algorithm();
    void prim_initialize_vectors(Location &firstLocation, 
        size_t firstLocationIndex);
    void prim_algorithm_update(Location &nextLocation, 
        size_t nextLocationIndex);
    size_t find_closest_location();
    double MST_get_total_distance();
    void MST_print();
    
    // PART B: FASTTSP //
    
    void run_FASTTSP();
    void FAST_initialize_vectors(size_t firstIndex, size_t secondIndex, 
        size_t thirdIndex, double &totalDist);
    void FAST_initialize_distance_vector();
    void FAST_arbitrary_insert_algorithm(double &totalDist);
    void FAST_print(double totalDist);
    double FAST_distance_change(size_t firstIndex, size_t secondIndex, 
        size_t new_index);
    
    // PART C: OPTTSP //
    
    void run_OPTTSP();
    void genPerms(size_t permLength);
    bool is_promising(size_t permLength);
    void OPT_initialize();
    void OPT_modified_prim_update(Location &nextLocation, 
        size_t nextLocationIndex, vector<size_t> &unvisitedLocations);
    void OPT_modified_prim_initialize_vectors(Location &firstLocation, 
        size_t firstLocationIndex, vector<size_t> &unvisitedLocations);
    void OPT_modified_prim_algorithm(vector<size_t> &unvisitedLocations);
    void OPT_reset_prim();
    void OPT_print();
    
private:
    
    // 'N' by default; must be 'M' for MST, 'F' for FASTTSP, or 'O' for OPTTSP
    char mode;
    
    int numLocations;
    vector<Location> locationsVector;
    
    //part A
    
    vector<Location> primParents;
    vector<double> primDist;
    vector<bool> primVisited;
    
    //part b
    
    vector<size_t> FASTpath;

    //part c
    
    vector<size_t> OPTpath;
    vector<size_t> OPTbestPath;
    double OPTbestDist;
    double OPTcurrDist;
    
};

//main

int main(int argc, char** argv) {
    
    ios_base::sync_with_stdio(false);
    
    cout << setprecision(2); 
    cout << fixed;
    
    Drone d1;
    d1.get_options(argc, argv);
    
    if (d1.get_mode() == 'M') {
        d1.run_MST();
    }
    
    else if (d1.get_mode() == 'F') {
        d1.run_FASTTSP();
    }
    
    else if (d1.get_mode() == 'O') {
        d1.run_OPTTSP();
    }
    
    else {
        cerr << "Error: Invalid mode.\n";
        exit(1);
    }
    return 0;
}


//Location definitions

Location::Location() {
    locationType = LocationType::Empty;
    locationNum = -1;
}

Location::Location(int x_coord_in, int y_coord_in, 
    int location_num_in, char mode_in) {
    
    locationNum = location_num_in;
    xCoord = x_coord_in;
    yCoord = y_coord_in;
    
    if (mode_in == 'M') {
        if (xCoord < 0 && yCoord < 0) {
            locationType = LocationType::Medical;
        }
        else if ((xCoord < 0 && yCoord == 0) ||
                 (yCoord < 0 && xCoord == 0) ||
                 (xCoord == 0 && yCoord == 0)) {
            locationType = LocationType::Border;
        }
        else {
            locationType = LocationType::Normal;
        }
    }
}

int Location::get_x_coord() {
    return xCoord;
}

int Location::get_y_coord() {
    return yCoord;
}

LocationType Location::get_location_type() {
    return locationType;
}

int Location::get_location_num() {
    return locationNum;
}

//Drone definitions

Drone::Drone() {
    mode = 'N';
}

void Drone::get_options(int argc, char** argv) {
    int optionIndex = 0, option = 0;
    opterr = false;
    
    struct option longOpts[] = {{ "mode", required_argument, nullptr, 'm' },
        { "help", no_argument, nullptr, 'h' },
        { nullptr, 0, nullptr, '\0' }};
    
    while ((option = 
        getopt_long(argc, argv, "m:h", longOpts, &optionIndex)) != -1) {
        switch (option) {
            case 'h':
                cerr << "This program simulates a drone delivery service.\n"
                << "There are two types of drones and \n"
                << "three types of clientsThe program will aim to find the\n"
                << "shortest route to service all locations across\n"
                << "campus.\n";
                
                exit(0);
                
            case 'm':
                if (strcmp(optarg, "MST") == 0) { // MST 
                    mode = 'M';
                }
                else if (strcmp(optarg, "FASTTSP") == 0) { // FASTTSP
                    mode = 'F';
                }
                else if (strcmp(optarg, "OPTTSP") == 0) { // OPTTSP 
                    mode = 'O'; 
                }
                else {
                    cerr << "Error: Invalid command line arguments.\n";
                }
                break;
                
            default:
                cerr << "Error: Invalid command line arguments.\n";
                exit(1);
        }
    }
}
        
char Drone::get_mode() {
    return mode;
}
        
double Drone::get_distance(Location &l1, Location &l2) {
    
    LocationType t1 = l1.get_location_type();
    LocationType t2 = l2.get_location_type();
    
    if ((t1 == LocationType::Medical && t2 == LocationType::Normal) ||
        (t2 == LocationType::Medical && t1 == LocationType::Normal)) {
        return numeric_limits<double>::infinity();
    }

    double x1 = static_cast<double>(l1.get_x_coord());
    double y1 = static_cast<double>(l1.get_y_coord());
    
    double x2 = static_cast<double>(l2.get_x_coord());
    double y2 = static_cast<double>(l2.get_y_coord());
    
    double distance = pow((x2 - x1), 2) + pow((y2 - y1), 2);
    distance = sqrt(distance);
    return distance;
}

//part a, mst

void Drone::run_MST() {
    read_input();
    prim_algorithm();
    MST_print();
}

void Drone::read_input() {
    int num_locations_in, x_in, y_in;
    cin >> num_locations_in;
    numLocations = num_locations_in;
    locationsVector.reserve(static_cast<size_t>(numLocations));
    for (int i = 0; i < numLocations; ++i) {
        cin >> x_in >> y_in;
        Location l_in(x_in, y_in, i, mode);
        locationsVector.push_back(l_in);
    }
}

void Drone::prim_algorithm() {
    
    Location first = locationsVector[0];
    prim_initialize_vectors(first, 0);
    int count = 1;
    
    while (count != numLocations) {
        size_t nextLocationIndex = find_closest_location();
        prim_algorithm_update(locationsVector[nextLocationIndex], 
            nextLocationIndex);
        ++count;
    }
}

void Drone::prim_initialize_vectors(Location &firstLocation, 
    size_t firstLocationIndex) {
    
    vector<Location> tempParents(static_cast<size_t>(numLocations), 
        firstLocation);
    primParents.swap(tempParents);
    vector<double> tempDists(static_cast<size_t>(numLocations));
    
    for (size_t i = 0; i < static_cast<size_t>(numLocations); ++i) {
        tempDists[i] = get_distance(firstLocation, locationsVector[i]);
    }
    
    primDist.swap(tempDists);
    vector<bool> tempVisited(static_cast<size_t>(numLocations), false);
    tempVisited[firstLocationIndex] = true;
    primVisited.swap(tempVisited);
    return;
}

size_t Drone::find_closest_location() {
    
    double minDist = numeric_limits<double>::infinity();
    int index = -1;
    
    for (int i = 0; i < static_cast<int>(primDist.size()); ++i) {
        if (primDist[static_cast<size_t>(i)] < minDist) {
            if (primVisited[static_cast<size_t>(i)] == false) {
                minDist = primDist[static_cast<size_t>(i)];
                index = i;
            }
        }
    }

    // DEBUG
    if ((minDist == numeric_limits<double>::infinity()) || (index == -1)) {
        cerr << "Error: No closest location found.\n";
        exit(1);
    }
    return static_cast<size_t>(index);
}

void Drone::prim_algorithm_update(Location &nextLocation, 
    size_t nextLocationIndex) {
    
    primVisited[nextLocationIndex] = true;
    
    for (size_t i = 0; i < static_cast<size_t>(numLocations); ++i) {
        if (i == nextLocationIndex) {
            continue;
        }
        
        if (primVisited[i] == false) {
            double tempDist = get_distance(nextLocation, locationsVector[i]);
            if (tempDist < primDist[i]) {
                primParents[i] = nextLocation;
                primDist[i] = tempDist;
            }
        }
    }
}

double Drone::MST_get_total_distance() {
    
    double totalWeight = 0;
    for (size_t i = 0; i < primDist.size(); ++i) {
        if (primDist[i] == numeric_limits<double>::infinity()) {
            cerr << "Error: Found an edge with length INFINITY.\n";
            exit(1);
        }
        totalWeight += primDist[i];
    }
    return totalWeight;
}

void Drone::MST_print() {

    double totalWeight = MST_get_total_distance();
    cout << totalWeight << "\n";
    
    for (size_t i = 1; i < static_cast<size_t>(numLocations); ++i) {
        Location thisLocation = locationsVector[i];
        Location parentLocation = primParents[i];
        if (thisLocation.get_location_num() < 
            parentLocation.get_location_num()) {
            cout << thisLocation.get_location_num() << " " 
                 << parentLocation.get_location_num() << "\n";
        }
        else {
            cout << parentLocation.get_location_num() << " " 
                 << thisLocation.get_location_num() << "\n";
        }
    }
}

//part b, fasttsp

void Drone::run_FASTTSP() {
    read_input();
    double totalDist = 0;
    FAST_initialize_vectors(0, 1, 2, totalDist);
    FAST_arbitrary_insert_algorithm(totalDist);
    FAST_print(totalDist);
}

void Drone::FAST_initialize_vectors(size_t firstIndex, 
    size_t secondIndex, size_t thirdIndex, double &totalDist) {
    
    FASTpath.reserve(static_cast<size_t>(numLocations + 1));
    
    FASTpath.push_back(firstIndex);
    FASTpath.push_back(secondIndex);
    FASTpath.push_back(thirdIndex);
    FASTpath.push_back(firstIndex);
    
    totalDist += get_distance(locationsVector[firstIndex],
    locationsVector[secondIndex]) +
    get_distance(locationsVector[secondIndex], locationsVector[thirdIndex]) +
    get_distance(locationsVector[thirdIndex], locationsVector[firstIndex]);
    
}

void Drone::FAST_arbitrary_insert_algorithm(double &totalDist) {

    for (size_t i = 3; i < static_cast<size_t>(numLocations); ++i) {
        double minDistChange = numeric_limits<double>::infinity();
        auto it = FASTpath.begin();
        auto itInsertIndex = it;
        for (size_t j = 0; j < FASTpath.size() - 1; ++j) {
            double distChange = FAST_distance_change(j, (j + 1), i);
            if (distChange < minDistChange) {
                minDistChange = distChange;
                itInsertIndex = it;
                ++itInsertIndex;
            }
            ++it;
        }
        totalDist += minDistChange;
        FASTpath.insert(itInsertIndex, i);
    }
}

double Drone::FAST_distance_change(size_t firstIndex, 
    size_t secondIndex, size_t new_index) {
    
    Location first = locationsVector[FASTpath[firstIndex]];
    Location second = locationsVector[FASTpath[secondIndex]];
    Location newLocation = locationsVector[new_index];
    
    double distChange = get_distance(first, newLocation) + 
        get_distance(newLocation, second) - get_distance(first, second);
    return distChange;
}

void Drone::FAST_print(double totalDist) {
    
    FASTpath.pop_back();
    cout << totalDist << "\n";
    for (size_t i = 0; i < FASTpath.size(); ++i) {
        cout << FASTpath[i] << " ";
    }
}


//part c, opttsp

void Drone::run_OPTTSP() {
    read_input();
    OPT_initialize();
    genPerms(1);
    OPT_print();
}

void Drone::OPT_initialize() {

    double totalDist = 0;
    FAST_initialize_vectors(0, 1, 2, totalDist);
    FAST_arbitrary_insert_algorithm(totalDist);
    OPTbestDist = totalDist;
    FASTpath.pop_back();
    OPTbestPath = FASTpath;
    OPTpath.resize(static_cast<size_t>(numLocations));
    for (size_t i = 0; i < static_cast<size_t>(numLocations); ++i) {
        OPTpath[i] = i;
    }
    OPTcurrDist = 0;
}

void Drone::genPerms(size_t permLength) {
    
    if (permLength == 25) {
        cout << "DEBUG\n";
    }
    
    if (permLength == OPTpath.size()) {
        double closingEdge = get_distance(locationsVector[OPTpath[0]], 
            locationsVector[OPTpath[permLength - 1]]);
        OPTcurrDist += closingEdge;
        if (OPTcurrDist < OPTbestDist) {
            OPTbestDist = OPTcurrDist;
            OPTbestPath = OPTpath;
        }
        OPTcurrDist -= closingEdge;
        return;
    }
    if (is_promising(permLength) == false) {
        return;
    }
    
    for (size_t i = permLength; i < OPTpath.size(); ++i) {
        swap(OPTpath[permLength], OPTpath[i]);
        OPTcurrDist += get_distance(locationsVector[OPTpath[permLength]], 
            locationsVector[OPTpath[permLength - 1]]);
        genPerms(permLength + 1);
        OPTcurrDist -= get_distance(locationsVector[OPTpath[permLength]], 
            locationsVector[OPTpath[permLength - 1]]);
        swap(OPTpath[permLength], OPTpath[i]);
    } 
    
}

bool Drone::is_promising(size_t permLength) {
    if (OPTpath.size() - permLength <= 5) {
        return true;
    }

    vector<size_t> unvisited;
    unvisited.reserve(static_cast<size_t>(numLocations) - permLength);

    for (size_t i = permLength; i < OPTpath.size(); ++i) {
        unvisited.push_back(i);
    }

    OPT_modified_prim_algorithm(unvisited);
    double zeroDist = numeric_limits<double>::infinity(), 
    lastDist = numeric_limits<double>::infinity();
    for (size_t i = 0; i < unvisited.size(); ++i) {
        double tempZero = get_distance(locationsVector[OPTpath[unvisited[i]]], 
            locationsVector[OPTpath[0]]);
        if (tempZero < zeroDist) {
            zeroDist = tempZero;
        }
        double tempLast = get_distance(locationsVector[OPTpath[unvisited[i]]], 
            locationsVector[OPTpath[permLength - 1]]);
        if (tempLast < lastDist) {
            lastDist = tempLast;
        }
    }
    
    double lowerBound = MST_get_total_distance() + OPTcurrDist 
        + zeroDist + lastDist;

    OPT_reset_prim();

    if (lowerBound < OPTbestDist) {
        return true;
    }
    else {
        return false;
    }
}

void Drone::OPT_modified_prim_algorithm(vector<size_t> &unvisitedLocations) {
    
    Location first = locationsVector[unvisitedLocations[0]];
    OPT_modified_prim_initialize_vectors(first, 0, unvisitedLocations);
    size_t count = 1;
    while (count != unvisitedLocations.size()) {
        size_t nextLocationIndex = find_closest_location();
        OPT_modified_prim_update(
            locationsVector[unvisitedLocations[nextLocationIndex]], 
            nextLocationIndex, unvisitedLocations);
        ++count;
    }
}

void Drone::OPT_modified_prim_initialize_vectors(Location &firstLocation, 
    size_t firstLocationIndex, vector<size_t> &unvisitedLocations) {
    
    vector<Location> tempParents(static_cast<size_t>
        (unvisitedLocations.size()), firstLocation);
    primParents.swap(tempParents);
    vector<double> tempDists(unvisitedLocations.size());
    
    for (size_t i = 0; i < unvisitedLocations.size(); ++i) {
        tempDists[i] = get_distance(firstLocation, 
            locationsVector[unvisitedLocations[i]]);
    }
    
    primDist.swap(tempDists);
    vector<bool> tempVisited(unvisitedLocations.size(), false);
    tempVisited[firstLocationIndex] = true;
    primVisited.swap(tempVisited);
    return;
}

void Drone::OPT_modified_prim_update(Location &nextLocation, 
    size_t nextLocationIndex, vector<size_t> &unvisitedLocations) {
    
    primVisited[nextLocationIndex] = true;
    for (size_t i = 0; i < unvisitedLocations.size(); ++i) {
        if (i == nextLocationIndex) {
            continue;
        }
        
        if (primVisited[i] == false) {
            double tempDist = get_distance(nextLocation, 
                locationsVector[unvisitedLocations[i]]);
            if (tempDist < primDist[i]) {
                primParents[i] = nextLocation;
                primDist[i] = tempDist;
            }
        }
    }
}

void Drone::OPT_reset_prim() {
    primParents.clear();
    primDist.clear();
    primVisited.clear();
}

void Drone::OPT_print() {
    
    cout << OPTbestDist << "\n";
    for (size_t i = 0; i < OPTbestPath.size(); ++i) {
        cout << OPTbestPath[i] << " ";
    }
}