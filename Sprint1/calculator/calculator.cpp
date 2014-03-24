#include <iostream>
using namespace std;
//-----------------------------------------
//  This program demonstrates the if statement.
//  There are 2 options - use a symbol or
//  use a letter for the operations
//  Things to note :
//   1. the compound if statements
//   2. use of the character functions
//   3. the use of the variable op_ok to
//    signal if an invalid operator was entered
//-----------------------------------------

int main()
{
float a, b;
char op;
float result;
int op_ok = 0;

cerr << "Enter first value : " ;
cin >> a;

cerr << "Enter the operation (A, a, or + for addition etc.) : ";
cin >> op;
if ( isalpha(op))
  op = toupper(op);  // could also do op = tolower(op);

cerr << "Enter second value : ";
cin >> b;

if ((op == 'A') || (op == '+'))
   {
   result = a + b;
	cout << result << endl;
   //op_ok = 1;
   }
else if ((op == 'S') || (op == '-'))
   {
   result = a - b;
 	cout << result << endl;
   //op_ok = 1;
	}
else if ((op == 'M') || (op == '*'))
   {
   result = a * b;
	cout << result << endl;
	//op_ok = 1;
   }
else if ((op == 'D') || (op == '/'))
   {
   if ( b == 0)
     cout << "Divide by zero error" << endl;
   else
     {
     result = a / b;
     cout << result << endl;
     }
   //op_ok = 1;
   }   
else  // if a valid operator hasn'tbeen recognized yet
    {
    cout << "Invalid operator " << endl;
    }



} // end program
