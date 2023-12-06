// PROJECT IDENTIFIER: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <getopt.h>
#include "starwars.h"
#include "P2random.h"

Options::Options() {
    verbose = false;
    median = false;
    generalEval = false;
    watcher = false;
}

Options::Options(bool v, bool m, bool g, bool w) {
    verbose = v;
    median = m;
    generalEval = g;
    watcher = w;
}

Input::Input() { 
    mode = 1; 
    numGenerals = 0; 
    numPlanets = 0; 
}

Battalion::Battalion() {
    force = 0;
    numTroops = 0;
    time = 0;
    id = 0;
    general = 0;
}

Battalion::Battalion(uint32_t f, uint32_t n, uint32_t t, uint32_t i, uint32_t g) {
    force = f;
    numTroops = n;
    time = t;
    id = i;
    general = g;
}

General::General() {
    numJedi = 0;
    numSith = 0;
    survivors = 0;
}

//initialize the galaxy
Galaxy::Galaxy(uint32_t numPlanets, uint32_t numGenerals) {
    planets.reserve(numPlanets);
    planets.resize(numPlanets);
    std::fill(begin(planets), end(planets), Planet());

    generals.reserve(numGenerals);
    generals.resize(numGenerals);
    std::fill(begin(generals), end(generals), General());

    planetsAtWar.reserve(numPlanets);
    planetsAtWar.resize(numPlanets);
    std::fill(begin(planetsAtWar), end(planetsAtWar), '0');
}

//set up the troops
void Galaxy::deployTroops(uint32_t planetID, uint32_t generalID, 
                          const Battalion& newDeployment, bool jediSide) {
    if (jediSide) {
        planets[planetID].jedi.push(newDeployment);
        generals[generalID].numJedi += newDeployment.numTroops;
        generals[generalID].survivors += newDeployment.numTroops;
    }
    else {
        planets[planetID].sith.push(newDeployment);
        generals[generalID].numSith += newDeployment.numTroops;
        generals[generalID].survivors += newDeployment.numTroops;
    }
}

//update the states of the watcher
void Galaxy::updateWatcher(const Battalion& newDeployment, uint32_t planetID, bool jediSide) {
    //attack states
    if (planets[planetID].attack.state == '1') {
        if (jediSide) {
            planets[planetID].attack.jedi = newDeployment;
            planets[planetID].attack.state = '2';
        }
    }
    else if (planets[planetID].attack.state == '2') {
        if (jediSide) {
            if (newDeployment.force < planets[planetID].attack.jedi.force) {
                planets[planetID].attack.jedi = newDeployment;
            }
        }
        else {
            if (newDeployment.force >= planets[planetID].attack.jedi.force) {
                planets[planetID].attack.sith = newDeployment;
                planets[planetID].attack.state = '3';
            }
        }
    }
    else if (planets[planetID].attack.state == '3') {
        if (jediSide) {
            if (newDeployment.force < planets[planetID].attack.jedi.force) {
                planets[planetID].attack.temp = newDeployment;
                planets[planetID].attack.state = '4';
            }
        }
        else {
            if (newDeployment.force > planets[planetID].attack.sith.force) {
                planets[planetID].attack.sith = newDeployment;
            }
        }
    }
    else {
        if (jediSide) {
            if (newDeployment.force < planets[planetID].attack.temp.force) {
                planets[planetID].attack.temp = newDeployment;
            }
        }
        else {
            if (signed(newDeployment.force) - signed(planets[planetID].attack.temp.force) > 
                signed(planets[planetID].attack.sith.force) - signed(planets[planetID].attack.jedi.force)) {
                planets[planetID].attack.sith = newDeployment;
                planets[planetID].attack.jedi = planets[planetID].attack.temp;
                planets[planetID].attack.state = '3';
            }
        }
    }

    //ambush states
    if (planets[planetID].ambush.state == '1') {
        if (!jediSide) {
            planets[planetID].ambush.sith = newDeployment;
            planets[planetID].ambush.state = '2';
        }
    }
    else if (planets[planetID].ambush.state == '2') {
        
        if (!jediSide) {
            if (newDeployment.force > planets[planetID].ambush.sith.force) {
                planets[planetID].ambush.sith = newDeployment;
            }
        }
        else {
            if (newDeployment.force <= planets[planetID].ambush.sith.force) {
                planets[planetID].ambush.jedi = newDeployment;
                planets[planetID].ambush.state = '3';
            }
        }
    }
    else if (planets[planetID].ambush.state == '3') {
        if (!jediSide) {
            if (newDeployment.force > planets[planetID].ambush.sith.force) {
                planets[planetID].ambush.temp = newDeployment;
                planets[planetID].ambush.state = '4';
            }
        }
        else {
            if (newDeployment.force < planets[planetID].ambush.jedi.force) {
                planets[planetID].ambush.jedi = newDeployment;
            }
        }
    }
    else {
        if (!jediSide) {
            if (newDeployment.force > planets[planetID].ambush.temp.force) {
                planets[planetID].ambush.temp = newDeployment;
            }
        }
        else {
            if (signed(planets[planetID].ambush.temp.force) - signed(newDeployment.force) > 
                signed(planets[planetID].ambush.sith.force) - signed(planets[planetID].ambush.jedi.force)) {
                planets[planetID].ambush.jedi = newDeployment;
                planets[planetID].ambush.sith = planets[planetID].ambush.temp;
                planets[planetID].ambush.state = '3';
            }
        }
    }
}

void Galaxy::battle(uint32_t planetID, const Options& userOptions) {
    if (planets[planetID].jedi.empty() || planets[planetID].sith.empty()) {
        return;
    }
        
    Battalion jedi = planets[planetID].jedi.top();
    Battalion sith = planets[planetID].sith.top();

    //no battle if jedi force is more than sith
    if (jedi.force > sith.force) {
        return;
    }
    
    bool atWar = true;
    uint32_t deaths = 0;
    uint32_t totalDeaths = 0;
    uint32_t temp = 0;
    int diff = 0;
    planetsAtWar[planetID] = '1';

    while (atWar) {
        ++numBattles;
        deaths = jedi.numTroops < sith.numTroops ? jedi.numTroops : sith.numTroops;
        totalDeaths = deaths * 2;

        jedi.numTroops -= deaths;
        planets[planetID].jedi.pop();

        //still some jedi remaining
        if (jedi.numTroops != 0) {
             planets[planetID].jedi.push(jedi);
        }

        sith.numTroops -= deaths;
        planets[planetID].sith.pop();

        //still some sith remaining
        if (sith.numTroops != 0)
            planets[planetID].sith.push(sith);
        
        generals[jedi.general].survivors -= deaths;
        generals[sith.general].survivors -= deaths;

        if (userOptions.verbose) {
            std::cout << "General " << sith.general << "'s battalion attacked General ";
            std::cout << jedi.general << "'s battalion on planet " << planetID << ". ";
            std::cout << totalDeaths << " troops were lost.\n";
        }

        if (userOptions.median) {
            if (planets[planetID].largest.empty() && planets[planetID].smallest.empty()) {
                planets[planetID].smallest.push(totalDeaths);
            }

            else if (totalDeaths > planets[planetID].smallest.top()) {
                planets[planetID].largest.push(totalDeaths);
            }
            
            else {
                planets[planetID].smallest.push(totalDeaths);
            }

            diff = (signed(planets[planetID].smallest.size()) - 
                    signed(planets[planetID].largest.size()));

            if (diff > 1) {
                temp = planets[planetID].smallest.top();
                planets[planetID].smallest.pop();
                planets[planetID].largest.push(temp);
            }

            else if (diff < -1) {
                temp = planets[planetID].largest.top();
                planets[planetID].largest.pop();
                planets[planetID].smallest.push(temp);
            }
        }
        
        //no troops left to battle
        if (planets[planetID].jedi.empty() || planets[planetID].sith.empty()) {
            atWar = false;
        }
        else {
            jedi = planets[planetID].jedi.top();
            sith = planets[planetID].sith.top();
            if (jedi.force > sith.force) {
                atWar = false;
            }
        }
    }
}

void Galaxy::printMedian(uint32_t time) {
    uint32_t median = 0;

    for (uint32_t i = 0; i != planetsAtWar.size(); ++i) {
        if (planetsAtWar[i] == '1') {
            if (planets[i].smallest.size() == planets[i].largest.size()) {
                median = ((planets[i].smallest.top() + planets[i].largest.top()) / 2);
            }

            else {
                median = (planets[i].smallest.size() > planets[i].largest.size() ? 
                          planets[i].smallest.top() : planets[i].largest.top());
            }
            
            std::cout << "Median troops lost on planet " << i << " at time " << time;
            std::cout << " is " << median << ".\n";
        }
    }
}

void Galaxy::printGeneralEval() {
    std::cout << "---General Evaluation---\n";

    for (uint32_t i = 0; i < generals.size(); ++i) {
        std::cout << "General " << i << " deployed " << generals[i].numJedi;
        std::cout << " Jedi troops and " << generals[i].numSith << " Sith troops, and ";
        std::cout << generals[i].survivors << "/" << generals[i].numJedi + generals[i].numSith;
        std::cout << " troops survived.";

        if (i == generals.size() - 1) {
            std::cout << std::endl;
        }
        else {
            std::cout << "\n";
        }
    }
}

void Galaxy::printMovieWatcher() {
    std::cout << "---Movie Watcher---\n";

    for (uint32_t i = 0; i < planets.size(); ++i) {
        if (planets[i].ambush.state == '1' || planets[i].ambush.state == '2') {
            std::cout << "A movie watcher would enjoy an ambush on planet " << i;
            std::cout << " with Sith at time -1 and Jedi at time -1 with a force";
            std::cout << " difference of 0.\n";
        }
        else {
            std::cout << "A movie watcher would enjoy an ambush on planet " << i;
            std::cout << " with Sith at time " << planets[i].ambush.sith.time;
            std::cout << " and Jedi at time " << planets[i].ambush.jedi.time;
            std::cout << " with a force difference of ";
            std::cout << (planets[i].ambush.sith.force - planets[i].ambush.jedi.force);
            std::cout << ".\n";
        }

        if (planets[i].attack.state == '1' || planets[i].attack.state == '2') {
            std::cout << "A movie watcher would enjoy an attack on planet " << i;
            std::cout << " with Jedi at time -1 and Sith at time -1 with a force";
            std::cout << " difference of 0.";
        }
        else {
            std::cout << "A movie watcher would enjoy an attack on planet " << i;
            std::cout << " with Jedi at time " << planets[i].attack.jedi.time;
            std::cout << " and Sith at time " << planets[i].attack.sith.time;
            std::cout << " with a force difference of ";
            std::cout << (planets[i].attack.sith.force - planets[i].attack.jedi.force);
            std::cout << ".";
        }
        if (i + 1 != planets.size()) {
            std::cout << "\n";
        }
        else {
            std::cout << std::endl;
        }
    }
}

Options getOptions(int argc, char* argv[]) {
    opterr = false;
    int choice, option_index = 0;
    Options userOptions;
    
    option long_options[] = {{ "verbose",      no_argument, nullptr, 'v'  },
                             { "median",       no_argument, nullptr, 'm'  },
                             { "general-eval", no_argument, nullptr, 'g'  },
                             { "watcher",      no_argument, nullptr, 'w'  }};
    
    while ((choice = getopt_long(argc, argv, "vmgw", long_options, &option_index)) != -1) {
        switch (choice) {
            case 'v': {
                userOptions.verbose = true;
                break;
            }
            
            case 'm': {
                userOptions.median = true;
                break;
            }

            case 'g': {
                userOptions.generalEval = true;
                break;
            }

            case 'w': {
                userOptions.watcher = true;
                break;
            }

            default: {
                std::cerr << "Unknown command line option" << std::endl;
                exit(1);
            }
        }
    }
    return userOptions;
}

Input getInput() {
    Input userInput;
    std::string junk, mode;

    std::getline(std::cin, junk);
    std::cin >> junk >> mode >> std::ws;
    std::cin >> junk >> userInput.numGenerals >> std::ws;
    std::cin >> junk >> userInput.numPlanets >> std::ws;

    if (mode == "PR") {
        userInput.mode = 0;
    }
    
    return userInput;
}

std::stringstream getRandom(const Input& userInput) {
    std::stringstream ss;

    if (userInput.mode) {
        return ss;
    }

    std::string junk;
    uint32_t seed;
    uint32_t numDeploy;
    uint32_t rate;

    std::cin >> junk >> seed >> std::ws;
    std::cin >> junk >> numDeploy >> std::ws;
    std::cin >> junk >> rate >> std::ws;

    P2random::PR_init(ss, seed, userInput.numGenerals, 
                      userInput.numPlanets, numDeploy, rate);
    return ss;
}

//the actual game
void galacticWarfare(const Options& userOptions, const Input& userInput) {
    char junk;
    bool jediSide;
    std::string jediOrSith;
    int32_t tempForce;
    int32_t tempTroops;
    uint32_t prevTime = 0;
    uint32_t currentTime;
    uint32_t planetID;
    uint32_t generalID;
    uint32_t battalionID = 0;
    uint32_t numberOfTroops;
    uint32_t forceSensitivity;
    
    Galaxy galaxy(userInput.numPlanets, userInput.numGenerals);

    std::stringstream ss = getRandom(userInput);
    std::istream& is = userInput.mode ? std::cin : ss;

    std::cout << "Deploying troops...\n";

    while (is >> currentTime >> jediOrSith >> junk >> generalID >> junk >> planetID
              >> junk >> tempForce >> junk >> tempTroops >> std::ws) {

        //errors
        if (currentTime < prevTime) {
            std::cerr << "Invalid decreasing timestamp" << std::endl;
            exit(1);
        }
        if (generalID >= userInput.numGenerals) {
            std::cerr << "Invalid general ID" << std::endl;
            exit(1);
        }
        if (planetID >= userInput.numPlanets) {
            std::cerr << "Invalid planet ID" << std::endl;
            exit(1);
        }
        if (tempForce <= 0) {
            std::cerr << "Invalid force sensitivity level" << std::endl;
            exit(1);
        }
        if (tempTroops <= 0) {
            std::cerr << "Invalid number of troops" << std::endl;
            exit(1);
        }

        forceSensitivity = unsigned(tempForce);
        numberOfTroops = unsigned(tempTroops);

        if (userOptions.median && currentTime != prevTime && battalionID != 0) {
            galaxy.printMedian(prevTime);
        }

        if (jediOrSith[0] == 'J') {
            jediSide = 1;
        }
        else {
            jediSide = 0;
        }
        
        Battalion newDeployment(forceSensitivity, numberOfTroops, 
                                currentTime, battalionID, generalID);

        galaxy.deployTroops(planetID, generalID, newDeployment, jediSide);

        if (userOptions.watcher) {
            galaxy.updateWatcher(newDeployment, planetID, jediSide);
        }

        galaxy.battle(planetID, userOptions);

        prevTime = currentTime;
        ++battalionID;
    }

    if (battalionID != 0) {
        if (userOptions.median) {
            galaxy.printMedian(prevTime);
        }

        std::cout << "---End of Day---\n";
        std::cout << "Battles: " << galaxy.getNumBattles() << std::endl;

        if (userOptions.generalEval) {
            galaxy.printGeneralEval();
        }
        
        if (userOptions.watcher) {
            galaxy.printMovieWatcher();
        }
    }
}