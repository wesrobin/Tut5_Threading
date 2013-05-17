/* 
 * Wesley Robinson - RBNWES001
 * main.cpp
 * Tut 4 - Templating
 * Program to demonstrate the use of Templating to implement various ciphers which can
 * be used to encrypt and decrypt text.
 * Created on 07 May 2013, 1:10 PM
 */

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <ostream>
#include <boost/algorithm/string.hpp>
#include <string>
#include <map>
#include <algorithm>

#include "cmdline_parser.h"

using namespace std;

namespace RBNWES001 {

    struct CharCouple { //Structure for pairs of characters, eg 'ab'
        char first;
        char second;

        /**
         * Overload of < operator, returns true if the first paramater is < than
         * the second, false if it not. (Compares ascii values of characters)
         * @param one the first (RHS) charPair to be compared
         * @param two the second (LHS) charPair to be compared
         * @return true or false
         */
        bool operator<(CharCouple& rhs) const {
            if (this->first > rhs.first) {
                return false;
            }//If the first character of the second pair is less than that of the first pair, false
            else if (this->first == rhs.first && this->second > rhs.second) {
                return false;
            }//If the first characters of both pairs are equal, check the second
            else if (this->first == rhs.first && this->second == rhs.second) {
                return false;
            } //If both pairs are equal, return true.
            return true;
        }
    };

    class Counter {
    public:
        map<char, int> count;
        Counter(void){};
        void operator() (const string& str) {
            char ch;
            for (int i = 0 ; i < str.length() ; i++) {
                ++count[str[i]];
            }
        }
    };
}

int main(int argc, char** argv) {

    using namespace RBNWES001;

    cmdline_parser parser; //Initialise the parser

    if (!parser.process_cmdline(argc, argv)) { //Try parse cmdline args
        cerr << "Couldn't process command line arguments" << endl;
    }

    if (parser.should_print_help()) { //Print help if requested
        parser.print_help(std::cout);
    }

    string inputFileName = parser.get_input_filename();

    cout << "Input file in use: " << inputFileName << endl;

    string temp, fileStr;
    ifstream inpFileStream;

    inpFileStream.open(inputFileName);

    if (inpFileStream.is_open()) {

        while (!inpFileStream.eof()) { //Read from file into string
            getline(inpFileStream, temp);
            fileStr += temp;
        }
    } else {
        cout << "Error opening file" << endl;
        exit(EXIT_FAILURE);
    }
    
    inpFileStream.close();

    int numThreads = parser.get_num_threads();
    int period = parser.get_period();

    cout << "Number of threads: " << numThreads << endl;
    cout << "Period (ms): " << period << endl;
    
    string * section = new string[numThreads];
    int strLen = fileStr.length() / numThreads;

    for (int i = 0; i < numThreads; i++) {
        section[i] = fileStr.substr(i*strLen, strLen);
        cout << section[i] << endl;
    }

    thread * threads = new thread[numThreads];
    Counter * functors = new Counter[numThreads];
    
    for (int i = 0 ; i < numThreads ; i++) {
        threads[i] = thread(ref(functors[i]), section[i]);
    }
    
    for (int i = 0 ; i < numThreads ; i++) {
        threads[i].join();
    }
    
    map<char, int> finalCount;
    
    for (int i = 0 ; i < numThreads ; i++) {
        for( map<char, int>::iterator ii = functors[i].count.begin(); ii != functors[i].count.end(); ++ii)
        {
            cout << (*ii).first << ": " << (*ii).second << endl;
            finalCount[(*ii).first] += (*ii).second;
        }
        cout << "*****************************************" << endl;
    }
    
    cout << "FINAL" << endl;
    
    for( map<char, int>::iterator ii = finalCount.begin(); ii != finalCount.end(); ++ii)
    {
        cout << (*ii).first << ": " << (*ii).second << endl;
    }

    return 0;
}

