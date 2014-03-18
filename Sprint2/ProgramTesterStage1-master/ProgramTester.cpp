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


/**********************************************************************************
 *                               FUNCTION PROTOTYPES
 *********************************************************************************/
bool run_test_case( std::string test_file, std::string exec,
                    std::ofstream &log_file );
void Compil(  std::string root, std::string progName );
void FinalLogWrite( std::ofstream & fout, int numPassed, int numTest );
void LogWrite( std::ofstream & fout, std::string testNumber, std::string result );
void DirCrawl( std::string rootDir , std::ofstream &logFile , std::string exec ,
               int &passed , int &tested );

/******************************************************************************//**
 *
 *
 *********************************************************************************/
int main( int argc , char** argv )
{
    int passed = 0;
    int tested = 0;
    std::ofstream logfile;
    std::string logName;
    std::string root;
    std::string exec;
    char cCurrentPath[FILENAME_MAX];
    time_t timer;
    DIR* dir;
    struct dirent* file;
    std::string filename;
    bool foundFlag = false;
    std::string cppFile;

    //check that an argument was passed
    if ( argc < 2 )
    {
        std::cout << "No argument was passed to test.\nExiting Program"
                     << std::endl;
        return 0;
    }

    //get current working directory, place in cCurrentPath
    getcwd(cCurrentPath , sizeof(cCurrentPath) );
    //get root directory in string
    
    root = argv[1];

    //Find the ".cpp"
    dir = opendir( root.c_str() );
    while( ( file = readdir(dir) ) != NULL && !foundFlag )
      {
        //Get the file name
        filename = file -> d_name;
        //skip over "." and ".."
        if( filename != "." && filename != ".." )
          {
        if(filename.find( ".cpp" ) != std::string::npos )
          {
            cppFile = filename;
            foundFlag = true;
          }
          }
      }


    if( !foundFlag )
      {
        std::cout << "Could not find a cpp file." << std::endl;
	std::cout << "Ending Program" << std::endl;
        return 0;
      }
    
    //Get the name for the log file
    logName = root + '/' + cppFile;
    //Remove the extension from the file name, keep the '.'
    while(logName[logName.length() - 1] != '.')
        logName.resize(logName.length() - 1);
    //add the log to the file name
    logName += "log";

    //open log file and append to it
    logfile.open(logName.c_str(), std::ofstream::app);

    //Check that logfile was opened
    if ( !logfile )
    {
        std::cout << "Log file could not be opened\nExiting Program"
                     << std::endl;
        return 0;
    }

    //getting the current time
    time( &timer);
    //Just need to print it out to the log file first thing.
    logfile << "--------------------" << std::endl;
    logfile << ctime( &timer ) << std::endl;

    //compile the code
    //Passing the root directory of this program
    //and the .cpp or .C file to be tested
    Compil(root, cppFile);

    
    //get directory to executable in string
    exec = cCurrentPath;
    exec += "/a.out";

    //find and run test cases
    DirCrawl( root + '/' , logfile , exec , passed , tested );

    //write final output to logfile
    FinalLogWrite(logfile,passed,tested);

    //close logfile
    logfile << "--------------------" << std::endl;
    logfile.close();

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
 * a.out. The source code should be in the directory provided
 * by root.
 * 
 * @parm[in] root - directory to find the program.
 * @pram[in] progName - The name of the .cpp file for the source code.
 *                      (should include .cpp at the end.)
 *
 * @returns none
 *********************************************************************************/
void Compil(  std::string root, std::string progName )
{
  //Create the argument to send to g++
  std::string progPath = "";
  progPath += root;
  progPath += "/";
  progPath += progName;
  //creat the system command and execute it.
  std::string command;
  command = "g++ " + progPath;
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
			else
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

















