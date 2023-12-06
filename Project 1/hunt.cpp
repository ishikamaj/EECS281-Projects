// PROJECT IDENTIFIER: 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD

#include <stack>
#include <queue>
#include <deque>
#include <vector>
#include <iostream>
#include <utility>
#include "map.cpp"
#include "captain.cpp"
#include "firstmate.cpp"
#include <getopt.h>

using namespace std;

class Hunt {
private:
    // hunt participants
    Firstmate first;
    Captain capt;
    Map treasuremap;
    
    vector<pair<int, int>> treasuredeck;
    pair<int, int> treasurelocation;
    pair<int, int> startinglocation;

    bool verbose;
    bool stats;
    bool showPath;
    char pathtype;
    char maptype;
    int pathlength;

public:
    // default constructor
    Hunt() {
        verbose = false;
        stats = false;
        showPath = false;
        pathtype = '\0';
        maptype = '\0';
        pathlength = 0;

    }
    
    //tracing back from treasure
    void make_path() {
        treasuredeck.push_back(treasurelocation);
        pair<int, int> temp(treasurelocation.first, treasurelocation.second);
        Tile traceback = treasuremap.get_tile(treasurelocation.first, treasurelocation.second);
        while (traceback.previous != '\0') {
            pathlength++;
            //if you went north then the previous tile was 1 above so subtract
            if (traceback.previous == 'n') {
                temp.first--;
                treasuredeck.push_back(temp);
                traceback = treasuremap.get_tile(temp.first, temp.second);
            }
            //if you went east then the previous tile was 1 to the right so add
            else if (traceback.previous == 'e') {
                temp.second++;
                treasuredeck.push_back(temp);
                traceback = treasuremap.get_tile(temp.first, temp.second);
            }
            //if you went south then the previous tile was 1 below so add
            else if (traceback.previous == 's') {
                temp.first++;
                treasuredeck.push_back(temp);
                traceback = treasuremap.get_tile(temp.first, temp.second);
            }
            //if you went west then the previous tile was 1 to the left so subtract
            else {
                temp.second--;
                treasuredeck.push_back(temp);
                traceback = treasuremap.get_tile(temp.first, temp.second);
            }
        }
    }

    void print_path() {
        //start with map type
        if (pathtype == 'M') {
            // starts away from start location
            size_t tdsize = treasuredeck.size();
            for (size_t i = 1; i < tdsize; i++) {
                //if north is the previous
                if (treasuremap.get_tile(treasuredeck[i].first, 
                treasuredeck[i].second).previous == 'n') {
                    //east and west in comparison to north make a corner,
                    // so use + to signify that corner
                    if (treasuremap.get_tile(treasuredeck[i-1].first, 
                    treasuredeck[i-1].second).previous == 'e' ||
                    treasuremap.get_tile(treasuredeck[i-1].first, 
                    treasuredeck[i-1].second).previous == 'w') {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '+';
                    }
                    //otherwise it's just a vertical line to go north
                    else {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '|';
                    } 
                }
                //if east is the previous
                else if (treasuremap.get_tile(treasuredeck[i].first, 
                treasuredeck[i].second).previous == 'e') {
                    //north and south in comparison to east make a corner,
                    // so use + to signify that corner
                    if (treasuremap.get_tile(treasuredeck[i-1].first, 
                    treasuredeck[i-1].second).previous == 'n' ||
                    treasuremap.get_tile(treasuredeck[i-1].first, 
                    treasuredeck[i-1].second).previous == 's') {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '+';
                    }
                    //otherwise it's just a horizontal line to go east
                    else {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '-';
                    }
                    
                }
                //if south is the previous
                else if (treasuremap.get_tile(treasuredeck[i].first, 
                treasuredeck[i].second).previous == 's') {
                    //east and west in comparison to south make a corner,
                    // so use + to signify that corner
                    if (treasuremap.get_tile(treasuredeck[i-1].first, 
                    treasuredeck[i-1].second).previous == 'e' ||
                    treasuremap.get_tile(treasuredeck[i-1].first, 
                    treasuredeck[i-1].second).previous == 'w') {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '+';
                    } 
                    //otherwise it's just a vertical line to go south
                    else {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '|';
                    }
                } 
                //if west is the previous
                else {
                    //north and south in comparison to west make a corner,
                    // so use + to signify that corner
                    if (treasuremap.get_tile(treasuredeck[i-1].first,
                    treasuredeck[i-1].second).previous == 'n' ||
                    treasuremap.get_tile(treasuredeck[i-1].first, 
                    treasuredeck[i-1].second).previous == 's') {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '+';
                    }
                    //otherwise it's just a horizontal line to go west
                    else {
                        treasuremap.get_tile(treasuredeck[i].first, 
                        treasuredeck[i].second).location = '-';
                    } 
                }
            }
            treasuremap.get_tile(treasurelocation.first, treasurelocation.second).location
                        = 'X';
            treasuremap.get_tile(startinglocation.first, startinglocation.second).location
                        = '@';
            treasuremap.print_map();
        }
        //list type
        else {
            cout << "Sail:\n";
            for (size_t i = treasuredeck.size() - 1; i > 0; i--) {
                //if it's water or the start, you sail
                if (treasuremap.get_tile(treasuredeck[i].first, 
                treasuredeck[i].second).location == '.' || 
                treasuremap.get_tile(treasuredeck[i].first, 
                treasuredeck[i].second).location == '@' ) {
                    cout << treasuredeck[i].first << "," << 
                    treasuredeck[i].second << "\n";
                }
            }
            cout << "Search:\n";
            //if it's land or the treasure, you search
            for (size_t i = treasuredeck.size() - 1; i > 0; i--) {
                if ((treasuremap.get_tile(treasuredeck[i].first, 
                treasuredeck[i].second).location == 'o') || 
                (treasuremap.get_tile(treasuredeck[i].first, 
                treasuredeck[i].second).location == '$') ) {
                    cout << treasuredeck[i].first << "," << treasuredeck[i].second << "\n";
                }
            }
            cout << treasuredeck[0].first << "," << treasuredeck[0].second << "\n";
        }
    }
    
    void treasurehunting() {
        bool treasure = false;
        while (!capt.deck_empty()) {
            //captain goes first and loop continues until path found
            //or until deck runs out
            capt.investigate();
            treasure = (discover_capt(capt.get_current_location().first,
                capt.get_current_location().second));
            
            if (treasure) {
                break;
            }
        }
        //if treasure found
        if (treasure) {
            make_path();
            if (stats) {
                cout << "--- STATS ---\n";
                cout << "Starting location: " << startinglocation.first << ","
                          << startinglocation.second << "\n";
                cout << "Water locations investigated: "
                          << capt.get_tiles_investigated() << "\n";
                cout << "Land locations investigated: "
                          << first.get_tiles_investigated() << "\n";
                cout << "Went ashore: " << first.get_wentashore() << "\n";
                cout << "Path length: " << pathlength << "\n";
                cout << "Treasure location: " << treasurelocation.first << ","
                          << treasurelocation.second << "\n";
                cout << "--- STATS ---\n";
            }
            if (showPath) {
                print_path();
            }
            cout << "Treasure found at " << treasurelocation.first
                      << "," << treasurelocation.second
                      << " with path length " << pathlength << ".\n";
        }
        //captain didn't find it and there is no path found
        else {
            if (verbose) {
                cout << "Treasure hunt failed\n";
            }
            if (stats) {
                cout << "--- STATS ---\n";
                cout << "Starting location: " << startinglocation.first << ","
                << startinglocation.second << "\n";
                cout << "Water locations investigated: " << 
                capt.get_tiles_investigated() << "\n";
                cout << "Land locations investigated: " << 
                first.get_tiles_investigated() << "\n";
                cout << "Went ashore: " << first.get_wentashore() << "\n";
                cout << "--- STATS ---\n";
            }
            cout << "No treasure found after investigating "
                      << capt.get_tiles_investigated() + 
                      first.get_tiles_investigated() << " locations.\n";
        }
    }

    //firstmate's turn
    bool fm_hunt() {
        first.increment_wentashore();
        if (verbose) {
            cout << "Searching island... ";
        }
        //firstmate goes and loop continues until path found 
        //or deck runs out
        while (!(first.deck_empty())) {
            first.investigate();
            if (treasuremap.get_tile(first.get_current_location().first, 
            first.get_current_location().second).location == '$') {
                pair<int, int> treasure(first.get_current_location().first, 
                first.get_current_location().second);
                treasurelocation = treasure;
                if (verbose) {
                    cout << "party found treasure at " << treasurelocation.first
                              << "," << treasurelocation.second << ".\n";
                }
                return true;
            }
            //if firstmate discovers it first
            if (discover_first(first.get_current_location().first,
                               first.get_current_location().second)) {
                first.increment_tilesinvestigated();
                return true;
            }
        }
        if (verbose) {
            cout << "party returned with no treasure.\n";
        }
        return false;
    }

    bool discover_capt(int row, int col) {
        bool fmhunt = false;
        //4 for each of the directions
        for (int i = 0; i < 4; i++) {
            //if the hunt is to the north
            if (capt.get_hunt_order()[i] == 'n') {
                //if not the first row
                if (row - 1 >= 0) {
                    //if the north tile is water and has not been searched
                    if (treasuremap.get_tile(row - 1, col).location == '.' 
                    && treasuremap.get_tile(row - 1, col).previous == '\0') {
                        capt.discover_tile(row - 1, col);
                        treasuremap.get_tile(row - 1, col).previous = 's';
                    }
                    //if the hunt to the north is land and not been searched
                    else if (treasuremap.get_tile(row - 1, col).location == 'o'
                    && treasuremap.get_tile(row - 1, col).previous == '\0') {
                        first.discover_tile(row - 1, col);
                        treasuremap.get_tile(row - 1, col).previous = 's';
                        if (verbose) {
                            cout << "Went ashore at: " << row - 1 << "," << 
                            col << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                    //if north is the treasure
                    else if (treasuremap.get_tile(row - 1, col).location == '$') {
                        first.discover_tile(row - 1, col);
                        treasuremap.get_tile(row - 1, col).previous = 's';
                        if (verbose) {
                            cout << "Went ashore at: " << row - 1 << "," << col << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                }
            }
            //if the hunt is to the east
            else if (capt.get_hunt_order()[i] == 'e') {
                //not the last col
                if (col + 1 < treasuremap.get_size()) {
                    //if the hunt to the east is water and not been searched
                    if (treasuremap.get_tile(row, col + 1).location == '.'
                    && treasuremap.get_tile(row, col + 1).previous == '\0') {
                        capt.discover_tile(row, col + 1);
                        treasuremap.get_tile(row, col + 1).previous = 'w';
                    }
                    //if the hunt to the east is land and unsearched
                    else if (treasuremap.get_tile(row, col + 1).location == 'o'
                    && treasuremap.get_tile(row, col + 1).previous == '\0') {
                        first.discover_tile(row, col + 1);
                        treasuremap.get_tile(row, col + 1).previous = 'w';
                        if (verbose) {
                            cout << "Went ashore at: " << row << "," << col + 1 << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                    //if the hunt to the east is the treasure
                    else if (treasuremap.get_tile(row, col + 1).location == '$') {
                        first.discover_tile(row, col + 1);
                        treasuremap.get_tile(row, col + 1).previous = 'w';
                        if (verbose) {
                            cout << "Went ashore at: " << row << "," << col + 1 << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                }
            }
            //if the hunt is to the south
            else if (capt.get_hunt_order()[i] == 's') {
                //not the bottom row
                if (row + 1 < treasuremap.get_size()) {
                    //if the hunt to the south is water and not been searched
                    if (treasuremap.get_tile(row + 1, col).location == '.'
                    && treasuremap.get_tile(row + 1, col).previous == '\0') {
                        capt.discover_tile(row + 1, col);
                        treasuremap.get_tile(row + 1, col).previous = 'n';
                    }
                    //if the hunt to the south is land and not been searched
                    else if (treasuremap.get_tile(row + 1, col).location == 'o'
                    && treasuremap.get_tile(row + 1, col).previous == '\0') {
                        first.discover_tile(row + 1, col);
                        treasuremap.get_tile(row + 1, col).previous = 'n';
                        if (verbose) {
                            cout << "Went ashore at: " << row + 1 << "," <<
                            col << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                    //if the hunt to the south is the treasure
                    else if (treasuremap.get_tile(row + 1, col).location == '$') {
                        first.discover_tile(row + 1, col);
                        treasuremap.get_tile(row + 1, col).previous = 'n';
                        if (verbose) {
                            cout << "Went ashore at: " << row + 1 << "," <<
                            col << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                }
            }
            //if hunt is to the west
            else {
                //if the column is not the first
                if (col - 1 >= 0) {
                    //if the hunt to the west is water and not been searched
                    if (treasuremap.get_tile(row, col - 1).location == '.'
                    && treasuremap.get_tile(row, col - 1).previous == '\0') {
                        capt.discover_tile(row, col - 1);
                        treasuremap.get_tile(row, col - 1).previous = 'e';
                    }
                    //if the hunt to the west is land and not been searched
                    else if (treasuremap.get_tile(row, col - 1).location == 'o'
                    && treasuremap.get_tile(row, col - 1).previous == '\0') {
                        first.discover_tile(row, col - 1);
                        treasuremap.get_tile(row, col - 1).previous = 'e';
                        if (verbose){
                            cout << "Went ashore at: " << row << "," <<
                            col - 1 << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                    //if the hunt to the west is the treasure
                    else if (treasuremap.get_tile(row, col - 1).location 
                    == '$') {
                        first.discover_tile(row, col - 1);
                        treasuremap.get_tile(row, col - 1).previous = 'e';
                        if (verbose) {
                            cout << "Went ashore at: " << row << "," <<
                            col - 1 << "\n";
                        }
                        fmhunt = fm_hunt();
                    }
                }
            }
            if (fmhunt) {
                break;
            }
        }
        return fmhunt;
    }
    
    bool discover_first(int row, int col) {
        //each of the directions
        for (int i = 0; i < 4; i++) {
            //if hunt is north
            if (first.get_hunt_order()[i] == 'n') {
                if (row - 1 >= 0) {
                    //treasure location
                    if (treasuremap.get_tile(row - 1, col).location == '$') {
                        first.discover_tile(row - 1, col);
                        treasuremap.get_tile(row - 1, col).previous = 's';
                        pair<int, int> yes(row - 1, col);
                        treasurelocation = yes;
                        if (verbose) {
                            cout << "party found treasure at " << row - 1 <<
                            "," << col << ".\n";
                        }
                        return true;
                    }
                    //if land and has not been previously searched
                    else if (treasuremap.get_tile(row - 1, col).location == 'o'
                    && treasuremap.get_tile(row - 1, col).previous == '\0') {
                        first.discover_tile(row - 1, col);
                        treasuremap.get_tile(row - 1, col).previous = 's';
                    }
                }
            }
            //if hunt is east
            else if (first.get_hunt_order()[i] == 'e') {
                if (col + 1 < treasuremap.get_size()) {
                    //if treasure location
                    if (treasuremap.get_tile(row, col + 1).location == '$') {
                        first.discover_tile(row, col + 1);
                        treasuremap.get_tile(row, col + 1).previous = 'w';
                        pair<int, int> yes(row, col + 1);
                        treasurelocation = yes;
                        if (verbose) {
                            cout << "party found treasure at " << row << "," << col + 1 << ".\n";
                        }
                        return true;
                    }
                    //if land and has not been previously searched
                    else if (treasuremap.get_tile(row, col + 1).location == 'o'
                             && treasuremap.get_tile(row, col + 1).previous == '\0') {
                        first.discover_tile(row, col + 1);
                        treasuremap.get_tile(row, col + 1).previous = 'w';
                    }
                }
            }
            //if hunt is south
            else if (first.get_hunt_order()[i] == 's') {
                if (row + 1 < treasuremap.get_size()) {
                    //if treasure location
                    if (treasuremap.get_tile(row + 1, col).location == '$') {
                        first.discover_tile(row + 1, col);
                        treasuremap.get_tile(row + 1, col).previous = 'n';
                        pair<int, int> yes(row + 1, col);
                        treasurelocation = yes;
                        if (verbose) {
                            cout << "party found treasure at " << row + 1 << "," << col << ".\n";
                        }
                        return true;
                    }
                    //if land and has not been previously searched
                    else if (treasuremap.get_tile(row + 1, col).location == 'o'
                             && treasuremap.get_tile(row + 1, col).previous == '\0') {
                        first.discover_tile(row + 1, col);
                        treasuremap.get_tile(row + 1, col).previous = 'n';
                    }
                }
            }
            //if hunt is west
            else {
                if (col - 1 >= 0) {
                    //if treasure location
                    if (treasuremap.get_tile(row, col - 1).location == '$') {
                        first.discover_tile(row, col - 1);
                        treasuremap.get_tile(row, col - 1).previous = 'e';
                        pair<int, int> yes(row, col - 1);
                        treasurelocation = yes;
                        if (verbose) {
                            cout << "party found treasure at " << row << "," << col - 1 << ".\n";
                        }
                        return true;
                    }
                    //if land and has not been previously searched
                    else if (treasuremap.get_tile(row, col - 1).location == 'o'
                             && treasuremap.get_tile(row, col - 1).previous == '\0') {
                        first.discover_tile(row, col - 1);
                        treasuremap.get_tile(row, col - 1).previous = 'e';
                    }
                }
            }
        }
        return false;
    }
        
    void set_start (int row, int col) {
        capt.discover_tile(row, col);
        pair<int, int> start(row, col);
        startinglocation = start;
        if (verbose) {
            cout << "Treasure hunt started at: " << startinglocation.first << "," << startinglocation.second << "\n";
        }
    }

    void set_capt_searchmode(bool defaultsearch) {
        capt.set_searchmode(defaultsearch);
    }
    
    void set_fm_searchmode(bool defaultsearch) {
        first.set_searchmode(defaultsearch);
    }
    
    void set_capt_hunt_order(string &hunt) {
        capt.set_hunt_order(hunt);
    }
    
    void set_fm_hunt_order(string &hunt) {
        first.set_hunt_order(hunt);
    }
    
    void set_verbose(bool verbose_choice) {
        if (verbose_choice) {
            verbose = true;
            return;
        }
        verbose = false;
    }
    
    void set_pathtype(char type, bool map) {
        if (map == true) {
            showPath = true;
            pathtype = type;
            return;
        }
        showPath = false;
        return;
    }
    
    void set_statsmode(bool statsmode) {
        if (statsmode) {
            stats = true;
        }
        return;
    }

    void prepare_map(int size) {
        treasuremap.set_size(size);
    }
    
    //make map from map type
    void make_map(char maptyping, string &data, int row) {
        maptype = maptyping;
        treasuremap.fill_map(data, row);
        int dsize = static_cast<int>(data.size());
        for (int i = 0; i < dsize; i++) {
            if (data[i] == '@') {
                set_start(row, i);
            }
        }
    }
    
    //make map from list type
    void make_list_map(char terrain, int row, int col) {
        maptype = 'L';
        treasuremap.create_tile(terrain, row, col);
        if (terrain == '@') {
            set_start(row, col);
        }
    }

bool is_valid_hunt_order(string &order) {
    if (order.size() != 4) {
        return false;
    }
    else {
        int n;
        int e;
        int s;
        int w;
        for (int i = 0; i < 4; i++) {
            if (order[i] == 'n') {
                n++;
            }
            else if (order[i] == 'e') {
                e++;
            }
            else if (order[i] == 's') {
                s++;
            }
            else if (order[i] == 'w') {
                w++;
            }
            else {
                return false;
            }
        }
        if (n != 1 || e != 1 || s != 1 || w != 1) {
            return false;
        }
        return true;
    }
}

};
int main (int argc, char* argv[]) {
    int option;
    int opt_index = 0;
    struct option longOpts[] = {
        {"captain", required_argument, nullptr, 'c'},
        {"first-mate", required_argument, nullptr, 'f'},
        {"hunt-order", required_argument, nullptr, 'o'},
        {"help", no_argument, nullptr, 'h'},
        {"verbose", no_argument, nullptr, 'v'},
        {"stats", no_argument, nullptr, 's'},
        {"show-path", required_argument, nullptr, 'p'}
        //{ nullptr, 0, nullptr, '\0'}
    }; 
    
    Hunt treasurehunt;
    int pathoption = 0;
    int captcount = 0;
    int firstcount = 0;
    while((option = getopt_long(argc, argv, "c:f:o:hvsp:", longOpts, &opt_index)) != -1) {
        switch (option) {
            // CAPTAIN OPTION
            case 'c':
                if (optarg[0] == '\0') {
                    cerr << "Invalid argument to --captain\n";
                    exit(1);
                }
                else if (captcount >= 1) {
                    cerr << "Invalid argument to --captain\n";
                    exit(1);
                }
                else {
                    string mode(optarg);
                    if (mode != "queue" && mode != "stack") {
                        cerr << "Invalid argument to --captain\n";
                        exit(1);
                    }
                    
                    if (mode == "queue") {
                        treasurehunt.set_capt_searchmode(false);
                    }
                    captcount++;
                }
                break;
                
            
            //first mate
            case 'f':
                if (optarg[0] == '\0') {
                    cerr << "Invalid argument to --first-mate\n";
                    exit(1);
                }
                else if (firstcount >= 1) {
                    cerr << "Invalid argument to --first-mate\n";
                    exit(1);
                }
                else {
                    string mode(optarg);
                    if (mode != "queue" && mode != "stack") {
                        cerr << "Invalid argument to --first-mate\n";
                        exit(1);
                    }
                    
                    if (mode == "stack") {
                        treasurehunt.set_fm_searchmode(false);
                    }
                    firstcount++;
                }
                break;
            
            //help
            case 'h':
                cout << "Try using c, f, o, v, s, p\n";
                exit(1);

            //hunt order    
            case 'o':
                if (optarg[0] == '\0') {
                    cerr << "Invalid argument to --hunt-order\n";
                    exit(1);
                }
                else {
                    string huntstring(optarg);
                    if (!is_valid_hunt_order(huntstring)) {
                        cerr << "Invalid argument to --hunt-order\n";
                        exit(1);
                    }
                    treasurehunt.set_fm_hunt_order(huntstring);
                    treasurehunt.set_capt_hunt_order(huntstring);
                }
                break;

            //verbose
            case 'v':
                treasurehunt.set_verbose(true);
                break;
               
            //stats
            case 's':
                treasurehunt.set_statsmode(true);
                break;
                
            //show path
            case 'p':
                if (optarg[0] == '\0' || (optarg[0] != 'L' && optarg[0] != 'M')) {
                    cerr << "Invalid argument to --show-path\n";
                    exit(1);
                }
                if (pathoption >= 1) {
                    cerr << "Invalid argument to --show-path\n";
                    exit(1);
                }
                else if (optarg[0] == 'M') {
                    treasurehunt.set_pathtype('M', true);
                }
                else {
                    treasurehunt.set_pathtype('L', true);
                }
                pathoption++;
                break;
                
            default:
                cerr << "Unknown option\n";
                exit(1);
            } // switch
    }
        // optarg global varial with option arguments, --captain stack makes optarg stack as a char*
        // optarg = string(optarg);
        string junk;
        while (std::cin) {
            getline(std::cin, junk);
            if (junk[0] == '#') {
                junk.clear();
            }
            else {
                break;
            }
        }
        
        maptype = junk[0];
        string size;
        getline(std::cin, size);
        int mapsize = stoi(size);
        treasurehunt.prepare_map(mapsize);
        
        //make map based on map type
        if (maptype == 'M') {
            int i = 0;
            string mapdata;
            //continue reading data
            while (cin) {
                if (i == mapsize) {
                    break;
                }
                getline(cin, mapdata);
                treasurehunt.make_map('M', mapdata, i);
                i++;
                mapdata.clear();
            }
        }
        //make map based on list type
        else {
            cin.clear();
            char terrain = '\0';
            int row;
            int col;
            while (cin >> row >> col >> terrain) {
                treasurehunt.make_list_map(terrain, row, col);
            }
        }
    //start looking for treasure
    treasurehunt.treasurehunting();
    
    return(1);
    };
