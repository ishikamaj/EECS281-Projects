// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9
//./letter --begin ship -e shot --stack -c -o W < test-1-ship-shot-scw.txt > test-1-out.txt

#include <vector>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <string>    
#include <deque>
#include <cmath>
#include <chrono>

using namespace std;

struct word {
    string wordIn = "";
    int discoveredBy = -1;
};

class letterMan {

private:

    //in the command line
    bool output = false;
    bool begin = true;
    bool end = true;
    char outputFormat = '\0';
    bool q = false;
    bool s = false;
    bool changeBool = false;
    bool swapBool = false;
    bool lengthBool = false;

    string startWord = "";
    string endWord = "";
    size_t startLen = 0;

public:
    string sortedStart = "";
    bool startExist = false;
    bool endExist = false;

    vector<string> dictionary;
    size_t size = 0;

    vector<word> discard;
    int discardSize = 0;
    
    deque<word> queue;
    int queueSize = 0;
    
    word currentWord;

    bool checkLetters(string);
    bool checkWord(const string &);
    bool checkEnds(const string & temp);
    string different(const string&, const string &);

    //Reads in txt file
    void read_in();
    //Command Line Arguments
    void get_options(int argc, char** argv);
    //Finds the path
    bool find_path();
    //print to cout
    void write();
    //Compares to see if can be marked as discovered
    bool compare(const string & current, const string & reviewed);

};

void letterMan::get_options(int argc, char** argv) {
    int option_index = 0;
    int option = 0;
    
    // no error for options
    opterr = false;

    int sqNum = 0;

    //command line options
    struct option longOpts[] = { { "stack", no_argument, nullptr, 's' },
                                { "queue", no_argument, nullptr, 'q' },
                                { "begin", required_argument, nullptr, 'b'},
                                { "end", required_argument, nullptr, 'e'},
                                { "change", no_argument, nullptr, 'c' },
                                { "swap", no_argument, nullptr, 'p'},
                                { "length", no_argument, nullptr, 'l'},
                                { "output", required_argument, nullptr, 'o'},
                                { "help", no_argument, nullptr, 'h'},
                                { nullptr, 0, nullptr, '\0' } };

    while ((option = getopt_long(argc, argv, "sqcplo:b:e:h", 
    longOpts, &option_index)) != -1) {
        switch (option) {
        //stack
        case 's':
            sqNum++;
            s = true;
            break;
        //queue
        case 'q':
            sqNum++;
            q = true;
            break;
        //beginning word
        case 'b':
            startWord = optarg;
            if (startWord.substr(0, 1) != "-") {
                startLen = startWord.size();
                sortedStart = startWord;
                sort(sortedStart.begin(), sortedStart.end());
                break;
            }
            else {
                begin = false;
                break;
            }
            break;
        //ending word
        case 'e':
            endWord = optarg;
            if (endWord.substr(0, 1) != "-") {
                break;
            }
            else {
                end = false;
                break;
            }
            break;
        //change letters
        case 'c':
            changeBool = true;
            break;
        //swap 2 letters that are next to each other
        case 'p':
            swapBool = true;
            break;
        //change length of word by inserting/deleting
        case 'l':
            lengthBool = true;
            break;
        //output format
        case 'o':
            output = true;
            outputFormat = *optarg;
            break;
        //help
        case 'h':
            std::cerr << "This program reads in a dictionary from a file.\n"
                << "It runs takes in an end word as an initial argument\n"
                << "and generates a path from the beginning word to the end \n"
                << "word according to the provided arguemnts."
                << "Usage: \'./letter\n\t[--stack | -s]\n"
                << "\t[--queue | -q]\n"
                << "\t[--begin | -b] <word>\n"
                << "\t[--end | -e] <word>\n"
                << "\t[--change | -c]\n"
                << "\t[--swap | -p]\n"
                << "\t[--length | -l]\n"
                << "\t[--output | -o] (W|M)\n"
                << "\t[--help | -h]\n"
                << "\t<Dictionary File>\'" << std::endl;
            exit(1);
        }
    }

    //only one s or q
    if (sqNum != 1) {
        cerr << "More or less than one -s or -q on command line.\n";
        exit(1);
    }
       
    if (!begin) {
        cerr << "No begin word specified.\n";
        exit(1);
    }

    if (!end) {
        cerr << "No end word specified.\n";
        exit(1);
    }

    if (!changeBool && !swapBool && !lengthBool) {
        cerr << "No -c, -l, -s on command line.\n";
        exit(1);
    }

    if (!(outputFormat == 'W' || outputFormat == 'M') && output) {
        cerr << "Output Format not specified.\n";
        exit(1);
    }
    
    if (startWord.empty() || endWord.empty()) {
        cerr << "Start word or end word not specified.\n";
        exit(1);
    }

    if (startWord.size() != endWord.size() && !lengthBool) {
        cerr << "Cannot modify length. Impossible case.\n";
        exit(1);
    }

    if (startWord == endWord) {
        cout << "Words in morph: 1\n";
        cout << startWord << "\n";
        exit(0);
    }
}

void reverse(string &string_in) {
    string reversed = "";
    size_t size = string_in.size();
    for (size_t i = 0; i < size; ++i) {
        reversed += string_in[size - 1 - i];
    }
    string_in = reversed;
}

bool letterMan::checkLetters(string check) {
    sort(check.begin(), check.end());
    return sortedStart == check;    
}

bool letterMan::checkWord(const string &word) {
    //check if word even needs adding to the dictionary
    if (!lengthBool) {

        if (startLen != word.size()) {
            return false;
        }
        else if (!changeBool) {
            if (checkLetters(word))
                return true;
            else
                return false;
        }
        else {
            return true;
        }
    }
    return true;
}

bool letterMan::checkEnds(const string &ends) {
    if (ends == startWord) {
        startExist = true;
        return true;
    }
    else if (ends == endWord)
        endExist = true;
    return false;
}

void letterMan::read_in() {
    char type = '\0';
    cin >> type;
    //simple
    if (type == 'S') {
        string numIn = "";
        cin >> numIn;
        const char* dictSize = { numIn.c_str() };
        size = atoi(dictSize);

        dictionary.reserve(size-1);
        cin.ignore();

        string temp = "";
        for (size_t i = 0; i < size; ++i) {
            cin >> temp;
            if (temp.substr(0, 2) == "//") {
                getline(cin, temp);
                --i;
            }
            else if (temp.size() == 0) {
                --i;
            }
            else{
              
                if (checkWord(temp)) {
                    if (!checkEnds(temp)) {
                        dictionary.push_back(temp);
                    }
                }
            }// end of else
        }//end of for loop
        --size;
    }
    //complex
    else {
        string numIn = "";
        cin >> numIn;
        const char* dictSize = { numIn.c_str() };
        size = atoi(dictSize);

        dictionary.reserve(size);
        discard.reserve(size / 4);

        cin.ignore();
        string temp = "";

        string reversal = "&";
        string insert = "[]";
        string swap = "!";
        string doubleLetter = "?";
        const string chars = "&[]!?";

        for (size_t i = 0; i < size; ++i) {
            cin >> temp;
            if (temp.substr(0, 2) == "//") {
                getline(cin, temp);
                --i;
            }            
            else {
                size_t index = temp.find_first_of(chars);
                if (index != string::npos) {
                    if (temp[index] == '&') {
                        string toReverse = temp.substr(0, index);
                        if (checkWord(toReverse)) {
                            if (!checkEnds(toReverse)) {
                                dictionary.push_back(toReverse);
                            }
                            reverse(toReverse);
                            
                            if (!checkEnds(toReverse)) {
                                dictionary.push_back(toReverse);
                            }
                        }
                    } //end of if statement for reversal
                    else if (temp[index] == '[') {
                        size_t endIndex = temp.find(']');

                        for (size_t i = index + 1; i < endIndex; ++i) {
                            string temp1 = temp.substr(0, index) + temp[i] + 
                            temp.substr(endIndex + 1, temp.size() - 
                            endIndex - 1);
                            if (checkWord(temp1)) {
                                if (!checkEnds(temp1)) {
                                    dictionary.push_back(temp1);
                                }
                            }
                        } //end of for loop
                    } //end of else if for inseret
                    else if (temp[index] == '!') {
                        size_t size = temp.size();
                        string temp1 = temp.substr(0, index) + 
                        temp.substr(index + 1, size-index-1);
                        if (checkWord(temp1)) {
                            if (!checkEnds(temp1)) {
                                dictionary.push_back(temp1);
                            }
                            temp1[index - 2] = temp[index - 1];
                            temp1[index - 1] = temp[index - 2];
                            if (!checkEnds(temp1)) {
                                dictionary.push_back(temp1);
                            }
                        }
                    } //end of else if for swap
                    else if (temp[index] == '?') {
                        size_t size = temp.size();
                        string temp1 = temp.substr(0, index) + 
                        temp.substr(index + 1, size - index - 1);
                        string temp2 = temp.substr(0, index) + 
                        temp[index - 1] + temp.substr(index + 1, 
                        size - index - 1);
                        if (checkWord(temp1)) {
                            if (!checkEnds(temp1)) {
                                dictionary.push_back(temp1);
                            }
                        }
                        if (checkWord(temp2)) {
                            if (!checkEnds(temp2)) {
                                dictionary.push_back(temp2);
                            }
                        }
                    } // end of if else for double letters
                } //end of if to test for the special characters
                else {
                    if (checkWord(temp)) {
                        if (!checkEnds(temp)) {
                            dictionary.push_back(temp);
                        }
                    }
                } //end of else for no special characters    
            }
        }
        size = dictionary.size();
    }
}


bool letterMan::compare(const string & current, const string & reviewed) {

    size_t currentSize = current.size();
    size_t reviewedSize = reviewed.size();

    //check if differ
    if (reviewedSize > currentSize && reviewedSize - currentSize > 1) {
        return false;
    }
    else if (currentSize > reviewedSize && currentSize - reviewedSize > 1) {
        return false;
    }

    //If length is the same, change and swap
    else if (currentSize == reviewedSize) {
        if (changeBool && swapBool) {
            bool changed = false;
            bool swapped = false;

            for (size_t i = 0; i < currentSize; ++i) {
                //If two letters are different
                if (current[i] != reviewed[i]) {
                    //If not at the end
                    if (i != currentSize - 1) {
                        //check if already been swapped or changed
                        if (!swapped && !changed) {
                            //If swap works
                            if (current[i] == reviewed[i + 1] && 
                            current[i + 1] == reviewed[i]) {
                                swapped = true;
                                ++i;
                            }
                            //If swap doesn't work, change
                            else {
                                changed = true;
                            }
                        }
                        else
                            return false;
                    }
                    //Change
                    else {
                        //check if already been swapped or changed
                        if (!swapped && !changed) {
                            changed = true;
                        }
                        else {
                            return false;
                        }
                    } //end of else for change
                } //end of if current == reviewed
            } //end of for loop
            return true;
        }
        else if (changeBool) {

            bool changed = false;

            for (size_t i = 0; i < currentSize; ++i) {
                //If two letters are different
                if (current[i] != reviewed[i]) {
                    //check if already been changed
                    if (!changed) {
                        changed = true;
                    }
                    else {
                        return false;
                    }
                }
            } //end of foor loop
            return true;
        } //end of else if for change
        else if (swapBool) {
            bool swapped = false;

            for (size_t i = 0; i < currentSize; ++i) {
                //If two letters are different
                if (current[i] != reviewed[i]) {
                    //check if already been swapped or changed
                    if (!swapped) {
                        //If not at the end
                        if (i != currentSize - 1) {
                            //If swap works
                            if (current[i] == reviewed[i + 1] && current[i + 1] == reviewed[i]) {
                                swapped = true;
                                ++i;
                            }
                            else {
                                return false;
                            }
                        }
                        else {
                            return false;
                        }
                    }
                    else {
                        return false;
                    }
                } // end of if
            }
            return true;

        }
        else {
            return false;
        }
    }
    //If length is different by lengthcheck
    else {        
        bool lengthed = false;

        if (currentSize > reviewedSize) {
            //check for differences
            int j = 0;
            for (size_t i = 0; i < currentSize; ++i) {
                if (current[i] != reviewed[j]) {
                    //check if already changed
                    if (!lengthed) {
                        if (current[i + 1] == reviewed[j]) {
                            lengthed = true;
                            ++i;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                } //end of if
                ++j;
            } //end of for loop
            return true;
        }
        else {
            //check for differences
            int j = 0;
            for (size_t i = 0; i < reviewedSize; ++i) {
                if (current[j] != reviewed[i]) {
                    //check if already changed
                    if (!lengthed) {
                        if (current[j] == reviewed[i + 1]) {
                            lengthed = true;
                            ++i;
                        }
                        else {
                            return false;
                        }
                    }
                    else {
                        return false;
                    }
                }
                j++;
            }
            return true;
        }
    }
    return true;
}

bool letterMan::find_path() {
   
    currentWord.wordIn = startWord;

    do {
        //After discovering all words for a particular word
        if (queueSize > 0) {

            //Add to discard
            discard.push_back(currentWord);
            ++discardSize;
            
            currentWord = queue.at(0);
            queue.pop_front();
            --queueSize;

        }

        for (size_t i = 0; i < size; ++i) {
            //check if word has been discovered already
            if (dictionary[i].empty()) {}

            else {
                if (compare(currentWord.wordIn, dictionary.at(i))) {
                    
                    word temp;
                    temp.wordIn = dictionary.at(i);
                    temp.discoveredBy = discardSize;

                    if (endWord == dictionary.at(i)) {
                        
                        discard.push_back(currentWord);
                        discard.push_back(temp);
                        discardSize += 2;
                        return true;
                    
                    }
                    //if queue, push to back
                    if (q) {
                        queue.push_back(temp);
                        ++queueSize;
                    }
                    //otherwise it's a stack, so push to the front
                    else {
                        queue.push_front(temp);
                        ++queueSize;
                    }
                    
                    dictionary[i] = std::string();
                }
            }
        }
            
     
    } while (queueSize > 0);
    return false;
}

string letterMan::different(const string& current, const string& reviewed) {
    size_t currentSize = current.size();
    size_t reviewedSize = reviewed.size();
    
    if (currentSize == reviewedSize) {
        for (size_t i = 0; i < currentSize; ++i) {
            if (current[i] != reviewed[i]) {
                if (i == currentSize - 1)
                    return "c," + to_string(i) + "," + current[i];
                else {
                    if (current[i + 1] != reviewed[i + 1])
                        return "s," + to_string(i);
                    else
                        return "c," + to_string(i) + "," + current[i];
                }
            }
        }
    }
    else {
        if (currentSize > reviewedSize) {
            for (size_t i = 0; i < reviewedSize; ++i) {
                if (current[i] != reviewed[i])
                    return "i," + to_string(i) + "," + current[i];
            }  
            return "i," + to_string(currentSize - 1) + "," 
            + current[currentSize - 1];
        }
        else {
            for (size_t i = 0; i < currentSize; ++i) {
                if (current[i] != reviewed[i]) {
                    return "d," + to_string(i);
                }
            }
            return "d," + to_string(reviewedSize-1);
        }
    }
    return string();
    
}

void letterMan::write() {
    
    currentWord = discard.at(discardSize-1);
    vector<string> solution;

    if (outputFormat == 'M') {
        while (currentWord.discoveredBy != -1) {

            solution.push_back(different(currentWord.wordIn, 
            discard.at(currentWord.discoveredBy).wordIn));
            currentWord = discard.at(currentWord.discoveredBy);
        }
    }
    else {
        while (currentWord.discoveredBy != -1) {

            solution.push_back(currentWord.wordIn);
            currentWord = discard.at(currentWord.discoveredBy);

        }
    }
    
    solution.push_back(currentWord.wordIn);
    int solSize = static_cast<int>(solution.size());
    
    cout << "Words in morph: " << to_string(solSize) << "\n";
    cout << solution.at(solSize - 1) << "\n";
    
    for (int i = solSize - 2; i >= 0; --i) {
        
        if (outputFormat == 'W')
            cout << solution.at(i) << "\n";
        else
            cout << solution.at(i) << "\n";

    }
}

int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    letterMan letterman;

    letterman.get_options(argc, argv);
    letterman.read_in();
    
    if (!letterman.startExist) {
        cout << "Start word not found in dictionary\n";
        exit(1);
    }
    if (!letterman.endExist) {
        cout << "End word not found in dictionary\n";
        exit(1);
    }
    
    
    if (letterman.find_path()) {
        letterman.write();
    }
        
    else {
        cout << "No solution, " << to_string(letterman.discardSize + 1) << " words discovered.\n";
    }

    return 0;
}