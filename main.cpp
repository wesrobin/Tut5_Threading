/* 
 * Wesley Robinson - RBNWES001
 * main.cpp
 * Tut 4 - Templating
 * Program to demonstrate the use of Templating to implement various ciphers which can
 * be used to encrypt and decrypt text.
 * Created on 07 May 2013, 1:10 PM
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ostream>
#include <boost/algorithm/string.hpp>
#include <string>

#include "cmdline_parser.h"

using namespace std;

int main(int argc, char** argv) {

    cmdline_parser parser; //Initialise the parser

    if (!parser.process_cmdline(argc, argv)) { //Try parse cmdline args
        cerr << "Couldn't process command line arguments" << endl;
    }

    if (parser.should_print_help()) {   //Print help if requested
        parser.print_help(std::cout);
    }
    
    string inputFileName = parser.get_input_filename();
    
    cout << "Input file in use: " << inputFileName << endl;
    
	/*string temp;
	ifstream inpFileStream;

	inpFileStream.open(inputFileName);


	if (inpFileStream.is_open()) {

		while (!inpFileStream.eof()) { //Read from file into string encodable
			getline(inpFileStream, temp);
			encodable += temp;
		}
	}*/
	
	cout << "Number of threads: " << parser.get_num_threads() << endl;
	cout << "Period (ms): " << parser.get_period() << endl;

	//inpFileStream.close();

    return 0;
}

