#include <fstream>
#include <string>
#include <dirent.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>
#include <time.h>
#include <vector>


/**********************************************************************************
 *                               FUNCTION PROTOTYPES
 *********************************************************************************/
bool run_test_case( std::string test_file, std::string exec,
        std::ofstream &log_file );
void Compil( std::string progName );
void FinalLogWrite( std::ofstream & fout, int numPassed, int numTest );
void LogWrite( std::ofstream & fout, std::string testNumber, std::string result );
void DirCrawl( std::string rootDir , std::ofstream &logFile , std::string exec ,
        int &passed , int &tested );
void cppDirCrawl( std::string curDir, std::vector< std::string > &cppFiles );
void nameLogFiles( std::vector< std::string > &logNames, time_t &timer);
void nameExec( std::string &exec );
void critTest(std::string curDir, std::string logFile, std::string exec std::string pass_fail );
bool runCritTst( string(namelist[i]->d_name), exec, logFile);
/******************************************************************************//**
 *
 *
 *********************************************************************************/
int main() /*int argc , char** argv  )*/
{
    int passed = 0;
    int tested = 0;
    std::ofstream logfile;
    std::vector< std::string > logNames;
    std::string root;
    std::string exec;
    char cCurrentPath[FILENAME_MAX];
    time_t timer;
    DIR* dir;
    struct dirent* file;
    std::string filename;
    bool foundFlag = false;
    std::string cppFile;
    std::vector< std::string > cppFiles;
    int i;

    //get current working directory, place in cCurrentPath
    getcwd(cCurrentPath , sizeof(cCurrentPath) );

    //finds all the cpp files below current directory
    cppDirCrawl(string(cCurrentPath), cppFiles)

        //creates a copy of the vector of strings holding the cpp files
        logNames = std::vector(cppFiles);

    //getting the current time
    time( &timer );

    //Modifies strings to use student directory name in log file name.
    nameLogFiles(logNames, timer);

    //compile the code
    //Passing the root directory of this program
    //and the .cpp or .C file to be tested
    for( cppFile : cppFiles )
        Compil(cppFile);


    //get directory to executable in string
    i = 0;
    for( exec : cppFiles )
    {
        nameExec(exec)
            //find and run test cases
            DirCrawl( string(cCurrentPath) + "/tests/" , logNames[i] , exec , passed , tested );

        //write final output to logfile
        FinalLogWrite(logNames[i],passed,tested);
        //close logfile
        logfile << "--------------------" << std::endl;
        logfile.close();
    }
    //remove junk files
    system("rm a.out");
    system("rm nul");

    return 0;
}

/******************************************************************************//**
 * @author Andrew Koc
 * 
 * @Description
 * This function takes two strings, one the directory to a test case file, and
 * the second the directory to an executable program.  It assumes the name and
 * location of the .ans file related to the test file.  The function then makes
 * a system call to run the program piping the input from the .tst file and the
 * output to the .out file.  After this another system command is called
 * that compares the .out file to the .ans file and calls the function to write
 * to the log file with the appropriate data.
 *
 * @param[in] - test_file the directory of the .tst file
 * @param[in] - exec the directory of the executable program to be tested
 * @param[out] - log_file the ofstream to the log file
 *
 * @returns true - the program passed the test
 * @returns false - the program failed the test
 *
 *********************************************************************************/
bool run_test_case( std::string test_file, std::string exec,
        std::ofstream &log_file )
{
    std::string out_file = test_file;
    std::string ans_file = test_file;
    std::string test_num = "";
    std::string command_string = "";
    int i;
    int result;

    //get test number
    //name for the test file will be "*case###.tst" so the last number is at
    //position length - 5
    for( i = test_file.length() - 5; test_file[i] >= '0' && test_file[i] <= '9'; i-- )
        //since we are reading in backward the new number gets added at the front
        test_num = test_file[i] + test_num;

    //get text for .out file and .ans file
    //remove tst
    out_file.resize(out_file.size() - 3);
    //add out so we have case#.out
    out_file += "out";

    //remove tst
    ans_file.resize(out_file.size() - 3);
    //add ans so we have case#.ans
    ans_file += "ans";

    //command string = "executable < case.tst > case.out"
    //run the program with input from .tst and pipe output to .out
    command_string = exec + " < " + test_file + " > " + out_file;
    //execute the program
    std::system(command_string.c_str());

    //compare the programs output and the expected output( .out and .ans )
    // diff --ignore-all-space case.out case.ans > nul
    //if it == 0 the files were the same
    // the --ignore ignores whitespace on each line, so trailing spaces
    // or newlines aren't flagged as incorrect
    // > pipes the output into a file called nul
    command_string = "diff --ignore-all-space " + out_file + " " + ans_file + " > nul";
    result = std::system(command_string.c_str());

    //passed test
    if ( result == 0 )
    {
        LogWrite(log_file, test_num,"passed");
        return true;
    }
    //failed test
    else
    {
        LogWrite(log_file, test_num,"failed");
        return false;
    }
}

/******************************************************************************//**
 * @author Jonathan Tomes
 * 
 * @Description
 * A simple funciton that compiles the program, given by progname
 * in the form of a .cpp file, with g++ to default compiled program
 * a.out.
 * 
 * @pram[in] progName - The name of the .cpp file for the source code.
 *                      (should include .cpp at the end.)
 *
 * @returns none
 *********************************************************************************/
void Compil( std::string progName )
{
    //creat the system command and execute it.
    std::string command;
    command = "g++ " + progName;
    system( command.c_str() );

    return;
}

/******************************************************************************//**
 *@Author Jonathan Tomes
 *
 *@Description
 *A function to right the final results of testing to a
 * log file, with the given ofstream.
 *
 *@param[in] fout - the output stream to write to.
 *@param[in] numPassed - the number of passed test cases.
 *@param[in] numTest - the total number of test cases.
 *
 *@returns none
 *********************************************************************************/
void FinalLogWrite( std::ofstream & fout, int numPassed, int numTest )
{
    //Calculate the number of tests failed.
    int numFailed;
    numFailed = numTest - numPassed;
    //Calculate the percent passed.
    float perPassed;
    perPassed = (float) numPassed/numTest;
    perPassed =  (int)(perPassed * 100);
    //Calculate the percent failed.
    float perFailed;
    perFailed = (float) numFailed/numTest;
    perFailed = (int)(perFailed * 100);

    //Write to stream.
    fout << "Percent of tests Passed: " << perPassed <<  "%" << std::endl;
    fout << "Percent of tests failed: " << perFailed << "%" << std::endl;
    return;
}

/******************************************************************************//**
 *@author Jonathan Tomes
 *
 *@description
 *A function to write the results of a test case
 * to the given file stream. 
 *
 *@param[in] fout - the output stream to write to.
 *@param[in] testNumber - the number of the test case, i.e. test #1, test #2, etc.
 *@param[in] status - The result of the test, passed or fail
 *
 *@returns none
 *********************************************************************************/
void LogWrite( std::ofstream & fout, std::string testNumber, std::string result )
{
    fout << testNumber << ": " << result.c_str() << std::endl;
    return;

}

/******************************************************************************//**
 * @author Erik Hattervig
 *
 * @Description
 * A recursive file traversal that gets a root directory and traverses through
 * all sub-directories looking for .tst files. When a tst file is found the
 * function will call for it to be tested
 * 
 * @param[in] rootDir
 * @param[in,out] logFile - a filestream to the logFile so that is passed on
 * to the tester functions
 * @returns none
 *********************************************************************************/
void DirCrawl( std::string rootDir , std::ofstream &logFile , std::string exec , int &passed , int &tested )
{
    DIR* dir = opendir( rootDir.c_str() );	// Open the directory
    struct dirent* file;	// File entry structure from dirent.h
    std::string filename;	//used in finding if a file has the extention .tst

    // Read each file one at a time
    // Readdir returns next file in the directory, returns null if no other files exist
    while( ( file = readdir(dir)) != NULL )
    {
        //place file name into string filename for easier checking
        filename = file->d_name;

        // skip over the directories "." and ".."
        if ( filename != "." && filename != ".." )
        {
            // checks if the file is a subdirectory, 4 is the integer idetifyer
            // for the dirent struct on Lixux systems
            if ( (int)file->d_type == 4 )
            {
                //moves into the sub-directory
                DirCrawl( rootDir + filename + "/" , logFile , exec , passed , tested );
            }
            else if(filename.find("_crit.tst") == std::string::npos)
            {
                // checks if the file has a .tst in it. string find returns
                // string::npos if the substring cannot be found
                if ( filename.find( ".tst") != std::string::npos )
                {
                    // pass the file onto the grader 
                    if (run_test_case( rootDir + '/' + filename , exec , logFile ) )
                    {
                        passed += 1;
                    }
                    tested += 1;

                }
            }
        }
    }

    closedir(dir);

    return;
}



void cppDirCrawl( std::string curDir, std::vector< std::string > &cppFiles )
{
    struct dirent **namelist; //structure in dirent.h stores the file name 
    //and the file id number
    int n;
    std::string tmp;
    int i;
    //scans the current directory for all 
    //types stores how many are found in n and the names in namelist
    n = scandir(file.c_str(), &namelist, 0, alphasort);
    if(n == -1)
        return;
    //starts at the second position since 
    //the first two files found are the . and .. directories
    for(i=2; i<n; i++)
    {
        //checks if the file found is a .cpp file
        if(check_if_cpp_file( namelist[i] -> d_name ))
        {
            cppFiles.pushback( string( namelist[i] -> d_name ) )
        }
        //Checks if the current spot is the tests directory and skips it
        else if( check_if_tests_dir( namelist[i] -> d_name ) )
        {
            continue;
        }
        else
        {
            tmp = string(namelist[i] -> d_name);
            cppDirCrawl(curDir + "/" + tmp, cppFiles);
        }

    }
    for( i = 0; i < n; i++)
        delete []namelist[i];
    delete []namelist;
}

bool check_if_cpp_file(char name[])
{
    std::string tmp = string(name);
    int found;

    found = tmp.find(".cpp");

    if(found == std::string::npos)
        return false;
    return true;
}

bool check_if_tests_dir(char name[])
{
    std::string tmp = string(name);
    int found;

    found = tmp.find("tests");

    if(found == std::string::npos)
        return false;
    return true;
}

nameLogFiles( std::vector< std::string > &logNames, time_t &timer )
{
    string tmp = "";
    int i;
    //iteraties through the vector creating strings for all log files.
    for(logName : logNames)
    {
        //iterates through a string removing everything after the last occurence of '/'
        //and stops when it is found
        for( i = logName.length - 1; logName[i] != '/'; i--)
            logName.pop_back();

        //moves the access pointer one spot to the last occurrence of '/'
        i--;
        //stores the student name in tmp up to the second to last '/'
        while(logName[i] != '/')
        {
            tmp += logName[i];
            i--;
        }
        //appends a .log to tmp and appends the file name to string
        tmp+="_" + std::string( ctime( timer ) ) + ".log";
        logName+=tmp;
        //sets tmp as an empty string for next string in vector that needs to be modified
        tmp = "":
    }
}

void nameExec( std::string &exec )
{
    int i;
    for( i = exec.length-1;exec[i]!= '/'; i--)
        exec.pop_back();
    exec+="./a.out"
}

void critTest(std::string curDir, std::string logFile, std::string exec std::string pass_fail )
{
    struct dirent **namelist; //structure in dirent.h stores the file name 
    //and the file id number
    int n;
    std::string tmp;
    int i;
    //scans the current directory for all 
    //types stores how many are found in n and the names in namelist
    n = scandir(curDir.c_str(), &namelist, 0, alphasort);
    if(n == -1)
        return;
    //starts at the second position since 
    //the first two files found are the . and .. directories
    for(i = 2; i<n && pass_fail != "FAILED"; i++)
    {
        if(string(namelist[i] -> d_name).find("_crit.tst") != std::string::npos)
        {
            if(runCritTest( std::string(nameList[i] -> d_name), exec, logFile)
                    pass_fail = "FAILED";
        }

        else if( (int)namelist[i] -> d_type == 4 )
            critTest(curDir + string(namelist[i] -> d_name) + '/', logFile, exec, pass_fail);
    }
    for( i = 0; i < n; i++ )
        delete []namelist[i];

    delete []namelist;
}

bool runCritTst( std::string critTst, std::string exec, std::string logFile)
{
    
}
         
