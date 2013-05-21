/* 
 * Wesley Robinson - RBNWES001
 * cmdline_parser.cpp
 * cmdline_parser program for Tut 5
 * Created on 14 May 2013, 13:57 AM
 * Uses base code given by Simon Perkins as part of assignment brief
 */

#include "cmdline_parser.h"
#include <cstdint>


// Definition of static strings in the class
const std::string cmdline_parser::INPUTFILE = "input file";
const std::string cmdline_parser::NUMTHREADS = "num threads";
const std::string cmdline_parser::PERIOD = "period";

//-------------------------------------------------------------------------//
// Constructor, initialise the variables_map object with default
// constructor, options_descriptor with the name "Options"
//-------------------------------------------------------------------------//

cmdline_parser::cmdline_parser(void) : vm(), od("Options") {
    // Shorter alias for the boost::program_options namespace
    namespace po = boost::program_options;

    // Add two cmdline options
    // --help or -?
    // --input-file or -i
    od.add_options()
            ("help,?", "show usage help")

            ((INPUTFILE + ",i").c_str(), po::value<std::string>(), "input file name (default \"input.txt\")")
            ((NUMTHREADS + ",t").c_str(), po::value<int>(), "number of threads to be used (default 4)")
            ((PERIOD + ",p").c_str(), po::value<int>(), "period (ms) after which to collate and print frequencies (default 100)");
};

//-------------------------------------------------------------------------//
// Process the cmdline
//-------------------------------------------------------------------------//

bool cmdline_parser::process_cmdline(int argc, char * argv[]) {
    // Shorter alias for the boost::program_options namespace
    namespace po = boost::program_options;

    // Clear the variable map
    vm.clear();

    // Parse the cmdline arguments defined by argc and argv,
    // looking for the options defined in the od variable,
    // and store them in the vm variable.
    po::store(po::parse_command_line(argc, argv, od), vm);
    po::notify(vm);

    bool success = true;

    // THIS BELOW BIT IS COMMENTED OUT BECAUSE I'VE INCLUDED DEFAULT VALUES
    /*
//Both period and number of threads must be defined
if (vm.count(PERIOD) + vm.count(NUMTHREADS) != 2) {
    //Check if valid number of cyphers given
    success = false;
    errors.push_back("Please specify the period length and the number of threads to use");
}*/

    return success;
}

//--------------------------------Methods-------------------------------//
//Files

/**Get Input File
 * 
 * returns the file name of the input file
 */
std::string cmdline_parser::get_input_filename(void) const {
    if (vm.count(INPUTFILE) == 0)//If unspecified
        return "input.txt";     //Default filename
    return vm[INPUTFILE].as<std::string>();
}

/**Get num threads
 * 
 * returns the number of threads specified
 */
int cmdline_parser::get_num_threads(void) {
    if (vm.count(NUMTHREADS) == 0) //If unspecified
        return 4; //Default thread number
    return vm[NUMTHREADS].as<int>();
}

/**Get period
 * 
 * returns the period for collation specified
 */
int cmdline_parser::get_period(void) {
    if (vm.count(PERIOD) == 0) //If unspecified
        return 100; //Default period (in ms)
    return vm[PERIOD].as<int>();
}

//------------------------------End Methods-----------------------------//

//----------------------------------------------------------------------//
// Should we print cmdline help?
//----------------------------------------------------------------------//

bool cmdline_parser::should_print_help(void) const {
    // Are there instances of the help option stored in the variable map
    return vm.count("help") > 0;
}

//----------------------------------------------------------------------//
// Print out the options_descriptor to the supplied output stream
//----------------------------------------------------------------------//

void cmdline_parser::print_help(std::ostream & out) const {
    out << od;
}

//----------------------------------------------------------------------//
// Print out the options_descriptor to the supplied output stream
//----------------------------------------------------------------------//

void cmdline_parser::print_errors(std::ostream & out) const {
    std::cerr << "Error processing command line arguments:" << std::endl;
    std::copy(errors.begin(), errors.end(),
            std::ostream_iterator<std::string>(out, "\n"));
}
