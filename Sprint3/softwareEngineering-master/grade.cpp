/*!
 * \file grade.cpp
 * \brief Grade.cpp is designed to test computer programs against
 *          predefined test cases and answer files. Test cases contain
 *          output that may be provided to the program and answer files
 *          contain a correct solution. Test files may also be generated
 *          by the program. Generation is done based on user input to 
 *          define the number of test files, file size, numeric range of 
 *          entries, entry type (int or float), and an output file name.
 *
 */ 

/***********************************************************************
                            SPRINT 1 INFORMATION
   Author: Intolerable Optimists (Chris Smith, Jarod Hogan, Charles Parsons)
   Class:CSC 470 Software Engineering
   Instructor:Dr. Logar
   Date:19 Feb 2014
   Description:    This program is a testing suite for other programs
   Input:       Program to be tested *.cpp
   Output:      *.log is the log file with passes and fails of tests include
                d is the percentage of each and the amount
   Compilation instructions: standard usage with g++
   Usage:./grade <input_program.cpp>
   Known bugs/missing features: No known bugs
   Modifications:
 ************************************************************************/

/***********************************************************************
                            SPRINT 2 INFORMATION
   Author:      The Obfuscators (Dan Nix, Joe Lillo, Lisa Woody)

   Class:       CSC 470 Software Engineering
   
   Instructor:  Dr. Logar
   
   Date:        23 Mar, 2014
   
   Description: This program runs predefined test cases agains other 
                c++ programs and tests for correct output.
   
   Input:       Program to be tested (without cpp extension)
   
   Output:      Student log files:  Individual student record
                Main log file:      Log including all students 
                .tst files:         Generated test files 
                .ans files:         Generated answer files 
   
   Compiling:   Standard usage of g++:
                g++ grade.cpp -o grade
   
   Usage:       ./grade <input_program>
   
   Known bugs/missing features: 
        Bug: Infinite loops if a string is entered (opposed to a single 
                character, int, or float) when prompted for test file
                generation 
   
   Modifications:
 ************************************************************************/

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <cmath>
#include <limits.h>
#include <iomanip>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;

const bool FILES = true;
const bool DIRECTORIES = false;


//function prototypes
void grade( vector <string> ,vector <string> , vector <string>, ofstream &, string );
double calc_percentage(int , int );
void directoryCrawl( bool , string , string , bool, vector <string> & names, bool );
bool test(const string &, string , string );
int print_menu();
bool create_test_cases();
bool create_specific_test_cases( string );
double generate_random( double min, double max, char type );
string iToA( int i );
bool create_ans_file( string , string );
bool compile_code( string  );
bool check_if_menu_prog( string );//Checks if program structure is menu driven
void create_menu_test_cases(string);//creates the test cases for a menu driven program
bool run_code( string , string  );
string removeExtension (string );
string fileNameFromPath (string );
void grade_specific_assignment( string );//grades a specific directory
void grade_all_assignments();
void create_all_test_cases();
/*! *******************************************************
 * \brief Main entry point of program.
 * \return 0 - Success / Else - Error
 ********************************************************** */
int main( )
{
    vector <string> critTestCases;
    vector <string> testCases;
    vector <string> studentDirs;
    ofstream LOG;
    //time_t rawTime;
    //tm * timeInfo;
    //char buffer [40];
    string specific_dir;
    int menuFLAG = 0;

    // Create menu until user chooses to exit
    while ( menuFLAG != 5 )
    {
        // Get user input from the menu
        menuFLAG = print_menu();

        // If the user selects to grade student programs...
        if ( menuFLAG == 1 )
        {
            grade_all_assignments();
        }
        else if ( menuFLAG == 2 )
        {
            cout << "Enter name of the directory to be graded: ";
            cin >> specific_dir;

            grade_specific_assignment( specific_dir + "/" );
        }
        // If the user selects to create test cases
        else if ( menuFLAG == 3 )
        {
            create_all_test_cases();
        }
        else if ( menuFLAG == 4 )
        {
            cout << "Enter name of the directory for test case generation: ";
            cin >> specific_dir;

            create_specific_test_cases( specific_dir + "/" );
        }
        else
            cout << "Invalid menu choice." << endl << endl;


        cout << endl << endl;
    }

}

/*! ********************************************************
 * \brief Run all student programs against all test cases.
 * \author Elizabeth Woody
 * \param critTestCases - Critical test cases.
 * \param testCases - Normal test cases.
 * \param studentDirs - Student directory names.
 * \param LOG - Main log file.
 * \param exeTime - Time stamp
 * *********************************************************/
void grade( vector <string> critTestCases, vector <string> testCases,
            vector <string> studentDirs, ofstream &LOG, string exeTime )
{
    string program;
    vector<string>::iterator stdnt_it;
    vector<string>::iterator test_it;
    vector<string> cppFiles;
    bool FLAG;
    int passCount;
    ofstream student_LOG;

    // Iterate over student directories
    for ( stdnt_it = studentDirs.begin(); stdnt_it < studentDirs.end(); stdnt_it++)
    {
        // Clear the vector the vector containing the currently found cpp programs
        cppFiles.clear();

        // Find all cpp files in the directory
        directoryCrawl( FILES, *stdnt_it, ".cpp", false, cppFiles, 0 );

        // Create variables to store the name of the executable and the directory
        string executable;
        string dir_name = fileNameFromPath(*stdnt_it);


        // If a cpp file exists in the directory, store the names of the executable
        // and the directory.
        if (cppFiles.size() > 0)
        {
            executable = fileNameFromPath(cppFiles[0]);
            executable = removeExtension(executable);

            // If the executable file found is does not have the same name as the
            // directory, ignore it.
            if (dir_name != executable)
            {
                continue;
            }
        }
        // If no cpp files were found, continue
        else
        {
            continue;
        }

        // Compile the program to be tested
        compile_code(cppFiles[0]);

        // Add the student directory path to the executable
        executable = *stdnt_it + "/" + executable;

        // Create a student log in the directory
        student_LOG.open( string(executable + exeTime + ".log").c_str() );
        student_LOG << "Critical Test Cases:" << endl << endl;

        // Create a flag to keep track of failed acceptance tests
        FLAG = true;

        // Iterate through the critical test cases
        for ( test_it = critTestCases.begin(); test_it != critTestCases.end() && FLAG == true; test_it++ )
        {
            // Create variables to store the names of the file to be tested and the file containing the output
            string testFile = fileNameFromPath(*test_it);
            string out_file = *stdnt_it + "/" + testFile;
            out_file = removeExtension(out_file);
            out_file += ".out";

            // Record the currently tested file name in the student log
            student_LOG << string( removeExtension(testFile) ) << ": ";

            // If the student's program fails the critical test...
            if ( !test(executable,out_file,*test_it) )
            {
                // Record the student's grade as "FAILED" in the main summary log
                LOG << stdnt_it -> substr( stdnt_it -> find_last_of('/') + 1, stdnt_it -> length() )
                    << ": FAILED" << endl;

                // Set the acceptance test flag to false
                FLAG = false;

                // Document the the failure in the student's directory log
                student_LOG << "FAIL" << endl << endl;
                student_LOG << "Student Failed a Critical Test File" << endl;


            }
            // If the student's program passes the critical test, record a "PASS" in the student log.
            else
                student_LOG << "PASS" << endl;
        }

        // Create a variable to store the number of passed regular test cases
        passCount = 0;

        // If the student all of the critical test cases, begin documentation for the regular test cases.
        if ( FLAG == true)
            student_LOG << endl << "Non-Critical Test Cases:" << endl << endl;

        // Iterate through the regular test cases, provided that the student passed the acceptance testing
        for ( test_it = testCases.begin(); test_it != testCases.end() && FLAG == true; test_it++ )
        {
            // Create variables to store the names of the file to be tested and the file containing the output
            string testFile = fileNameFromPath(*test_it);
            string out_file = *stdnt_it + "/" + testFile;
            out_file = removeExtension(out_file);
            out_file += ".out";

            // Record the currently tested file name in the student log
            student_LOG << string( removeExtension(testFile) ) << ": ";

            // If the student's program passes the critical test, record a "PASS" in the student log
            // and increment the pass count
            if ( test(executable,out_file, *test_it) )
            {
                passCount++;
                student_LOG << "PASS" << endl;
            }
            // If the student's program fails the test, record a "FAIL" in the student log
            else
                student_LOG << "FAIL" << endl;
        }

        // If all the critical test cases were passed, record success rate of the other test cases
        // as the student's grade in the main summary log file,
        // and record the details of the passes and failures in the student log file.
        if ( FLAG )
        {
            //Debug// cout << calc_percentage( passCount, testCases.size() ) << endl;
            LOG << stdnt_it -> substr( stdnt_it -> find_last_of('/') + 1, stdnt_it -> length() )
                << ": " << calc_percentage( passCount, testCases.size() ) << endl;

            // outputs the passes, fails and percentages to the student's log file.
            student_LOG << endl << endl << "total number of passes: " << passCount << endl;
            student_LOG << "total number of fails: " << (testCases.size() - passCount) << endl;
            student_LOG << "percentage of passes: "
                        << calc_percentage( passCount, testCases.size() ) << "%" << endl;
        }

        // Close the student log file
        student_LOG.close();
    }
}


/*! *******************************************************
 * \brief Returns a floating point percentage (passed/total)
 * \param passes - number of passed tests
 * \param total - Total number of tests
 * \return Percentage (0-100)
 ********************************************************** */
double calc_percentage(int passes, int total)
{
    return (double)passes/(total)*100.0;
}



/*! *******************************************************
 * \brief Recursively crawls through a file system to find files or directory names.
 * \author Joe Lillo & Elizabeth Woody
 * \param type - Target directory names or regular file names.
                       (True = Files, False = Directories)
 * \param dir - The root directory of the search. (Updated recursively)
 * \param file - String to match in file name.
 * \param recursive - Recursively search sub-directories?
 * \param names - Return vector of file/directory names.
 * \param critFLAG - Used to specify if we are looking for critical test files
 *
 * Recursively crawls through a file system to find files or directory names.
 *               The files that meet the search parameters are returned in a vector
 *                of strings (the names parameter).
 *
 * ***********************************************************/
void directoryCrawl( bool type, string dir, string file, bool recursive, vector <string> &names, bool critFLAG )
{
    // Structure in dirent.h stores the file name and the file id number
    struct dirent **namelist;

    int n;
    int i;

    // Scan dir for files and directories
    n = scandir(dir.c_str(), &namelist, 0, alphasort);

    // Error check
    if(n == -1)
        return;

    // Loop over the files in the directory
    for ( i = 2; i < n; i++ )
    {
        string tempFile = namelist[i]-> d_name;

        // Verify that the file name contains the passed in criteria
        if(string::npos != tempFile.find(file))
        {
            // If the file found is a directory and the purpose is to
            // look for direcotries, add to the vector
            if( DT_DIR == namelist[i] -> d_type && type == DIRECTORIES )
            {
                names.push_back(dir + "/" + tempFile);
            }

            // If the file found is a file and the purpose is to
            // look for files, add to the vector
            if( DT_REG == namelist[i] -> d_type && type == FILES )
            { 
                // Filter critcal test cases if not looking for critical test cases
                if ( string::npos != tempFile.find("_crit.tst") && !critFLAG )
                    continue;

                // Add the discovered files to the vector
                names.push_back(dir + "/" + tempFile);
            }
        }

        // Make a recursive call
        if (recursive && DT_DIR == namelist[i] -> d_type && tempFile != "." && tempFile != ".." )
            directoryCrawl( type, dir + "/" + tempFile, file, true , names, critFLAG );

    }

    // Cleanup
    for(i = 0; i < n; i++)
        delete []namelist[i];
    delete []namelist;
}


/*! *******************************************************
 * \brief This function will run a tst file with the compiled cpp
                file and compare the outputs of redirected output to a
                .out file with a .ans file by running a diff command
 * \author Chris Smith
 * \param program - current program string to run
 * \param a_file - Output file.
 * \param tst_file - Test file
 * \return Program Passed or Failed.
 ********************************************************** */
bool test(const string &program, string a_file, string tst_file)
{

    
    //runs the .cpp program with redirected input and output
    system(string(program + " < " + tst_file + " > " + a_file).c_str());




    //runs a diff command on the .ans and .out files and returns true if 
    //they are not different
    if(system(string("diff " + a_file + " " + tst_file.substr(0,tst_file.find_last_of('.') ) + ".ans").c_str()) == 0)
    {
        return true;
    }

    return false;
}



/*! *******************************************************
 * \brief Prints menu to let user choose between running test
 *                  cases or creating new ones
 * \author Dan Nix
 * \return choice - Menu choice number
 ********************************************************** */
int print_menu()
{

    int choice = 0;
	
    // While the user hasn't entered a valid choice, get a new choice
    while( choice < 1 || choice > 5 )
    {

        cout << "Main Menu:" << endl;
        cout << "1. Test all program assignments" << endl;
        cout << "2. Test specific assignment" << endl;
        cout << "3. Create test files for all assignments" << endl;
        cout << "4. Create test files for specific assignment" << endl;
        cout << "5. EXIT" << endl;

        cout << "Choice: ";
        cin >> choice;
        cout << endl;
    }

    return choice;
}




/*! *******************************************************
 * \author Daniel Nix
 * \return true on successfull creation, false otherwise
 *
 * This is the primary function for generating new test
 *                  and answer files. It prompts the user for the number
 *                  of test cases, max entries per file, max and min values
 *                  for file entries, int or float test entries, and output
 *                  test/answer file name
 ********************************************************** */
bool create_test_cases()
{

    string fn = "gen_case";
    int num_cases = 0;  // Number of test files to create
    int max_entries = 0;    // Number of entries in a test file
    
    
    double range_min = INT_MIN; // Minimum in user range
    double range_max = INT_MAX;  // Maximum in user range
    
    char data_type = '\0';
    char choice = 'n';
    ofstream fout;
    
    
    // Find how many test files the user wants to create
    while( num_cases < 1 || num_cases > 100){
        cout << "How many test cases: ";
        cin >> num_cases;
    }

    // How many entries will be in the test file
    while( max_entries < 1 || max_entries > 200 ){
        cout << "Number of entries in test file: ";
        cin >> max_entries;
        
    }

    while( data_type != 'i' && data_type != 'f' ){
        cout << "(I)ntegers or (F)loats? ";
        cin >> data_type;

        data_type = tolower( data_type );
    }

    // Get a minimum for the range if the user wants to supply it
    cout << "Would you like to specify a minimum value? (y/n) ";
    cin >> choice;

    // If the user want to enter a min, prompt for it now
    if( tolower( choice ) == 'y' ){
        // While the user hasn't entered a valid min, ask again
        while( range_min <= INT_MIN ){
            cout << "Minimum value: ";
            cin >> range_min;
        }
    }

    // Get a maximum for the range if the user wants to supply it
    cout << "Would you like to specify a maximum value? (y/n) ";
    cin >> choice;

    // If the user wants to enter a max, prompt for it now
    if( tolower( choice ) == 'y'){
        // While the user hasn't entered a valid number, ask again
        while( range_max >= INT_MAX || range_max < range_min ){
            cout << "Maximum value (>= minimum): ";
            cin >> range_max;
        }
    }

    // Prompt user for a test file name
    cout << "Would you like to specify an output name?\n \
                Default is 'gen_caseXXX': (y/n) ";
    cin >> choice;

    // If the user wants to specify a name, get it here
    if( tolower( choice ) == 'y' ){
        cout << "Base File Name (do not include .tst): ";
        cin >> fn;
    }

    //Begin test file creation
    int found;
    vector<string> golden_name;
    // Crawl through the root directory to find the golden cpp
    directoryCrawl( true, ".", ".cpp", false, golden_name, 0 );
    
    // Since our grade.cpp file could possibly be in the base directory
    // strip it out, just in case...
    for( unsigned int a=0; a < golden_name.size(); a++ ){
          
        // Assuming Dr. Logar will compile our code in the same directory,
        //  make sure that we don't try to use our code as the "golden cpp"
        //  i.e. Delete anything with "grade" in it from the list returned
        found = golden_name[a].find("grade");
        if ( (unsigned int)found < golden_name[a].size() ){
            golden_name.erase( golden_name.begin() + found );
                      
        }
                  
    }
    
    // The "golden cpp" will be the only item left in the list since it 
    // is the only .cpp file in the root directory. Compile it now!
    compile_code( golden_name[0] );

    // Initialize variables
    string filename;        //String holding the output file path
    int file_counter = 1;   //Number to append to output file name
    bool nopen = false;     //Bool to store if the file is opened
    double temp;             //Temporary var to use while generating random #s
    float file_size;        //Holds the maximum number of entries in a file

    // For each test file that we need to create
    for( int j=0; j < num_cases; j++  ){
        
        // Assume the file hasn't opened
        nopen = false;

        //Piece together the path to store the tst and ans files
        filename = "test_";
        fout << fixed;
        if( data_type == 'i' ){
            fout << setprecision(0);
            filename += "int/";
        }
        else{
            fout << setprecision(5);
            filename += "float/";
        }
        
        filename += fn; // Append the file name
        filename += iToA( file_counter );   // Append the counter
        filename += ".tst";
        file_counter++;
            
        // Open the file to write
        fout.open( filename.c_str() );

        // Abort if we couldn't open the test file
        if( !fout ){
            cout << "Could not open test file" << endl;
            nopen = true;
        }
 
        // If we opened the test file then fill it with randoms
        if( !nopen ){
            // Print the generated numbers to the test file
            
            // We want the file size to be between 0 and max_entries
            file_size = generate_random( 0, max_entries, 'i' );

            // Output random numbers to the test file
            for( int i=0; i < file_size; i++){
                // Generate a single random number for the file
                temp = generate_random( range_min, range_max, data_type );   
                
                // Write the number and a newline to file 
                fout << temp << "\n";
            }

            // Close the test file
            fout.close();

            // Using the newly generated test file, generate the answer file
            create_ans_file( filename, golden_name[0] );
        }
    }

    return true;
}

bool create_specific_test_cases( string assignment )
{
    //checks if menu assignment and goes through menu case generation or other generation
    return false;
}


/*! *******************************************************
 * \brief Generates a random integer or floating point number
 *                  on the closed interval [min,max]
 * \author Daniel Nix
 * \param min - Minimum value to generate
 * \param max - Maximum value to generate
 * \param type - character representing (i)nts or (f)loats
 * \return Random Number
**********************************************************  */
double generate_random( double min, double max, char type ){   
  
    double random = 0.0;
    double range = max - min;

    // Convert random int to [0,range]
    random = range * ((double)rand() /RAND_MAX);
    // Add min to get random number in [min,max]
    random = random + min;

    // If we want an integer, cast it to that
    if( type == 'i' ){
        return int( random );
    }
    // Otherwise, return the double
    return random;

}



/*! *******************************************************
 * \brief Converts an integer value to a string to append
 * \author Daniel Nix
 * \param i - base 10 integer to convert to a string
 * \return number  - integer represented as a string
 ********************************************************** */
string iToA( int i ){
    
    string number = "";
    char temp_char;
    int temp_num;
    float order_of_magnitude = 0;
    
    // Figure out the order of magnitude of the number
    // While i%(10^orderOfMagnitude) >= 10 )
    while( i / int( pow(10.0, order_of_magnitude) ) >= 10 ){
        order_of_magnitude++;
    }

    // Strip off digits one at a time and append to the string
    while( order_of_magnitude >= 0 ){
        // Strip off the greatest power of 10
        temp_num = i - (i % int( pow( 10.0, order_of_magnitude  ) ));
        temp_num = i / pow( 10.0, order_of_magnitude );

        // Convert the integer value to its ascii character
        temp_char = temp_num + '0';
        number += temp_char;
        
        // Subtract off the power that we just appended to the string
        i -= temp_num * pow( 10.0, order_of_magnitude );
        order_of_magnitude--;
    }
    return number;
}


/*! *******************************************************
 * \brief This function compiles the source code "filename.cpp"
 *                  into an executable called "filename"
 * \author Daniel Nix
 * \param filename - the name of the .cpp file
 * \return true - When the code is successfully compiled
 */
bool compile_code( string filename )
{
    // Strip off the .cpp extension
    string executable = removeExtension( filename );
    
    // Produce "g++ program.cpp -o program" command
    string compile_instruction = "g++ ";
    //add in the flags for gcov
    compile_instruction += "-fprofile-arcs -ftest-coverage ";
    //add in the flag for gprof
    compile_instruction += "-pg ";
    compile_instruction += filename;
    compile_instruction += " -o ";
    compile_instruction += executable; // name of golden cpp file

    // Compile the source code
    system( compile_instruction.c_str() );

    return true;
}

bool check_if_menu_prog( string program_directory )//Checks if program structure is menu driven
{
    struct dirent **namelist;   //structure that contains files and directories that scandir is called

    int n;                      //total of number files and directories in given directory
    int i;                      //loop counter

    string temp;                //tmp variable to conver c string to c++ string in loop
    bool found = false;         //variable to be returned regardless of file found or not since its initially set to false
    
    // Scan dir for files and directories
    n = scandir( program_directory.c_str(), &namelist, 0, alphasort );

    // Error check
    if(n == -1)
        return found;

    //exits the loop if a spec file is found or the whole array is searched
    for( i = 2; i < n && !found; i++)
    {
        //converts the c style string to a c++ string
        temp = namelist[i]-> d_name;
        //checks to see if the given string is a .spec file
        if( temp.find( ".spec" ) != string::npos )
            //if the file is a spec file then found is set to true
            found = true;
    }

    // Cleanup
    for(i = 0; i < n; i++)
        delete []namelist[i];
    delete []namelist;


    return found;

}

void create_menu_test_cases( string menu_file )//creates the test cases for a menu driven program
{

}


/*! *******************************************************
 * \brief This function runs the golden cpp on the test file to create the .ans file.
 * \param test_file_path - Path to test file to run
 * \param golden - Name of compiled "golden cpp"
 * \return true - if the file is created successfully
 ********************************************************** */
bool create_ans_file( string test_file_path, string golden ){

    // Remove the .cpp file extension
    string executable = removeExtension( golden );

    // Append .ans file extension 
    string ans_file_path = test_file_path;
    ans_file_path = ans_file_path.replace( ans_file_path.find( ".tst" ), 4, ".ans" );

    // Piece together the shell command to run the program
    // with redirected input and output
    string run_instruction = "./";
    run_instruction += executable; 
    run_instruction += " < ";
    run_instruction += test_file_path;
    run_instruction += " > ";
    run_instruction += ans_file_path;

    // Run the command
    system( run_instruction.c_str() );
    return true;
}


/*! *******************************************************
 * \brief Removes the extension from a file name.
 * \author Joe Lillo
 * \param file - File name
 * \return File name without extension.
 ********************************************************** */
string removeExtension (string file)
{
    string temp = file.substr(0 ,file.find_last_of('.'));
    return temp;
}


/*! *******************************************************
 * \brief Returns the file name from a path to a file.
 * \author Joe Lillo
 * \param file - File path.
 * \return File name
 ********************************************************** */
string fileNameFromPath (string file)
{
    string temp = file.substr( file.find_last_of('/')+1, file.length() );
    return temp;
}
void grade_specific_assignment( string directory)//grades a specific directory
{
    
    vector <string> critTestCases;
    vector <string> testCases;
    vector <string> studentDirs;
    ofstream LOG;
    time_t rawTime;
    tm * timeInfo;
    char buffer [40];

    // Clear vectors containing test cases and directories
    critTestCases.clear();
    testCases.clear();
    studentDirs.clear();

    // Create time stamp string for files
    time (&rawTime);
    timeInfo = localtime (&rawTime);
    strftime (buffer,40,"%d_%m_%y_%H_%M_%S",timeInfo);
    string exeTime = buffer;
    LOG.open(string("GRADES_"+exeTime+".log").c_str());

    // Find all critical test cases
    directoryCrawl( FILES,directory, "_crit.tst", true, critTestCases, 1 );

    // Find all regular test cases
    directoryCrawl( FILES, directory, ".tst", true, testCases, 0 );

    // Find all student directories
    directoryCrawl( DIRECTORIES,directory, "", true, studentDirs, 0  );

    // Grade programs against test cases
    grade( critTestCases, testCases, studentDirs, LOG, exeTime );

    // Close the Grading Summary log file
    LOG.close();
}
void grade_all_assignments()
{
    //grab all sub directories
  
    //loop through directories passing them to grade_specific_assignment
}


void create_all_test_cases()
{
    //grab all sub directories
    
 
    //loop through directories passsing them to specific test case generation function
}
