// PROJECT IDENTIFIER: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#ifndef STAR_WARS_H
#define STAR_WARS_H

#include <sstream>
#include <vector>
#include <queue>

struct Options {
   bool verbose, median, generalEval, watcher; 
   Options();
   Options(bool v, bool m, bool g, bool w);
};

struct Input {
    uint32_t numGenerals;
    uint32_t numPlanets;

    // DL = 1, PR = 0.
    bool mode; 
    Input();
};

struct Battalion {
    uint32_t force;
    uint32_t numTroops;
    uint32_t time;
    uint32_t id;
    uint32_t general;
    Battalion();
    Battalion(uint32_t f, uint32_t n, uint32_t t, uint32_t i, uint32_t g);
};

struct General {
    uint32_t numJedi;
    uint32_t numSith;
    uint32_t survivors;
    General();
};

struct Watcher {
    Battalion jedi;
    Battalion sith;
    Battalion temp;
    // 1 = initial, 2 = seen_one, 3 = seen_both, 4 = maybe_better.
    char state = '1'; 
};

//compare 2 jedis
struct compareJedi {
    bool operator() (const Battalion& j1, const Battalion& j2) {
        if (j1.force != j2.force) {
            return j1.force > j2.force;
        }
        else {
            return j1.id > j2.id;
        }
    }
};

//compare 2 siths
struct compareSith {
    bool operator() (const Battalion& s1, const Battalion& s2) {
        if (s1.force != s2.force) {
            return s1.force < s2.force;
        }
        else {
            return s1.id > s2.id;
        }
    }
};

struct minHeap {
    bool operator() (uint32_t x, uint32_t y) {
        return x > y;
    }
};

struct Planet {
    std::priority_queue<Battalion, std::vector<Battalion>, compareJedi> jedi;
    std::priority_queue<Battalion, std::vector<Battalion>, compareSith> sith;
    std::priority_queue<uint32_t, std::vector<uint32_t>, minHeap> largest;
    std::priority_queue<uint32_t> smallest;
    Watcher attack;
    Watcher ambush;
    Planet() {}
};

class Galaxy {
private:
    std::vector<Planet> planets;
    std::vector<General> generals;
    std::vector<char> planetsAtWar;
    uint32_t numBattles = 0;

public:
    Galaxy() {}
    Galaxy(uint32_t numPlanets, uint32_t numGenerals);
    void deployTroops(uint32_t planetID, uint32_t generalID, 
                      const Battalion& newDeployment, bool jediSide);
    
    void battle(uint32_t planetID, const Options& userOptions);

    uint32_t getNumBattles() { 
        return numBattles; 
    }

    void updateWatcher(const Battalion& newDeployment, uint32_t planetID, bool side);
    void printMedian(uint32_t time);
    void printGeneralEval();
    void printMovieWatcher();
};

Options getOptions(int argc, char* argv[]);
Input getInput();
std::stringstream getRandom(const Input& userInput);
void galacticWarfare(const Options& userOptions, const Input& userInput);
#endif