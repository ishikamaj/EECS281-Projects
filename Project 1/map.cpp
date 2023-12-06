// PROJECT IDENTIFIER: 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD

#include <iostream>
#include <vector>

using namespace std;

struct Tile {
    char location = '.';
    char previous = '\0';
};

class Map {
private:
    vector<vector<Tile>> map;
    int size;

public:
    
    Map() : size(0) {}
    
    //getters
    int get_size() {
        return size;
    }

    Tile& get_tile(int row, int col) {
        return map[row][col];
    }

    //setter
    void set_size(int size_in) {
        size = size_in;
        map.resize(size_in, vector<Tile>(size));
    }

    void create_tile(char terrain, int row, int col) {
        map[row][col].location = terrain;
    }
    
    void fill_map(string &data, int row) {
        for (int j = 0; j < size; j++)
        {
            create_tile(data[j], row, j);
        }
    }    
    
    void print_map() {
        int map_size = size;
        for (int i = 0; i < map_size; i++) {
            for (int j = 0; j < map_size; j++) {
                cout << map[i][j].location;
            }
            cout << "\n";
        }
    }

};
