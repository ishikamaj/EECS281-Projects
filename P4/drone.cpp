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

enum class LocationType { Normal, Medical, Border, Empty};

class Location {
    public:

        Location();
        Location(double x_coord_in, double y_coord_in, int location_num_in, char mode_in);

        double get_x_coord();
        double get_y_coord();
        LocationType get_location_type();
        int get_location_num();

    private:
        double xCoord;
        double yCoord;
        int locationNum;
        LocationType locationType;
};

class Drone {
    public:

        Drone();
        void get_options(int argc, char** argv);
        char get_mode();
        double get_distance(Location &l1, Location &l2);
        void read_input();

        //MST

        void run_MST();
        void prim_algorithm();
        void prim_initialize(Location &first, size_t firstIndex);
        void prim_alg_update(Location &next, size_t nextIndex);

        size_t find_closest_location();
        double MST_get_total_dist();
        void MST_print();

        //FASTTSP

        void run_FAST();
        void FAST_initialize(size_t firstIndex, size_t secondIndex, size_t thirdIndex, double &totalDist);
        void FAST_initialize_distance();
        void FAST_insert_algorithm(double &totalDist);
        void FAST_print(double totalDist);
        double FAST_distance_change(size_t firstIndex, size_t secondIndex, size_t newIndex);

        //OPTTSP

        void run_OPT();
        void genPerms(size_t permLength);
        bool promising(size_t permLength);

        void OPT_initialize();
        void OPT_FAST_helper();
        void OPT_prim_update(Location &next, size_t nextIndex, vector<size_t> &unvisited);
        void OPT_prim_initialize(Location &first, size_t firstIndex, vector<size_t> &unvisited);
        void OPT_prim_alg(vector<size_t> &unvisited);
        void OPT_reset_prim();
        void OPT_print();

    private:

        char mode;
        int numLocations;
        vector<Location> locationsVector;

        //part a
        vector<Location> primParents;
        vector<double>primDist;
        vector<bool> primVisited;

        //part b
        vector<size_t> FASTpath;

        //part c
        vector<size_t> OPTpath;
        vector<size_t> OPTbestPath;
        double OPTbestDist;
        double OPTcurrentDist;
};      

//Location Definitions
Location::Location() {
    locationType = LocationType::Empty;
    locationNum = -1;
}

Location::Location(double x_coord_in, double y_coord_in, int location_num_in, char mode_in) {
    locationNum = location_num_in;
    xCoord = x_coord_in;
    yCoord = y_coord_in;

    if (mode_in == 'M') {
        //both negative (bottom left quadrant) Medical
        if (xCoord < 0 && yCoord < 0) {
            locationType = LocationType::Medical;
        }
        //on axis (Border)
        else if ((xCoord < 0 && yCoord == 0) || (yCoord < 0 && xCoord == 0) || (xCoord == 0 && yCoord == 0)) {
            locationType = LocationType::Border;
        }
        else {
            locationType = LocationType::Normal;
        }
    }
}

double Location::get_x_coord() {
    return xCoord;
}

double Location::get_y_coord() {
    return yCoord;
}

LocationType Location::get_location_type() {
    return locationType;
}

int Location::get_location_num() {
    return locationNum;
}

//Drone Definitions
Drone::Drone() {
    mode = 'N';
}

void Drone::get_options(int argc, char** argv) {
    int optionIndex = 0, option = 0;
    opterr = false;

    struct option longOpts[] = {{ "mode", required_argument, nullptr, 'm' },
        { "help", no_argument, nullptr, 'h' },
        { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "m:h", longOpts, &optionIndex)) != -1) {
        switch (option) {
            case 'h':
                cerr << "This program simulates an on-campus drone delivery service.\n"
                << "There are two types of drones (Drone Type I and Drone Type II), and \n"
                << "three types of clients (A, B, and C). The program will find the\n"
                << "shortest route to be hit all locations across campus.\n"
                << "Usage: \'./drone\n"
                << "\t[--help | -h]\n"
                <<  "\t[--mode | -m] type either \"MST\", \"FASTTSP\", or \"OPTTSP\"\n";
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
                else { // Invalid 
                    cerr << "Error: Invalid arguments. \"mode\" must be either: "
                    << "\"MST\", \"FASTTSP\", or \"OPTTSP\".\n";
                }
                break;
            default:
                cerr << "Error: Invalid arguments. \n";
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

    //formula
    double x1 = l1.get_x_coord();
    double y1 = l1.get_y_coord();

    double x2 = l2.get_x_coord();
    double y2 = l2.get_y_coord();

    double distance = pow((x2 - x1), 2) + pow((y2 - y1), 2);
    distance = sqrt(distance);
    return distance;
}

//MST

void Drone::run_MST() {
    read_input();
    prim_algorithm();
    MST_print();
}

void Drone::read_input() {
    int num_locations_in;
    double x_in, y_in;

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
    prim_initialize(first, 0);
    int count = 1;

    while (count != numLocations) {
        size_t nextIndex = find_closest_location();

        prim_alg_update(locationsVector[nextIndex], nextIndex);
        ++count;
    }
}

void Drone::prim_initialize(Location &first, size_t firstIndex) {
    vector<Location> tempParents(static_cast<size_t>(numLocations), first);
    primParents.swap(tempParents);

    vector<double> tempDist(static_cast<size_t>(numLocations));

    for (size_t i = 0; i < static_cast<size_t>(numLocations); ++i) {
        tempDist[i] = get_distance(first, locationsVector[i]);
    }

    primDist.swap(tempDist);

    vector<bool> tempVisited(static_cast<size_t>(numLocations), false);
    tempVisited[firstIndex] = true;

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
    if ((minDist == std::numeric_limits<double>::infinity()) || (index == -1)) {
        cerr << "Error: No closest location found.\n";
        exit(1);
    }
    
    return static_cast<size_t>(index);
}

void Drone::prim_alg_update(Location &next, size_t nextIndex) {
    primVisited[nextIndex] = true;

    for (size_t i = 0; i < static_cast<size_t>(numLocations); ++i) {
        if (i == nextIndex) {
            continue;
        }
        if (primVisited[i] == false) {
            double tempDist = get_distance(next, locationsVector[i]);
            if (tempDist < primDist[i]) {
                primParents[i] = next;
                primDist[i] = tempDist;
            }
        }
    }
}

double Drone::MST_get_total_dist() {
    double totalWeight = 0;

    for (size_t i = 0; i < primDist.size(); ++i) {
        if (primDist[i] == numeric_limits<double>::infinity()) {
            cerr << "Error: edge with length infinity found\n";
            exit(1);
        }
        totalWeight += primDist[i];
    }

    return totalWeight;
}

void Drone::MST_print() {
    double totalWeight = MST_get_total_dist();
    cout << "Total Weight: " << totalWeight << endl;

    for (size_t i = 1; i < static_cast<size_t>(numLocations); ++i) {
        Location thisLocation = locationsVector[i];
        Location parentLocation = primParents[i];

        if (thisLocation.get_location_num() < parentLocation.get_location_num()) {
            cout << thisLocation.get_location_num() << " " << parentLocation.get_location_num() << endl;
        }
        else {
            cout << parentLocation.get_location_num() << " " << thisLocation.get_location_num() << endl;
        }
    }
}

//fasttsp

void Drone::run_FAST() {
    read_input();
    double totalDist = 0;
    FAST_initialize(0, 1, 2, totalDist);
    FAST_insert_algorithm(totalDist);
    FAST_print(totalDist);
}

void Drone::FAST_initialize(size_t firstIndex, size_t secondIndex, size_t thirdIndex, double &totalDist) {
    FASTpath.reserve(static_cast<size_t>(numLocations + 1));
    
    FASTpath.push_back(firstIndex);
    FASTpath.push_back(secondIndex);
    FASTpath.push_back(thirdIndex);
    FASTpath.push_back(firstIndex);
    
    totalDist += get_distance(locationsVector[firstIndex], locationsVector[secondIndex]) + get_distance(locationsVector[secondIndex], locationsVector[thirdIndex]) +  get_distance(locationsVector[thirdIndex], locationsVector[firstIndex]);
}

void Drone::FAST_insert_algorithm(double &totalDist) {
    for (size_t i = 3; i < static_cast<size_t>(numLocations); ++i) {
        double minDistChange = std::numeric_limits<double>::infinity();
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

double Drone::FAST_distance_change(size_t firstIndex, size_t secondIndex, size_t newIndex) {
    Location first = locationsVector[FASTpath[firstIndex]];
    Location second = locationsVector[FASTpath[secondIndex]];
    Location newLocation = locationsVector[newIndex];
    double distanceChange = get_distance(first, newLocation) + get_distance(newLocation, second) - get_distance(first, second);
    return distanceChange;
}

void Drone::FAST_print(double totalDist) {
    FASTpath.pop_back();
    cout << totalDist << endl;

    for (size_t i = 0; i < FASTpath.size(); ++i) {
        cout << FASTpath[i] << " ";
    }
}

//opttsp

void Drone::run_OPT() {
    read_input();
    OPT_initialize();
    genPerms(1);
    OPT_print();
}

void Drone::OPT_initialize() {
    OPT_FAST_helper();
    OPTpath.resize(static_cast<size_t>(numLocations));

    for (size_t i = 0; i < static_cast<size_t>(numLocations); ++i) {
        OPTpath[i] = i;
    }

    OPTcurrentDist = 0;
}

void Drone::OPT_FAST_helper() {
    double totalDist;
    FAST_initialize(0, 1, 2, totalDist);
    FAST_insert_algorithm(totalDist);
    OPTbestDist = totalDist;

    FASTpath.pop_back();
    OPTbestPath = FASTpath;
}

void Drone::genPerms(size_t permLength) {
    if (permLength == 25) {
        cout << "DEBUG\n";
    }
    
    if (permLength == OPTpath.size()) {
        double closingEdge = get_distance(locationsVector[OPTpath[0]], locationsVector[OPTpath[permLength - 1]]);
        OPTcurrentDist += closingEdge;

        if (OPTcurrentDist < OPTbestDist) {
            OPTbestDist = OPTcurrentDist;
            OPTbestPath = OPTpath;
        }
        OPTcurrentDist -= closingEdge;
        return;
    }
    if (!promising(permLength)) {
        return;
    }

     for (size_t i = permLength; i < OPTpath.size(); ++i) {
        swap(OPTpath[permLength], OPTpath[i]);
        OPTcurrentDist += get_distance(locationsVector[OPTpath[permLength]], locationsVector[OPTpath[permLength - 1]]);
        genPerms(permLength + 1);
        OPTcurrentDist -= get_distance(locationsVector[OPTpath[permLength]], locationsVector[OPTpath[permLength - 1]]);
        swap(OPTpath[permLength], OPTpath[i]);
    }
}

bool Drone::promising(size_t permLength) {
    if (OPTpath.size() - permLength <= 5) {
        return true;
    }
    vector<size_t> unvisited;
    unvisited.reserve(static_cast<size_t>(numLocations) - permLength);
    
    for (size_t i = permLength; i < OPTpath.size(); ++i) {
        unvisited.push_back(i);
    }
    OPT_prim_alg(unvisited);
    double zeroDist = numeric_limits<double>::infinity(), lastDist = numeric_limits<double>::infinity();
    
    for (size_t i = 0; i < unvisited.size(); ++i) {
        
        double tempZero = get_distance(locationsVector[OPTpath[unvisited[i]]], locationsVector[OPTpath[0]]);
        if (tempZero < zeroDist) {
            zeroDist = tempZero;
        }
        double tempLast = get_distance(locationsVector[OPTpath[unvisited[i]]], locationsVector[OPTpath[permLength - 1]]);
        
        if (tempLast < lastDist) {
            lastDist = tempLast;
        }
       
    }
    
    double lowerBound = MST_get_total_dist() + OPTcurrentDist + zeroDist + lastDist;
    
    OPT_reset_prim();
    
    if (lowerBound < OPTbestDist) {
        return true;        
    }
    else {
        return false;
    }
}

void Drone::OPT_prim_alg(vector<size_t> &unvisited) {
    Location first = locationsVector[unvisited[0]];
    OPT_prim_initialize(first, 0, unvisited);
    size_t count = 1;
    while (count != unvisited.size()) {
        
        size_t nextIndex = find_closest_location();
        OPT_prim_update(locationsVector[unvisited[nextIndex]], nextIndex, unvisited);
        ++count;
    }
}

void Drone:: OPT_prim_initialize(Location &first, size_t firstIndex, vector<size_t> &unvisited) {
    vector<Location> tempParents(static_cast<size_t>(unvisited.size()), first);
    primParents.swap(tempParents);
    
    vector<double> tempDist(unvisited.size());
    for (size_t i = 0; i < unvisited.size(); ++i) {
        tempDist[i] = get_distance(first, locationsVector[unvisited[i]]);
    }
    
    primDist.swap(tempDist);
    vector<bool> tempVisited(unvisited.size(), false);
    tempVisited[firstIndex] = true;
    primVisited.swap(tempVisited);
    return;
}

void Drone::OPT_prim_update(Location &next, size_t nextIndex, vector<size_t>&unvisited) {
    primVisited[nextIndex] = true;
    
    for (size_t i = 0; i < unvisited.size(); ++i) {
        if (i == nextIndex) {
            continue;
        }
        if (primVisited[i] == false) {
            double tempDist = get_distance(next, locationsVector[unvisited[i]]);
            if (tempDist < primDist[i]) {
                primParents[i] = next;
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
    cout << OPTbestDist << endl;
    
    for (size_t i = 0; i < OPTbestPath.size(); ++i) {
        cout << OPTbestPath[i] << endl;
    }
}

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);

    cout << setprecision(2);
    cout << fixed;

    Drone d1;
    d1.get_options(argc, argv);

    if (d1.get_mode() == 'M') {
        d1.run_MST();
    }
    else if (d1.get_mode() == 'F') {
        d1.run_FAST();
    }
    else if (d1.get_mode() == 'O') {
        d1.run_OPT();
    }
    else {
        cerr << "Error! Invalid mode - " << d1.get_mode() << " read in.\n";
        exit(1);
    }

    return 0;
}