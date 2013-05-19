/* 
 * Wesley Robinson - RBNWES001
 * main.cpp
 * Tut 4 - Templating
 * Program to demonstrate the use of Templating to implement various ciphers 
 * which can be used to encrypt and decrypt text.
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
        bool operator<(const CharCouple& rhs) const {
//            return ((this->first * 128 + this->second) < (rhs.first * 128 + rhs.second));
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
        
        /**
         * Operator<< overload
         */
        friend ostream& operator<<(ostream& os, const CharCouple& cc);
    };
    
    /**
     * Non-member operator << overload for CharCouple struct
     * @param os the ostream to << to
     * @param cc the charcouple to <<
     * @return the ostream
     */
    ostream& operator<<(ostream& os, const CharCouple& cc)
    {
        os << cc.first << cc.second;
        return os;
    }

    /**
     * Counter functor that counts the occurences of chars and CharCouples, 
     * storing them in maps templated appropriately.
     */
    class Counter {
    public:
        map<char, int> countSingle;     //For single chars
        map<CharCouple, int> countCouple;       //For char couplets
        
        Counter(void){};
        
        /**
         * Operator() overload, causing this class to become a functor, updates
         * the single char and char couplet maps stored in the functor.
         * @param str The string whose characters are to be counted
         */
        void operator() (const string& str) {
            CharCouple temp;
            ++countSingle[str[0]];
            for (int i = 1 ; i < str.length() ; i++) {
                temp.first = str[i-1];
                temp.second = str[i];
                ++countCouple[temp];
                ++countSingle[str[i]];
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
    
    fileStr.erase(remove_if(fileStr.begin(), fileStr.end(), [](char c) {return isspace(c);}), fileStr.end());   //Remove all spaces from string
    
    inpFileStream.close();

    int numThreads = parser.get_num_threads();
    int period = parser.get_period();

    cout << "Number of threads: " << numThreads << endl;
    cout << "Period (ms): " << period << endl;
    
    string * section = new string[numThreads];
    
    int fileLen = fileStr.length();     //Number of characters in the file
    
    int at, pre = 0, i;
    for (pre = i = 0; i < numThreads; ++i) {
        at = (fileLen + fileLen*i)/numThreads;
        section[i] = fileStr.substr(pre, at-pre);
        pre = at;
    }

    thread * threads = new thread[numThreads];
    Counter * counterFunctor = new Counter[numThreads];
    
    for (int i = 0 ; i < numThreads ; i++) {
        threads[i] = thread(ref(counterFunctor[i]), section[i]);
    }
    
    for (int i = 0 ; i < numThreads ; i++) {
        threads[i].join();
    }
    
    map<char, int> finalSingleCount;
    map<CharCouple, int> finalCoupleCount;
    
    for (int i = 0 ; i < numThreads ; i++) {
        for( map<char, int>::iterator ii = counterFunctor[i].countSingle.begin(); ii != counterFunctor[i].countSingle.end(); ++ii)
        {
            finalSingleCount[(*ii).first] += (*ii).second;
        }
        for( map<CharCouple, int>::iterator ij = counterFunctor[i].countCouple.begin(); ij != counterFunctor[i].countCouple.end(); ++ij)
        {
            finalCoupleCount[(*ij).first] += (*ij).second;
        }
    }
    
    for( map<char, int>::iterator i = finalSingleCount.begin(); i != finalSingleCount.end(); ++i)
    {
        cout << (*i).first << ": " << (*i).second << endl;
    }
    cout << "********" << endl;
    for( map<CharCouple, int>::iterator i = finalCoupleCount.begin(); i != finalCoupleCount.end(); ++i)
    {
        cout << (*i).first << ": " << (*i).second << endl;
    }

    return 0;
}