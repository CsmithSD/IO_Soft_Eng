#include <fstream>
#include <sstream>
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
#include <algorithm>


const int MAX_LIST = 200;
const int MAX_TEST = 100;

/**********************************************************************************
 *                               FUNCTION PROTOTYPES
 *********************************************************************************/
bool run_test_case( std::string test_file, std::string exec,
        std::string &log_file,const std::string &rootDir );
void Compil( std::string progName );
std::string FinalLogWrite( std::string &log_file, int numPassed, int numTest );
void LogWrite( std::ofstream & fout, std::string testNumber, std::string result );
void DirCrawl( std::string rootDir , std::string &logFile , std::string exec ,
        int &passed , int &tested, const std::string &masterRootDir);
void generateTst(std::string choice);
void prompt();
void sysProg(std::string test);
void cppDirCrawl( std::string curDir, std::vector< std::string > &cppFiles, const std::string &masterRootDir );
void nameLogFiles( std::vector< std::string > &logNames, time_t timer,
                   std::vector< std::string > &studentNames);
void nameExec( std::string &exec );
bool critTest(std::string curDir, std::string logFile, std::string exec, 
              std::string pass_fail);

bool runCritTst(std::string critTst , std::string exec, std::string logFile);
bool check_if_cpp_file(char name[]);
void cleanUpGeneratedTests();
void writeSummaryLog( std::string student_name, std::string result,
                      time_t timer );
void critLogWrite( std::string log_file_name, bool passed_crit_tests, 
                   std::string test_file_name );


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
    std::string critAccepted = "PASSED";
    char cCurrentPath[FILENAME_MAX];
    time_t timer;
    std::string filename;
    std::string percent;
    std::string cppFile;
    std::vector< std::string > cppFiles;
    std::vector< std::string > studentNames;
    int i;

    //Asks the user if they want tst files generated
    prompt();

    //get current working directory, place in cCurrentPath
    getcwd(cCurrentPath , sizeof(cCurrentPath) );

    //finds all the cpp files below current directory
    cppDirCrawl(std::string(cCurrentPath), cppFiles, std::string(cCurrentPath));
    
    //creates a copy of the vector of strings holding the cpp files
    logNames = std::vector< std::string >(cppFiles);

    //getting the current time
    time( &timer );

    //Modifies strings to use student directory name in log file name.
    nameLogFiles( logNames, timer, studentNames );

    //compile the code
    //Passing the root directory of this program
    //and the .cpp or .C file to be tested
    for( auto cppFile : cppFiles )
        Compil(cppFile);


    //get directory to executable in string
    i = 0;
    std::cout <<"INSIDE FOR LOOP NOW!!!!!!!!!!" <<std::endl << std::endl;
    for( auto exec : cppFiles )
    {
        nameExec(exec);
        if( critTest(std::string(cCurrentPath) + "/tests/", logNames[i],
                    exec, critAccepted ))
        {
            //find and run test cases
            DirCrawl( std::string(cCurrentPath), logNames[i], 
                      exec , passed , tested, std::string(cCurrentPath) );

            //write final output to logfile
            percent = FinalLogWrite(logNames[i], passed, tested);
            writeSummaryLog(studentNames[i], percent,timer);
        }
        else
            writeSummaryLog(studentNames[i], "FAILED",timer);
    }
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
        std::string &log_file, const std::string &rootDir)
{
    std::string out_file = test_file;
    std::string ans_file = test_file;
    std::string test_num = "";
    std::string command_string = "";
    std::ofstream fout;
    int i=0;
    int result;
    int found;
    fout.open(log_file, std::ios::app | std::ios::out);
    found = test_file.find("Program_Tester_Generated_test");

    if(found != std::string::npos )
    {
        found = test_file.find_last_of( "t" );
        ans_file = rootDir+"Program_Tester_Generated_test";

        for (i = found+1; test_file[i] != '.'; i++)
            ans_file+=test_file[i];

        ans_file+=".ans";
    }
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
        LogWrite(fout, test_num,"passed");
        fout.close();
        return true;
    }
    //failed test
    LogWrite(fout, test_num,"failed");
    fout.close();
    return false;
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
std::string FinalLogWrite( std::string &log_file, int numPassed, int numTest )
{
    std::string tmp;
    std::ofstream fout;
    fout.open(log_file, std::ios::app | std::ios::out);
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
    fout.close();
    tmp = std::to_string(perPassed) + "%";
    return tmp;
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
void DirCrawl( std::string rootDir, std::string &logFile , std::string exec,
               int &passed, int &tested, const std::string &masterRootDir )
{
    struct dirent** file;	// File entry structure from dirent.h
    std::string filename;	//used in finding if a file has the extention .tst
    int n;
    std::cout << filename << std::endl;
    n =scandir(rootDir.c_str(), &file, 0, alphasort);
    if(n == -1)
        return;

    // Read each file one at a time
    // Readdir returns next file in the directory, returns null if no other files exist
    for(int i = 2; i < n; i++)
    {
        //place file name into string filename for easier checking
        filename = std::string(file[i]->d_name);

        // checks if the file is a subdirectory, 4 is the integer idetifyer
        // for the dirent struct on Lixux systems
        if(filename != "." && filename != ".." &&  (int)file[i]->d_type == DT_DIR )
        {
            //moves into the sub-directory
            DirCrawl( rootDir + filename + "/" , logFile , exec , passed , tested,
                          masterRootDir);
        }
        else if(filename.find("_crit.tst") == std::string::npos)
        {
            // checks if the file has a .tst in it. string find returns
            // string::npos if the substring cannot be found
            if ( filename.find( ".tst") != std::string::npos )
            {
                std::cout << "RUNNING TEST: "+filename+" \n \n \n ";
                // pass the file onto the grader 
                if (run_test_case( rootDir + filename , exec , logFile,
                                                masterRootDir) )
                {
                    passed += 1;
                }
                tested += 1;
            }
         }
    }
    for( int i = 0; i < n; i++)
        delete []file[i];
    delete []file;

}



void cppDirCrawl( std::string curDir, std::vector< std::string > &cppFiles, const std::string &masterRootDir)
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
    for(i=2; i<n; i++)
    {
        //checks if the file found is a .cpp file
        if( curDir != masterRootDir && namelist[i]->d_type == DT_REG && check_if_cpp_file( namelist[i] -> d_name ))
        {
            cppFiles.push_back( curDir + std::string( namelist[i] -> d_name ) );
        }
        //Checks if the current spot is the tests directory and skips it
        else if( namelist[i] -> d_type == DT_DIR)
        {

            tmp = std::string(namelist[i] -> d_name);
            cppDirCrawl(curDir + "/" + tmp+"/", cppFiles, masterRootDir);
        }
    }
    for( i = 0; i < n; i++)
        delete []namelist[i];
    delete []namelist;
}

bool check_if_cpp_file(char name[])
{
    std::string tmp = std::string(name);
    int found;

    found = tmp.find(".cpp",tmp.length()-4);
    if(found != std::string::npos)
    {
        return true;
    }
    return false;
}

void nameLogFiles( std::vector< std::string > &logNames, time_t timer, 
                   std::vector < std::string > &studentNames )
{
    std::string tmp = "";
    int i;
    //iteraties through the vector creating strings for all log files.
    for(auto logName : logNames)
    {
        //iterates through a string removing everything after the last occurence of '/'
        //and stops when it is found
        for( i = logName.length() - 1; logName[i] != '/'; i--)
            logName.pop_back();
        //moves the access pointer one spot to the last occurrence of '/'
        i--;
        //stores the student name in tmp up to the second to last '/'
        while(logName[i] != '/')
        {
            tmp += logName[i];
            i--;
        }
        std::reverse( tmp.begin(), tmp.end() );
        studentNames.push_back(tmp);
        //appends a .log to tmp and appends the file name to string
        tmp+="_" + std::string( ctime( &timer ) ) + ".log";
        logName+=tmp;
        //sets tmp as an empty string for next string in vector that needs
        //to be modified
        tmp = "";
    }
}

void nameExec( std::string &exec )
{
    int i;
    for( i = exec.length()-1;exec[i]!= '/'; i--)
        exec.pop_back();
    exec+="./a.out";
}

bool critTest(std::string curDir, std::string logFile, std::string exec,
              std::string pass_fail )
{
    struct dirent **namelist; //structure in dirent.h stores the file name 
    //and the file id number
    int n;
    static bool critResult = true;
    std::string tmp;
    int i;
    //scans the current directory for all 
    //types stores how many are found in n and the names in namelist
    n = scandir(curDir.c_str(), &namelist, 0, alphasort);
    if(n == -1)
        return critResult;
    //starts at the second position since 
    //the first two files found are the . and .. directories
    for(i = 2; i<n; i++)
    {
        if(std::string(namelist[i] -> d_name).find("_crit.tst") != std::string::npos)
        {
            if(runCritTst( std::string(namelist[i] -> d_name), exec, logFile))
            {
                    critResult = false;
                    pass_fail = "FAILED";
            }
        }
        else if( (int)namelist[i] -> d_type == 4 )
            critResult = critTest(curDir + std::string(namelist[i] -> d_name)
                                  + '/', logFile, exec, pass_fail);
    }
    for( i = 0; i < n; i++ )
        delete []namelist[i];

    delete []namelist;
    return critResult;
}

bool runCritTst( std::string critTst, std::string exec, std::string logFile) 
{
    std::string out_file = critTst;
    std::string ans_file = critTst;
    std::string test_num = "";
    std::string command_string = "";
    int result;


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
    command_string = exec + " < " + critTst + " > " + out_file;
    //execute the program
    std::system(command_string.c_str());

    //compare the programs output and the expected output( .out and .ans )
    // diff --ignore-all-space case.out case.ans > nul
    //if it == 0 the files were the same
    // the --ignore ignores whitespace on each line, so trailing spaces
    // or newlines aren't flagged as incorrect
    // > pipes the output into a file called nul
    command_string = "diff --ignore-all-space " + out_file + " " + ans_file 
                                                           + " > nul";
    result = std::system(command_string.c_str());
    
    command_string = "rm " + out_file;
    std::system(command_string.c_str() );

    if(result == 0)
    {
        critLogWrite( logFile, true, critTst );
        return false;
    }
    critLogWrite( logFile, false, critTst );
    return true;

}

/******************************************************************************//**
 * @author Jarod Hogan
 *
 * @Description
 * A method to generate a random list of integers or floats as decided by the user.
 * When the list has been generated it is run against a program called 'golden.cpp'.
 * This program is assumed to be in the current directory. It is run by calling 
 * sysProg.
 * 
 * @param[in] max - Length of list
 * @param[in] choice - either int or float. This is checked when method is called.
 * 
 * @returns none
 *********************************************************************************/
void generateTst(std::string choice)
{
  std::ofstream fout;
  int randNum,i,j;
  double randNum2;
  std::string filePath = "./tests/";
  std::string name = "Program_Tester_Generated_test";
  std::string ext = ".tst";
  std::string num;
  std::string filename;
  std::ostringstream convert;
  int max1, max2, temp;

  srand(time(NULL));  
  max1 = rand() % MAX_TEST + 1;

  for(j = 0; j < max1; j++)
  {
    temp = j;
    convert << temp;
    num = convert.str();
    convert.str("");
    filename = filePath + name + num + ext;
    fout.open(filename.c_str());
    if(!fout)
    {
      std::cout << "Could not open " << filename << std::endl;
    }
    if(choice == "int")
    {
        max2 = rand() % MAX_LIST + 1;

        for(i= 0; i < max2; i++)
        {
          //generate rand number
          randNum = rand();

          fout << randNum << std::endl;
   
        }
    }//end if
    else
    {
      //initialize random seed
      srand(time(NULL));  
      max2 = rand() % MAX_LIST + 1;

      for(i= 0; i < max2; i++)
      {
        //generate rand number
        randNum = rand();
        randNum2 = rand();
 
        while(randNum2 > 0)
        {
          randNum2 /= 10;
        }
        randNum2 = double(randNum) + randNum2;
    
        fout << randNum2 << std::endl;
      }//end For
    }//end Else

    fout.close();
  
 
    sysProg(filename);
  }//end for loop
}

/******************************************************************************//**
 * @author Jarod Hogan
 *
 * @Description
 * This function compiles and runs the golden.cpp (which is specified by the user). 
 * The output is redirected to the answer (testfilename.ans) file for the 
 * cooresponding test file.
 * 
 * @param[in] test - the name of the test file generated above
 * 
 * @returns none
 *********************************************************************************/
void sysProg(std::string test)
{
  std::string compile = "g++ -o test golden.cpp ";
  std::string run = "./test";
  std::string answer = test.substr(8, test.size());
  answer = answer.substr(0, answer.size()-3) + "ans";

  system(compile.c_str());
  run = run + " < " + test + " > " + answer;

  system(run.c_str());
 
}


/******************************************************************************//**
 * @author Jarod Hogan
 *
 * @Description
 * This function prompts the user if she wants to generate her own test cases and
 * run them against the golden cpp.
 * 
 * @returns none
 *********************************************************************************/
void prompt()
{
  char ans;
  std::string choice;
  bool flag = false;


  do
  {
    std::cout << "Would you like to generate test cases?(Y(y)/N(n)): ";
    std::cin >> ans;

    ans = tolower(ans);
    switch(ans)
    { 
      case 'y':
        std::cout << "Enter choice: ";
        std::cin >> choice;
        if((choice != "int") && (choice != "float"))
        { 
          std::cout << "Bad Choice" << std::endl;
          break;
        }
        
        generateTst(choice);
        flag = true;
        break;
      case 'n':
        flag =true;
        break;
      default:
        std::cout << "Your choice " << ans << " is not a valid option." << std::endl;
        std::cout << "Valid options are Y,y,N,n only!" << std::endl;
    }
  }while(flag == false);
  
}


/*****************************************************************************
 * @Author: Charles Parsons
 *
 * @Description Writes to the student log file for the results of the critical
 *              tests.
 *
 * @Params
 *
 * ***************************************************************************/
void critLogWrite( std::string log_file_name, bool passed_crit_tests, 
                   std::string test_file_name )
{
    std::ofstream fout;
    int pos;
    std::string temp_test_file_name = test_file_name;

    //parse test file name
    pos = temp_test_file_name.find_last_of( '/' );
    if( pos != std::string::npos )
    {
       temp_test_file_name = temp_test_file_name.substr( pos + 1 ); 
    }
    else
    {
        std::cerr << "Could not get test file name." << std::endl;
        return;
    }
    
    //open class log summary file for appending
    fout.open( log_file_name, std::ios::out | std::ios::app );

    if( !fout )
    {
        std::cerr << "Failed to open student log file." << std::endl;
    }

    //if critical tests are not passed, write student name and "FAILED"
    //to log summary file
    if( !passed_crit_tests )
    {
        //write to student log file
        //test_name FAILED
        fout << std::setw( 50 ) << temp_test_file_name << std::setw( 20 ) <<
                "FAILED" << std::endl;
    }
    else
    {
        //write to student log file
        //test_name PASSED
        fout << std::setw( 50 ) << temp_test_file_name << std::setw( 20 ) << 
                "PASSED" << std::endl;
    }

    fout.close();
}

/*****************************************************************************
 * @Author: Charles Parsons
 *
 * @Description: Writes a line to the class summary log file. This is an
 *              entry for a single student.
 *
 * @Param[in] student_name - the name of the student
 * @Param[in] result - a string with either "FAILED" if they failed one or more
 *                     critical tests or a percentage representing the tests
 *                     the student passed
 * @Param[in] root_directory - a string with the root directory
 *
 *
 ****************************************************************************/

void writeSummaryLog( std::string student_name, std::string result,
                      time_t timer )
{
    std::ofstream fout;
    std::string summary_file_name;

    summary_file_name = "class_summary_" + std::string(ctime(&timer)) + ".log";
    
    for(int i = 14; i < summary_file_name.length(); i++)
    {
        if(summary_file_name[i]==' ')
            summary_file_name[i] = '_';
    }

    fout.open( summary_file_name.c_str(), std::ios::app | std::ios::out );

    fout << std::setw( 50 ) << student_name << std::setw( 20 ) << result <<
            std::endl;
 
    fout.close();
}

/******************************************************************************
 * @Author: Charles Parsons
 *
 * @Description: Removes all generated test and answer files
 *
 * ***************************************************************************/
void cleanUpGeneratedTests()
{
    std::string command_string;

    //the command string for removing all of our generated test cases and
    //their answer files
    command_string = "rm Program_Tester_Generated*";

    //use the command string
    system( command_string.c_str() );

    std::cerr << "This house is clean." << std::endl;
}
