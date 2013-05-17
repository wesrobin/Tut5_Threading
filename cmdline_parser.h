/*
 * cmdline_parser.h
 *
 *  Created on: 22 Feb 2012
 *      Author: simon
 *      Extended by: Wesley Robinson - RBNWES001
 */

#ifndef CMDLINE_PARSER_H_
#define CMDLINE_PARSER_H_

#include <iostream>
#include <list>
#include <string>

#include <boost/program_options.hpp>

//---------------------------------------------------------------------------//

class cmdline_parser {
public:
    //-----------------------------------------------------------------------//

    cmdline_parser(void); // Constructor

    bool process_cmdline(int argc, char * argv[]); //Parse command line options

    std::string get_input_filename(void) const; //Get files

    int get_num_threads(void); //Get num threads

    int get_period(void); //Get period for collation

    void print_errors(std::ostream & out) const;

    bool should_print_help(void) const;
    void print_help(std::ostream & out) const; // Output help

private:
    //-----------------------------------------------------------------------//
    // Member variables
    boost::program_options::variables_map vm;
    boost::program_options::options_description od;
    std::list<std::string> errors;

    //-----------------------------------------------------------------------//
    // Static string variables
    static const std::string INPUTFILE;
    static const std::string NUMTHREADS;
    static const std::string PERIOD;
};

#endif /* CMDLINE_PARSER_H_ */
