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
#include <chrono>

#include "cmdline_parser.h"

using namespace std;

namespace RBNWES001 {
    
    /**
     * Code to switch the key and value fields of a map, taken from:
     * http://stackoverflow.com/questions/5056645/sorting-stdmap-using-value
     */
    template<typename A, typename B>
    pair<B,A> flip_pair(const pair<A,B> &p)
    {
        return pair<B,A>(p.second, p.first);
    }

    template<typename A, typename B>
    map<B,A> flip_map(const map<A,B> &src)
    {
        map<B,A> dst;
        transform(src.begin(), src.end(), inserter(dst, dst.begin()), flip_pair<A,B>);
        return dst;
    }

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
    ostream& operator<<(ostream& os, const CharCouple& cc) {
        os << cc.first << cc.second;
        return os;
    }

    /**
     * Counter functor that counts the occurrences of chars and CharCouples, 
     * storing them in maps templated appropriately.
     */
    class Counter {
    public:
        map<char, int> countSingle; //For single chars
        map<CharCouple, int> countCouple; //For char couplets

        Counter(void) {
        };
       
        /**
         * Operator() overload, causing this class to become a functor, updates
         * the single char and char couplet maps stored in the functor.
         * @param str The string whose characters are to be counted
         */
        void operator() (const string& str) {
            CharCouple temp;
//            ++countSingle[str[0]];
//            for (int i = 1; i < str.length(); i++) {
//                temp.first = str[i - 1];
//                temp.second = str[i];
//                ++countCouple[temp];
//                ++countSingle[str[i]];
//            }
            //*********** STL Implementation ***********
            for_each(str.begin(), str.end(), [&](const char& c)->void {
                if (c == *str.begin()) {
                    temp.second = c;
                }
                else {
                    temp.first = temp.second;
                    temp.second = c;
                    ++countCouple[temp];
                }
                ++countSingle[c];
            });
            //******************************************
        }
    };

    /**
     * Collator functor that counts the occurrences of chars and CharCouples, 
     * storing them in maps templated appropriately.
     */
    class Collator {
    public:
        int period, numThreads_;
        Counter * counters;
        bool running;

        Collator(int period_, Counter* counters_, int threads_) : period(period_), counters(counters_), numThreads_(threads_), running(true) {
        };
        
        /**
         * Method to signal that the thread should stop looping by changing the
         * running variable to false
         */
        void stopRunning() {
            running = false;
        }

        void operator() (void) {
            while (running) {   //Runs until running becomes false
                this_thread::sleep_for(chrono::milliseconds(period));   //Sleeps the thread so it will not attain data again until it needs to
                
                cout << "********" << endl;
                map<char, int> singleCount;     //Variables are instantiated in the loop so that they are destroyed each time the loop is completed
                map<CharCouple, int> coupleCount;

                map<int, char> singleCountSwitched;
                map<int, CharCouple> coupleCountSwitched;
                
//                for (int i = 0; i < numThreads_; i++) {
//                    for (map<char, int>::iterator ii = counters[i].countSingle.begin(); ii != counters[i].countSingle.end(); ++ii) {
//                        singleCount[(*ii).first] += (*ii).second;
//                    }
//                    for (map<CharCouple, int>::iterator ij = counters[i].countCouple.begin(); ij != counters[i].countCouple.end(); ++ij) {
//                        coupleCount[(*ij).first] += (*ij).second;
//                    }
//                }
                
                //*********** STL Implementation ***********
                for_each(counters, counters + numThreads_, [&](Counter& counter)->void {
                    map<char, int> charCounts = counter.countSingle;
                    for_each(charCounts.begin(), charCounts.end(), [&](std::pair<const char, int>& p)->void {
                        singleCount[p.first] += p.second;
                    });
                    
                    map<CharCouple, int> coupleCounts = counter.countCouple;
                    for_each(coupleCounts.begin(), coupleCounts.end(), [&](pair<const CharCouple, int>& p)->void {
                        coupleCount[p.first] += p.second;
                    });
                });
                //******************************************
                
                singleCountSwitched = flip_map(singleCount);    //Flip the values and keys to give a new map that is ordered by the counts
                coupleCountSwitched = flip_map(coupleCount);    
                
//                map<int, char>::iterator sit = --singleCountSwitched.end();     //Since the switched map is ordered from smallest to biggest, start from the last element (one less than .end())
//                for (int i = 0 ; i < 10; i++) { //Return the 10 highest counts
//                    if (sit != singleCountSwitched.begin()) {   //If there are less than 10 elements, break out after printing the last element
//                        cout << (*sit).second << ": " << (*sit).first << endl;
//                        --sit;
//                    }
//                    else {cout << (*sit).second << ": " << (*sit).first << endl;break;}
//                }
//                
//                map<int, CharCouple>::iterator cit = --coupleCountSwitched.end();
//                for (int i = 0 ; i < 10; i++) {
//                    if (cit != coupleCountSwitched.begin()) {
//                        cout << (*cit).second << ": " << (*cit).first << endl;
//                        --cit;
//                    }
//                    else {break;}
//                }
                
                //*********** STL Implementation ***********
                auto beginPointSingle = singleCountSwitched.end();
                if (singleCountSwitched.size() > 10) {
                    advance(beginPointSingle, -10);     //Begin at the 10th-to-last element
                }
                else {
                    beginPointSingle = singleCountSwitched.begin();   //If there are less than 10 counts, just start at the beginning
                }
                for_each(beginPointSingle, singleCountSwitched.end(), [&](pair<const int, char>& p)->void {
                    cout << p.second << ": " << p.first << endl;
                });
                
                auto beginPointCouple = coupleCountSwitched.end();
                if (coupleCountSwitched.size() > 10) {
                    advance(beginPointCouple, -10);     //Begin at the 10th-to-last element
                }
                else {
                    beginPointCouple = coupleCountSwitched.begin();   //If there are less than 10 counts, just start at the beginning
                }
                for_each(beginPointCouple, coupleCountSwitched.end(), [&](pair<const int, CharCouple>& p)->void {
                    cout << p.second << ": " << p.first << endl;
                });
                //******************************************
                cout << "********" << endl;
                
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

    fileStr.erase(remove_if(fileStr.begin(), fileStr.end(), [](char c) {
        return isspace(c);
    }), fileStr.end()); //Remove all spaces from string

    inpFileStream.close();

    int numThreads = parser.get_num_threads();
    int period = parser.get_period();

    cout << "Number of threads: " << numThreads << endl;
    cout << "Period (ms): " << period << endl;

    string * section = new string[numThreads];

    int fileLen = fileStr.length(); //Number of characters in the file

    int at, pre = 0, i;
    for (pre = i = 0; i < numThreads; ++i) {
        at = (fileLen + fileLen * i) / numThreads;
        section[i] = fileStr.substr(pre, at - pre);
        pre = at;
    }

    thread * threads = new thread[numThreads];
    Counter * counterFunctor = new Counter[numThreads];

    for (int i = 0; i < numThreads; i++) {
        threads[i] = thread(ref(counterFunctor[i]), section[i]);
    }
    
    Collator collatorFunctor(period, counterFunctor, numThreads);
    thread collator(ref(collatorFunctor));

    for (int i = 0; i < numThreads; i++) {
        threads[i].join();
    }
    
    collatorFunctor.stopRunning();
    collator.join();    //Allows the collator to finish printing out the final count
  
    return 0;
}