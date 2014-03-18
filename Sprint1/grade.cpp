/***********************************************************************

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
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string>
#include <dirent.h>
#include <cstring>

using namespace std;

//function prototypes
double calc_percentage_f(int passes, int fails);
double calc_percentage_p(int passes, int fails);
bool check_if_dot(char name[]);
bool check_if_file(char name[]);
void findTestFile( string file, const string &program, int &pass, int &fail,
                   ofstream &fout );
void log_file(ofstream &fout, string log_line, string pass_fail);
void output_check();
void redirected_check();
bool test(const string &program, string file, string a_file, 
                string tst_file,string &log_line);

/************************************************************************
   Function: Main( int argc, char* argv[] )
   Author:  Charles, Jarod, Chris
   Description: This is the start of the program. It compiles the input cpp 
                file and then 
                calls the findTestFile function and finally after that is 
                complete the passes 
                and fails are outputed to the log file
   Parameters:  argc is the number of command line arguments
                argv is the cpp file to be compiles in argv[1]
 ************************************************************************/
int main( int argc, char* argv[] )
{
    string fileName; //store the file name without the extention
    string command; //store the command for the system call
    int pos; //store the position of the "." 
             //for removing the file extention
    ofstream fout;
    int pass = 0;
    int fail = 0;
    string tmp;
    double passes_percent;
    double fails_percent;



    //make sure the target program's file was included on command line
    if( argc != 2 )
    {
        cerr << "Usage: grade file_name.cpp" << endl;
        return -1;
    }
    //strip the file extention and resave into fileName
    fileName = argv[1];
    pos = fileName.find_last_of( "." );
    fileName = fileName.substr( 0, pos );

    tmp =fileName + ".log";

    fout.open( tmp.c_str() );
    //put together the command for the system call
    command = "g++ -o " + fileName + " " + argv[1];

    //make system call with a c-style string version of command
    system( command.c_str() );

    fileName = "./" + fileName;

    //finds files with .tst endings and runs them with compiled program
    findTestFile( ".", fileName, pass, fail, fout);

    //calculates percentages
    passes_percent = calc_percentage_p(pass, fail);
    fails_percent = calc_percentage_f(pass,fail);

    //outputs the passes, fails and percentages to log file.
    fout << "total number of passes: " << pass << '\n';
    fout << "total number of fails: " << fail << '\n';
    fout << "percentage of passes: " << passes_percent << "%" << "\n";
    fout << "percentage of fails: " << fails_percent<< "%" << "\n";
    fout.close();
    return 0;
}

/************************************************************************
   Function:calc_percentage_f(int passes, int fails)
   Author:Chris Smith
   Description:calculates the percentage of fails
   Parameters:  passes number of passes on testing files
                fails number of fails on testing files
 ************************************************************************/
double calc_percentage_f(int passes, int fails)
{
    return (double) fails/(passes+fails)*100.0;
}

/************************************************************************
   Function:calc_percentage_p(int passes, int fails)
   Author:Chris Smith
   Description:Calculates the percentage of passes
   Parameters:  passes number of passes on testing files
                fails number of fails on testing files
 ************************************************************************/
double calc_percentage_p(int passes, int fails)
{
    return (double) passes/(passes+fails) * 100.0;
}

/************************************************************************
   Function:check_if_dot(char name[])
   Author:Chris Smith
   Description: This function checks if the file is a directory or .ans 
                file and returns true if it is
   Parameters:  name[] is a char array that is converted to a string for 
                simple checks
 ************************************************************************/
bool check_if_dot(char name[])
{
    string tmp = string(name);
    int found;
    if(tmp == ".")
        return true;
    else if( tmp == ".." )
        return true;

    found = tmp.find(".ans");
    if(found != string::npos)
        return true;
    return false;
}

/************************************************************************
   Function:check_if_file(char name[])
   Author:  Chris Smith
   Description: This function checks if the file is a .tst file
   Parameters:  name[] is a char array that is converted to a string 
                for a simple check
 ************************************************************************/
bool check_if_file(char name[])
{
    string tmp = string(name);
    int found;

    found = tmp.find(".tst");
    if(found == string::npos)
        return false;
    return true;

}

/************************************************************************
   Function:findTestFile(string file, const string&program, int&pass, 
                         int &fail, ofstream &fout)
   Author:Chris Smith, Jarod Hogan
   Description: This function searches the current directory for .tst 
                files or nested unhidden directories and recursively 
                searches through the directories for .tst files
   Parameters:  file is the current directory the search is in
                program is the program string used in system calls to run 
                        against .tst files
                pass keeps track if a .tst file passed
                fail keeps track if a .tst file fails
                fout is the .log file that is outputted
 ************************************************************************/
void findTestFile(string file, const string &program, int &pass, int &fail, ofstream &fout)
{
    ifstream fin;
    struct dirent **namelist; //structure in dirent.h stores the file name 
                              //and the file id number
    int n;
    string ans_file;
    string log_line;
    string tmp;
    string actual_out;
    int i;
    //scans teh current directory for all 
    //types stores how many are found in n and the names in namelist
    n = scandir(file.c_str(), &namelist, 0, alphasort);
    if(n == -1)
        return;
    //starts at the second position since 
    //the first to files found are the . and .. directories
    for(i=2; i<n; i++)
    {
        //checks if the file found is a .tst
        if(check_if_file( namelist[i] -> d_name ))
        {

            tmp = string(namelist[i]->d_name);
            ans_file = tmp.substr(0,tmp.find_last_of('.') ) + ".out";

            if(test(program, file,ans_file,tmp,log_line))
            {
                log_file(fout, log_line,"pass");
                pass++;
            }
            else
            {
                log_file(fout, log_line, "fail");
                fail++;
            }
            
        }
        //if the file is not a .tst but is a .ans  
        //or . .. it skips those files
        else if(check_if_dot(namelist[i] -> d_name ))
        {
            continue;
        }
        //goes into this else if the file is a directory
        else
        {
            tmp = string(namelist[i] -> d_name);
            findTestFile(file + "/" + tmp, program, pass, fail, fout);
        }
    }
    for(i = 0; i < n; i++)
        delete []namelist[i];
    delete []namelist;
}

/************************************************************************
   Function:log_file(ofstream &fout, string log_line, string pass_fail)
   Author:Chris Smith
   Description: this function outputs a line to the log file
   Parameters:  fout is the file to output to .log
                log_line is the string outputed to the file <case#, actual 
                         output, expected output>
                pass_fail is the string passed to say if the case 
                          passed/failed
 ************************************************************************/
void log_file(ofstream &fout, string log_line, string pass_fail)
{
    fout << log_line << pass_fail << endl;
}

/************************************************************************
   Function:test(const string &program, string file,string a_file, string 
                                               tst_file, string &log_line)
   Author: Chris Smith
   Description: This function will run a tst file with the compiled cpp 
                file and compare the outputs of redirected output to a 
                .out file with a .ans file by running a diff command
   Parameters:program is the current program string to run
              file is the current directory
              a_file is the .out file and changed multiple times to get 
                     the .ans file
              tst_file is the .tst file string
              log_line is the string that will be modified for adding to 
                       the .log file
 ************************************************************************/
bool test(const string &program, string file,string a_file, string tst_file, string &log_line)
{
    ifstream fin;
    string tmp;
    string tmp_ans;
    
    //runs the .cpp program with redirected input and output
    system(string(program + " < " +  file +  "/"+ 
                    tst_file + " > " + file + "/" + a_file).c_str());
    log_line = tst_file;

    //adds the outputed answer to the log line
    fin.open(string(file+"/"+a_file.substr(0,a_file.find_last_of('.') )
                            + ".out").c_str() );
    getline(fin, tmp);
    fin.close();
    log_line += " " + tmp + " ";

    //adds the .ans expected answer to the log line
    tmp_ans = file + "/" + a_file.substr(0,a_file.find_last_of('.') ) 
                   + ".ans";
    fin.open( tmp_ans.c_str() );
    getline(fin, tmp);
    fin.close();
    log_line += " " +  tmp + " ";

    //runs a diff command on the .ans and .out files and returns true if 
    //they are not different
    if(system(string("diff " + file+"/"+ a_file + " " + file + "/" 
                    + a_file.substr(0,a_file.find_last_of('.') ) 
                    + ".ans").c_str()) == 0)
        return true;

    return false;
}
