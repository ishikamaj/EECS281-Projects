// PROJECT IDENTIFIER: 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD

#include <stack>
#include <queue>
#include <deque>
#include <vector>
#include <string>
#include <utility>

using namespace std;

class Firstmate {
private:
    string hunt_order;
    bool default_search;
    int ashore;
    int tiles_investigated;
    deque<pair<int, int>> deck;
    pair<int, int> current_location;

public:
    Firstmate() {
        hunt_order = "nesw";
        ashore = 0;
        tiles_investigated = 0;
        default_search = true;
    }

    //getters
    string get_hunt_order() const {
        return hunt_order;
    } 

    int get_tiles_investigated() {
        return tiles_investigated;
    }

    int get_wentashore() {
        return ashore;
    } 
    
    pair<int, int> get_current_location() {
        return current_location;
    }

    //setters
    void set_searchmode(bool searchmode) {
        default_search = searchmode;
    }
    
    void set_hunt_order(string &order) {
        hunt_order = order;
    } 

    void set_current_location (pair<int, int> &loc) {
        current_location = loc;
        tiles_investigated++;
    }
    
    void increment_wentashore() {
        ++ashore;
    }
    
    void increment_tilesinvestigated() {
        ++tiles_investigated;
    }  
    
    void discover_tile(int row, int col) {
        pair<int, int> tile(row, col);
        deck.push_back(tile);
 
    }
    
    void investigate() {
        if (default_search) {
            set_current_location(deck.front());
            deck.pop_front();
        }
        else {
            set_current_location(deck.back());
            deck.pop_back();
        }
    }
    
    bool deck_empty() {
        if (deck.empty()) {
            return true;
        }
        return false;
    }
      
};
