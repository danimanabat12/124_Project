#include<iostream>
#include<vector> 
#include<cctype>
#include<cstring>
#include<string>
#include<sstream>
#include<stack>
#include<algorithm>
#include<cmath>
#include<cstdlib>

using namespace std; 

//typedef struct for the masterlist of variables
typedef struct variables {                                                                          
    string name; 
    string value; 
    string type; 
}variables;

//helper functions
int string_size(string input);
bool isOperator(char ch);
bool isKeyword(string input);
bool containsNoChar (string input);
bool isValid(string input);
bool isUsed(vector<variables> var_list, string input);
bool isFloat(string input);
bool isInteger(string input);
bool isOperand(string input);
bool comparison(string input1, string input2);
void updateDataType (vector<variables> &var_list, string var_name, string data_type);
string extractDataType (vector<variables> &var_list, string var_name);

//tokenizing function
vector<string> processInput(string input);

//error-checking function
bool errorChecking(vector<string> user_command); 
bool processElements(vector<string> user_command, vector<variables> &var_list);

//processing functions
void variable_history(vector<variables> &var_list, vector <string> &user_command);
string evaluate(string &input, vector <variables> &var_list);
string concatenate(vector<string> user_command, int starting_point);
void semantics(vector <variables> &var_list, vector <string> &user_command);

//arithmetic functions
string arithmetic(string); 
string infixToPostfix(string);
bool isValidOperation(char);
int getPrecedence(char);
vector<string> tokenize(string);
int getState(char&);
float solvePostfix(const vector<string>&);
float performOp(char, float, float);
enum states{REJECT = 0, INTEGER, REAL, NEGATIVE, OPERATOR, UNKNOWN, SPACE};

int main() {
    string input;
	vector<variables> var_list;
	
    cout << "The SNOL environment is now active, you may proceed with giving your commands.\n";

    while(input != "EXIT!") {
        cout << "\nCommand: ";
        getline(cin, input);
       	vector<string> user_command = processInput(input);  
		if (!user_command.empty()) {
			if (errorChecking(user_command)) {
				if (processElements(user_command, var_list)) {
					//The reason nganong nagbuhat ko ani is because naay error something sa imong code van if for example: var lang ang gi-input sa user
					//mag throw siyag exception. So ang gibuhat nako, gi-modify nako akong processElements na function in a way nga pag isa lang ang naa sa vector,
					//dili na siya mag process.  
					if(user_command.size() > 1) {
						variable_history(var_list, user_command); 
		         		semantics(var_list, user_command);  
					}
				}
			}		
		}      
    }

    cout << "\nInterface is now terminated..." << endl;
}

//main function to be called to solve expression before converting answer to string
string arithmetic(string expression){
	float floatAns = solvePostfix(tokenize(infixToPostfix(expression)));	//solve for answer which is returned as float
	//convert float answer to string
	ostringstream ostr;
	ostr<<floatAns;
	string stringAns = ostr.str();
	return stringAns;	//return string answer	
}

//function that converts infix string to postfix string
string infixToPostfix(string infix){
    string postfix;
    stack<char> charStack;
    unsigned i;
    // negative numbers now symbolized by '~' to avoid confusion
    for(i = 0; i < infix.length(); i++){
        if(infix[i] != '-')
			continue;
		else if(i + 1 < infix.length() && isValidOperation(infix[i + 1]))
			continue;
        if (i == 0 || isValidOperation(infix[i - 1])|| infix[i - 1] == '(' )
            infix[i] = '~';
    }
 
    for (i = 0; i < infix.length(); i++){
        //numbers moved to postfix string
        if ((isdigit(infix[i])) || (infix[i] == '.') || (infix[i] == '~'))
            postfix += infix[i];
        //ignore spaces
        else if (isspace(infix[i]))
            continue;
        //if we get valid operators
        else if (isValidOperation(infix[i])){
            postfix += " ";
            //while stack isn't empty, check if operator in stack has higher precedence than the current one from infix. If so, move stack operator to postfix string
            while ((!charStack.empty()) &&(getPrecedence(charStack.top()) >= getPrecedence(infix[i]))){
                postfix += charStack.top();
                charStack.pop();
            }
            //push the current infix operator to top of stack
            charStack.push(infix[i]);
        }
        // push opening parentheses to stack
        else if (infix[i] == '(')
            charStack.push(infix[i]);
        //when closing parenthesis is encountered
        else if (infix[i] == ')'){
            //pop operator from top of stack until encounter another opening parenthesis or stack is emptied
            while ((!charStack.empty()) && (charStack.top() != '(')){
                postfix += charStack.top();    // place the math operator onto the postfix string
                charStack.pop();
            }
            if (!charStack.empty()) //if stack isn't empty, it means opening parenthesis is on top so pop it
                charStack.pop();
            else{
                cout << "\nERROR #5: Mismatched Parenthesis\n";
                exit(1);
            }
        }
        else{
            cout << "\nERROR #1: Invalid character in expression\n";
            exit(1);
        }
    }
 
    // move remaining math operators (if any) from the stack to postfix array
    while (!charStack.empty()){
        postfix += charStack.top();
        charStack.pop();
    }
    return postfix;
}// end of function infixToPostfix
 
//function that checks if an operation is valid
bool isValidOperation(char operation){
    switch (operation){
	    case '+': case '-': case '*': case '/': case '%': 
	        return true;
	        break;
	    default:
	        return false;
	        break;
    }
}// end of function isValidOperation

//function that returns a designated precedence for valid operators
int getPrecedence(char operation){
    int priority = 0;
    switch (operation){
		case '*': case '/': case '%':
	        priority = 3;
	        break;
	    case '-':
	        priority = 2;
	        break;
	    case '+':
	        priority = 1;
	        break;
    }
    return priority;
}// end of function getPrecedence

//function that tokenizes a postfix string
vector<string> tokenize(string postfix){
	//Finite State Machine(FSM)
	int stateTable[][7] = { {0, INTEGER,  REAL, NEGATIVE, OPERATOR,  UNKNOWN,  SPACE},
							{INTEGER,  INTEGER,  REAL,   REJECT,  REJECT,   REJECT,  REJECT},
							{REAL,       REAL,  REJECT,  REJECT,  REJECT,   REJECT,  REJECT},
							{NEGATIVE, INTEGER,  REAL,   REJECT,  REJECT,   REJECT,  REJECT},
							{OPERATOR,  REJECT, REJECT,  REJECT,  REJECT,   REJECT,  REJECT},
							{UNKNOWN,   REJECT, REJECT,  REJECT,  REJECT,   UNKNOWN, REJECT},
							{SPACE,     REJECT, REJECT,  REJECT,  REJECT,   REJECT,  REJECT} };
		
    vector<string> tokens;
    char currChar = ' ';
    int col = REJECT, currState = REJECT;
    string currToken = "";

    for (unsigned i = 0; i < postfix.length();){
        currChar = postfix[i];
        col = getState(currChar);	// get designated column of current character
        if ((currState == REAL) && (col == REAL)){
            cerr << "\nERROR #2: multiple decimal points\n";
            exit(1);
        }
        currState = stateTable[currState][col];	//get current state according to FSM
        //REJECT means successfully tokenized
        if (currState == REJECT){
            if (currToken != " ")
                tokens.push_back(currToken);
            currToken = "";
        }
        //otherwise, we keep building next token until successful
        else{
            currToken += currChar;
            i++;
        }
    }
    //double check to make sure last token is saved
    if (currToken != " ")
        tokens.push_back(currToken);
    return tokens;
}// end of function Lexer

//function that returns the state of a character being tokenized
int getState(char &currChar){
    if(isspace(currChar))
        return SPACE;
    else if(isdigit(currChar))
        return INTEGER;
    else if(currChar == '.')
        return REAL;
    else if(currChar == '~'){
		currChar = '-';
        return NEGATIVE;
    }
    else if (isValidOperation(currChar))
		return OPERATOR;
    return UNKNOWN;
}// end of function getState

//function that solves the now postfix expression
float solvePostfix(const vector<string>& postfix){
    float op1 = 0, op2 = 0;
    float answer = 0;
    stack<float> floatStack;

    for (unsigned i = 0; i < postfix.size(); i++){
        //push positive numbers and decimal points to stack
        if ((isdigit(postfix[i][0])) || (postfix[i][0] == '.'))
            floatStack.push(atof(postfix[i].c_str()));
        //push negative numbers onto the stack
        else if ((postfix[i].length() > 1) && ((postfix[i][0] == '-') && (isdigit(postfix[i][1]) || (postfix[i][1] == '.'))))
            floatStack.push(atof(postfix[i].c_str()));
        //if expression is an operator, pop numbers from stack and send them to function performOp
        else if (isValidOperation(postfix[i][0]) && (!floatStack.empty())){
            char token = postfix[i][0];
 
            if (floatStack.size() > 1){
                op2 = floatStack.top();
                floatStack.pop();
                op1 = floatStack.top();
                floatStack.pop();
                answer = performOp(token, op1, op2);
                floatStack.push(answer);
            }
        }
        else{
            cout << "\nERROR #3: Idk what happened lmao but something went VERY WRONG\n";
            exit(1);
        }
    }
    // pop the final answer from the stack, and return to main
    if (!floatStack.empty())
        answer = floatStack.top();
    return answer;
}// end of solvePostfix

//helper function for solvePostfix to perform individual operations
float performOp(char operation, float op1, float op2){
    float result;
    switch (operation){
	    case '+':
	        result = op1 + op2;
	        break;
	    case '-':
	        result = op1 - op2;
	        break;
	    case '*':
	        result = op1 * op2;
	        break;
	    case '/':
	        result = op1 / op2;
	        break;
	    case '%':
	        result = ((int)op1 % (int)op2) + modf(op1, &op2);
	        break;
	    default:
	    	cout << "\nERROR #4: Idk what happened but something went VERY WRONG hahaha\n";
	        exit(1);
	        break;
    }
    return result;
}// end of function performOp

void variable_history(vector<variables> &var_list, vector <string> &user_command){
	if(user_command[0]== "BEG" && isUsed(var_list, user_command[1])){			//for cases of beg which is an elaborate assignment operator, exceptions are made
		var_list.push_back(variables());
		var_list.at(var_list.size()-1).name = user_command.at(1);
	}
	
	if(!isUsed(var_list, user_command[0]) || isKeyword(user_command[0])) return;	//to prevent submitting keywords as variables as well as variable resubmission 
	

	if(user_command.at(0) != "" && isValid(user_command.at(0))){  					//if first element is variable, save it to the masterlist, assume its an assignment statement
		var_list.push_back(variables());
		var_list.at(var_list.size()-1).name = user_command.at(0);
	}

}

int string_size(string input){														//string size
	int i;
	for(i=0; input[i] != '\0';) i++;	
	return i;
}


int search(string &input, string &varname ){ cout<<"entering search: "<<input<<endl;
	int size = string_size(input);	
	for(int i=0; i < size;i++){ 
		string holder = "";
		
		//concatenate until an operator is found
		while(1){
			holder=holder + input[i];
			
			if(input[i]=='(')break;
			
			i++;
			if(i == size|| isOperator(input[i]) || input[i] == '(' || input[i] == ')'){ cout<<input[i]<<endl;
				break;
			}
		
		}
		
		cout<<"current variable: "<<holder<<endl;
		if(holder == varname){		//if equal return the subscript (the adress of the first char of the variable to be replaced)
			return i-string_size(holder);
		}
	
	}	
	return -1; //returns -1 if none found
}


string evaluate(string &input, vector <variables> &var_list){						//replaces all variables with their corresponding values for easier solving
	int i=0,j=1;
	
	cout<<input<<endl;
	while(i < j){

		j=string_size(input);			
		if(!isdigit(input[i]) && !isOperator(input[i])){											//if it is not a digit and not an operator
			//check for Mod
			string mod = "MOD";
			int found = search(input, mod);													//made specifically for MOD keyword, replace it with %
			if(found != -1){
				input.replace(found, found+2, "%");
				continue;
			}
			
			int k=0, l=0;
			while(k < var_list.size()){																//find and replace all present variables in the string
				//check if current variable is present on the input string
								
				int found = search(input, var_list[k].name);
				cout<<input[i]<<endl;
				if(found != -1){
					 
					input.replace(found, string_size(var_list[k].name), var_list[k].value);
				
				}else{
					k++;
				}
				
			}			
		}
		i++;
	}
	
	cout<<"post eval:"<<input<<endl;
	return input;
}


string concatenate(vector<string> user_command, int starting_point){							//	 given a starting point, combines all desparate strings from vector into 1 string
	int j = starting_point;
	string input ="";
		for(j; j< user_command.size(); j++){
				
				input = input + user_command[j];
			}
	return input;
}
	
void semantics(vector <variables> &var_list, vector <string> &user_command){					//due to nature of switch only accepting int and does not handle string well, if are used throughout

	if(!isUsed(var_list, user_command[0]) && user_command.at(1) == "="){						//check if assignment, variable in first element, '=' on the second
		int i=0;
	
		while(i < var_list.size()){																//search for the variable specified
			if(var_list[i].name == user_command[0]) {											//once found, concatenate the rest into 1 r-value, evaluates it and assigns it to var
				string input = concatenate(user_command, 2);
				var_list[i].value = arithmetic(evaluate(input, var_list));
				//place arithmetic function here
				
				if(!isInteger(var_list[i].value))var_list[i].type = "Float";					//determine if data type is float or integer
				else var_list[i].type = "Int";
				
				
				cout<<"datatype of "<<var_list[i].name<<" is "<<var_list[i].type<<endl;
				cout<<"Post arithmetic: "<<	var_list[i].value<<endl;
				break;	
			}
			i++;
		}		
	}
	//check if keyword
	else if(isKeyword(user_command[0])){
		if(user_command[0] == "PRINT"){
			string input = concatenate(user_command, 1);
			input = evaluate(input, var_list);

			cout<<input<<endl;
		}
		
		else if(user_command[0] == "BEG"){ 
			int i=0;
			string input;

			while(i < var_list.size()){
				if(var_list[i].name == user_command[1]) {
					cout<<"Please enter value for ["<<user_command[1]<<"]"<<endl<<"Input:";
					getline(cin, input);
					
					//place arithmetic function here
					var_list[i].value = arithmetic(evaluate(input, var_list));
					cout<<"Post arithmetic: "<<	var_list[i].value<<endl;
					
					if(!isInteger(var_list[i].value))var_list[i].type = "Float";					//determine if data type is float or integer
					else var_list[i].type = "Int";
					
					
					fflush(stdin);
					break;
				}
				i++;
			}			
		}		
	}
	//if not an assignment nor a keyword, assume an arithmetic
	else{
		string temp = concatenate(user_command, 0);
		temp = evaluate(temp, var_list);
		cout<<temp<<endl;
		temp = arithmetic(temp);
		cout<<"Post arithmetic: "<<temp<<endl;
		//place arithmetic function here
	}
}

bool errorChecking(vector<string> user_command){
	
	bool stringChecker=false;	//stringChecker returns a bool value to indicate if the user-input is valid or not
	int openParenthesis=0;		//counts the number of open parentheses on the expression
	int closeParenthesis=0;		//counts the number of close parentheses on the expression
	
	//cout << "Vector size: " << user_command.size() << endl;
	
	if(user_command.size()==1){	//if the size of the vector is 1, 
		
		if(user_command.at(0)!="PRINT" && user_command.at(0)!="BEG"){	//user input an expression with vector of size 1
			
			string tempString = user_command.at(0);	//assign the value of the vector on a temporary string
			
			//cout << "tempString length: " << tempString.length() << endl;
			//cout << "tempString size: " << tempString.size() << endl;
	
			if(isdigit(tempString[0])||tempString[0]=='('){	//if the first character of the string is an open parenthesis or digit, execute this code snippet

				for(int j=0;j<tempString.size();j++){	//for loop scans every character of the string , checks for operators, parenthesis and integers
					if(isdigit(tempString[j])||tempString[j]=='+'||tempString[j]=='-'||tempString[j]=='*'||tempString[j]=='/'||tempString[j]=='%'||tempString[j]=='('||tempString[j]==')'||tempString[j]=='.'){
						if(tempString[j]=='(')openParenthesis++;	//if an open parenthesis is encountered, increment
						else if(tempString[j]==')')closeParenthesis++; //if a close parenthesis is encountered, increment
						//cout << "position [" << j << "]" << "(1)true character: " << tempString[j] << endl;
						stringChecker=true;	//return true if string contains valid characters
					}else{

						//cout << "position [" << j << "]" << "(1)false character: " << tempString[j] << endl;
						stringChecker=false;	//return false if string contain invalid characters
						break;
					}
				}
			}else if(isalpha(tempString[0])||tempString[0]=='_'){ //if the string starts with letters or underscore

				for(int j=0;j<tempString.size();j++){	//scan every character of the string
					if(isalnum(tempString[j])||tempString[j]=='_'){	//checks for alphanumeric characters and underscore
						//cout << "position [" << j << "]" << "(2)true character: " << tempString[j] << endl;
						stringChecker=true;	//return true if valid characters are met
					}else{
						//cout << "position [" << j << "]" << "(2)false character: " << tempString[j] << endl;
						stringChecker=false; // return false if invalid
						break;
					}
				}
			}else
				//terminate called after throwing an instance of 'std::out_of_range'
  				//	what():  vector::_M_range_check: __n (which is 1) >= this->size() (which is 1)
  				//error occured at processElements()
			
			if(stringChecker==true) cout << "Simply specifying a simple expression" << endl;	//if string checker is true, print this statement
			else cout << "Unknown Word [" << tempString << "]" << endl;	//if stringchecker is false, print the unknown word
			
		}else if(user_command.at(0)=="EXIT!")exit(0);	//if user input is EXIT, terminate the function immediately
	}else if(user_command.size()==2&&(user_command.at(0)=="PRINT"||user_command.at(0)=="BEG")){	//if the vectors has a size of two
		
		if(user_command.at(0)=="BEG"){	//user calls the BEG function
			
			string tempString=user_command.at(1);	//initialize the second string on a temporary string
			
			for(int j=0; j<tempString.length(); j++){																//for loop that scans each character for variable initialization anomalies
				if(isdigit(tempString[0])){																			//checks if the first character of the variable is an integer										
					stringChecker = false;																			//return a false value, indicating invalid variable					
					break;																							//break the loop
				}else if(isalnum(tempString[j]) || tempString[j]=='_'){												//checks the string if it contains alphanumeric characters
						stringChecker = true;																		//return a true value, indicating valid variable
				}else{
					stringChecker = false;																			//the loop encountered and invalid character, returns a false value
					break;																							//break out of the loop
				}
			}
			
			if(stringChecker==false) cout << "Unknown Word [" << tempString << "]" << endl; //if stringchecker is false, print the unknown word
			
			
		}else if(user_command.at(0)=="PRINT"){ //user calls the PRINT function
			
			string tempString=user_command.at(1); //initialize the second string on a temporary string
			
				if(isdigit(tempString[0])){		 																		//if the first character of the string is an integer/digit, execute this code snippet
					for(int j=0; j<tempString.length(); j++){															//for loop scanning every character of the string from the vector
						if(isdigit(tempString[j])||tempString[j]=='.'){													//if the loop encounters an integer or floating-point, it returns a true value
							stringChecker = true;
						}else{	
							stringChecker = false;																		//if the loop encounters an invalid character, it returns a false value
							break;																						//break out of the loop
						}
					}
				}else{																									//if the first character of the string is not an integer/digit, execute this code snippet
					for(int j=0; j<tempString.length(); j++){															//for loop scanning every character of the string from the vector
						if(isalnum(tempString[j]) || tempString[j]=='_'){												//if the loop encounters an alphanumeric character, it returns a true value
							stringChecker = true;
						}else{																							
							stringChecker = false;																		//if the loop encounters an invalid character, it returns a false value
							break;																						//break out of the loop
						}
					}
				}
				if(stringChecker==false) cout << "Unknown Word [" << tempString << "]" << endl;	//if stringchecker is false, print the unknown word
			
		}else cout << "Unknown Command! Does not match any valid command of the language" << endl; //if the user inserted an invalid command, print this statement
				
	}else{ //assignment operation
		
		int equalCount=0; //counts instances the 'equal' sign is used
		int operatorCount=0;//counts instances operator signs are used
		
		for(int i=0;i<user_command.size();i++){	//for loop that scans every string inside the vector
			
			string tempString = user_command.at(i); //assign the current vector inside a temporary string
			
			if(user_command.at(0)=="+"||user_command.at(0)=="-"||user_command.at(0)=="*"||user_command.at(0)=="/"||user_command.at(0)=="%"||user_command.at(0)=="="){
				cout << "entered if statement" << endl;
				stringChecker=false;
				break;
				
			}else{
				
				cout << "entered else statement" << endl;
				if(isalpha(tempString[0])){	//if the string starts with a letter
					for(int j=0;j<tempString.size();j++){	//for loop scans for alphanumeric characters and underscore
						if(isalnum(tempString[j])||tempString[j]=='_'){
							//cout << "position [" << j << "]" << "(1)true character: " << tempString[j] << endl;
							stringChecker=true; // return true if valid characters are met
						}
						else{
							//cout << "position [" << j << "]" << "(1)false character: " << tempString[j] << endl;
							stringChecker=false; //return false if invalid characters are encountered
							break;	//break out of the loop
						}
					}
				}else if(isdigit(tempString[0])||tempString[0]=='('||tempString[0]==')'){ //if the string starts with an integer, and parenthesis
					for(int j=0;j<tempString.size();j++){ //for loop scans the string for valid characters
						if(isdigit(tempString[j])||tempString[j]=='+'||tempString[j]=='-'||tempString[j]=='*'||tempString[j]=='/'||tempString[j]=='%'||tempString[j]=='('||tempString[j]==')'||tempString[j]=='.'){
							
							if(tempString[j]=='+'||tempString[j]=='-'||tempString[j]=='*'||tempString[j]=='/'||tempString[j]=='%')operatorCount++;//if an operator is encountered, increment
							if(tempString[j]=='(')openParenthesis++; //if open parenthesis is encountered, increment
							else if(tempString[j]==')')closeParenthesis++; //if close parenthesis is encountered, increment
							//cout << "position [" << j << "]" << "(2)true character: " << tempString[j] << endl;
							stringChecker=true; //return true value if valid character is met
						}
						else{
							//cout << "position [" << j << "]" << "(2)false character: " << tempString[j] << endl;
							stringChecker=false; //return false if invalid character is encountered
							break;	//break out of the loop
						}
					}
					
				}else if(tempString[0]=='='){ //if the vector contained an equal sign
					//cout << "Vector [" << i << "]" << "(3)true character: " << tempString << endl;
					equalCount++; //increment per equal signs encountered
				}else if(tempString[0]=='+'||tempString[0]=='+'||tempString[0]=='*'||tempString[0]=='/'||tempString[0]=='%'){ //if the vector contain operators
					operatorCount++;
					//cout << "Vector [" << i << "]" << "(4)true character: " << tempString << endl;
					stringChecker=true; //return true for every valid characters met
				}
				
			}	
		}
		
		if(equalCount==0&&operatorCount==0)stringChecker=false;
		
		if(equalCount>1&&openParenthesis>1&&closeParenthesis>1){//enter this if statement if the expression has equal sign and parenthesese
			
			if((equalCount==1||equalCount==0)&&openParenthesis==closeParenthesis){ //if the amount of equal sign does not exceed one, and the number of parenthesis are equal, the expression is valid and true
				stringChecker=true;
			}else stringChecker=false; //return otherwise
			
		}	

		if(stringChecker==false) cout << "Unknown Command! Does not match any valid command of the language" << endl; //print this statement if the stringchecker is false

	}
	
	return stringChecker;	
}

bool processElements(vector<string> user_command, vector<variables> &var_list) {				//boolean function that checks the data type of the input and the variables's validity
														
	int i; 
	string input; 												//declaration of string inputs and some were initialized empty strings
	string bin = ""; 
	string dataType; 
	string var_name = "";
	string toRemove = "()";											//initialize parenthesis in this string variable 
	bool encounteredEqual = false; 										//initialize boolean functions as false 
	bool encounteredOperator = false;	
	bool undefinedVars = false; 		
	bool encounteredKeyword = false;
	vector<string> stripedInput;										//declaration of vector variables
	vector<string> undefined; 
	vector<string> varDataTypes; 
	
	
	for (i = 0; i < user_command.size(); i++) {								//iterate the user_command at its maximum size
		input = user_command.at(i); 									//store the each element in the string input
		
		for (int j = 0; j < 2; j++) {									//scan each element 
			input.erase(remove(input.begin(), input.end(), toRemove[j]), input.end()); 		//erase unnecessary characters
		}
		if (input != "") {										//if no invalid characters pushback the input to the vector stripedInput
			stripedInput.push_back(input);
		}
	}
	if (stripedInput.size() == 1) { 									//if the vector is 1 return true
		if (isValid(stripedInput.at(0)) && isUsed(var_list, stripedInput.at(0))) {			//and if the variable is valid but the variable was not defined
			cout << "SNOL> Error! [" << user_command.at(0) << "] is not defined!" << endl;		//print error because variable is not defined
			return false;										//return false
		}
		return true;											
	}
	
	if (isOperator(stripedInput.at(0)[0]) || stripedInput.at(0) == "=") {					//if the element of the vector is an operator or equal sign
		cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;	//print  unknown command
		return false; 											//return false
	}
	
	for (i = 0; i < stripedInput.size(); i++) {								//scan the element of the stripedInput vector
		input = stripedInput.at(i);									//store each element in the string input
		
		if (stripedInput.size() > 3 && input.size() == 1 && i > 1 && !isOperator(stripedInput.at(i)[0]) && !isOperator(stripedInput.at(i-1)[0])) {
			cout << input << endl;
			cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
			return false; 
		}
		
		if (stripedInput.size() > 2 && isValid(input) && i > 1 && (!isOperator(stripedInput.at(i-1)[0]) && stripedInput.at(i-1) != "=")) {
			cout << input << endl;
			cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
			return false; 
		}

		if(isValid(input) && !isKeyword(input)) {								//if the variable is valid and not a keyword
			if(!isUsed(var_list, input)) {									//also if the variable was not already used
				varDataTypes.push_back(extractDataType(var_list, input));				//push the variable to the vector varDataTypes
			}
			else {
				if (i > 0 || isOperator(stripedInput.at(i+1)[0])) {					//else invalid variable
					undefined.push_back(input);							//push it to another vector called undefined
					undefinedVars = true;								//undefinedVars is true because it is invalid
				} 
			}
		}
		
		else if (input == "=") {										//if the input has encountered an equal sign
			encounteredEqual = true;									
		}
		
		else if (!isValid(input) && containsNoChar (input)) {							//if it is invalid variable and has only alphanumeric string
			
			for (int j = 0; j < input.size(); j++) {							//scan the input
				if (isdigit(input.at(j))) {								//if there is a digit encountered
					bin += input.at(j);								//concatenates the string
				}
				
				else if (input.at(j) == '.') {								//if the input encounters a decimal point
					bin += input.at(j);								//concatenates the string
				}
				
				else {
					if (isOperator) {								//if there encountered an operator
						encounteredOperator = true;						//then it is true
					
					}
					if(!isOperand(bin) && bin != "") {						//if the string is not an empty tring and not an operand
						cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; //not a numeric data type
						return false;								//return false
					}
					if (!isInteger(bin) && bin !="") {						//if not an empty string and not an integer
						varDataTypes.push_back("Float"); 					//push the string to the vector and assign as floating-point
						bin = "";								//reset the string
					}	
					else if (isInteger(bin) && bin != "") {						//if not an empty string and is an integer
						varDataTypes.push_back("Int");						//push the string to the vector and assign as an integer
						bin = "";								//reset the string
					}
				}
			}
			
			if(!isOperand(bin) && bin != "") {								//if not empty string but not an operand
				cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; 		//not a numeric data type
					return false;									//return false
				}
				if (!isInteger(bin) && bin !="") {							//if the string is not an integer
					varDataTypes.push_back("Float"); 						//push it to the vector as a floating-point
					bin = "";									//reset string
				}	
				else if (isInteger(bin) && bin != "") {							//if the string is an integer
					varDataTypes.push_back("Int");							//push it to the vector as integer
					bin = "";									//reset the string
				}
			}
		
		else if (isKeyword(input)) encounteredKeyword = true;							//if there is a keyword then set the boolean encounteredKeyword to true
	}
	
	if (undefinedVars == true ) {											//if the undefined variables are true
		if (undefined.size() == 1 && stripedInput.at(0) == "BEG") {;						//undefined size is 1 
		}
		else {
			cout << "SNOL> Error! Undefined variables: "; 							//print error of undefied variables
			for (int i = 0; i < undefined.size(); i++) {							//iterate the elements of the undefined
				cout << "[" << undefined.at(i) << "] ";			
			}
			cout << "\n"; 
			return false;											//return false
		}
	}
	
	if(encounteredEqual == true && !isUsed(var_list, stripedInput.at(0))) {						//there is an equal sign and the variable is unique
		varDataTypes.erase(varDataTypes.begin());								
	}
	
	if (varDataTypes.size() > 1) {
		for (int i = 0; i < varDataTypes.size() - 1; i++) {
			string str1 = varDataTypes.at(i);
			string str2 = varDataTypes.at(i+1);
			cout << str1 << " and " << str2 << endl;
			if(str1 != str2) {
				if (!(encounteredEqual == true && stripedInput.size() == 3 && encounteredOperator == false)) {
					cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!" << endl;
					return false;
				} 
			}
		}
	} 
	
	return true; 												//if no encountered errors then the variable is valid and the data type was determined
}

vector<string> processInput(string input) {                                                            		//function that process that user command
    vector<string> user_command;                                                                      		//vector of type string which stores the user command string into substrings
    string placeholder="";                                                                              
    int i = 0; 
    bool flag = true; 
    bool characterError = false;

    while (i < input.length()) {                                                                      		//runs until the length of user command string

        if (isalnum(input[i]) || input[i] == '.') {                                                   		//checks if the current character is alphanumeric or a dot
            if (isOperator(placeholder[placeholder.size()-1])) {                                        	//if yes, it firstly checks if the last character on the placeholder string is an operator   
                if(isdigit(input[i]) || input[i] =='(' || input[i] == ')' ) {                               //if the upper if-statement is evaluated, it checks if the current char is a digit
                    placeholder += input[i];                                                              	//if the current char is digit, it concatenates the digit to the placeholder string
                }
                else {
					if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}                                                                                  		//if the current char is not a digit, it pushes the placeholder onto the vector user_command
                    user_command.push_back(placeholder); 
                    placeholder = input[i];                                                            		//re-set the placeholder string to the current char
                }
            }
            else {                                                                                      	//else if the last charater on the placeholder string is not an operator
                placeholder += input[i];                                                                	//we concatenate the current char to the placeholder string
            }
        }
		
		else if (input[i] == '(' || input[i] == ')') {
			if (isValid(placeholder)) {
				user_command.push_back(placeholder); 
				if (isalpha(input[i+1])) {
						user_command.push_back(string(1, input[i]));
						placeholder = "";
					}
				else placeholder = input[i];
			}
			else if (isdigit(input[i+1]) || input[i+1] == '(' || input[i+1] == ')' || isOperator(input[i+1])) {
				placeholder += input[i];
			}
			else { 
				placeholder += input[i]; 
				user_command.push_back(placeholder);
				placeholder = ""; 
				
			}
		}
		
        else if (isOperator(input[i])) {                                                             		//checks if the current character is an operator
            if (containsNoChar(placeholder)) {                                                          	//if yes, we check if the placeholder string contains no character
                placeholder += input[i];                                                                 	//if it doesn't contains any character, we concatenate the current input to the placeholder string
            }                                                                                               
            else {                                                                                     		//else,
                if (placeholder != "") {                                                                 	//we check if the placeholder string is empty
                    if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}
					user_command.push_back(placeholder);                                                 	//if not, we push the placeholder to the user_command vector
                    user_command.push_back(string(1, input[i]));                                         	//furthermore, we add the current operator to the user_command vector
                    placeholder = "";                                                                    	//reset the placeholder string to an empty string
                }
            }
        }

        else if (input[i] == '=') {                                                                    	 	//check if the current character is an equal sign
            if (placeholder != "") {                                                                     	//checks if the placeholder string is not empty
                if (characterError == true) {
					cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                   	user_command.clear();
                   	return user_command;
				}
				user_command.push_back(placeholder);                                                     	//place whatever in the placeholder to the user_command vector
            }
            user_command.push_back("=");                                                                 	//also, we push the equal sign to the vector
            placeholder = "";                                                                            	//reset the placeholder string to empty string
        }

        else if (input[i] == ' ') {                                                                    		//checks if the current character is a whitespace
            while (input[i] == ' ') {                                                                    	//This loop runs until the character is no longer a white space (eliminating extra ws)
                i++;                                                                                     	//increment i so we can access the next character in the input string
			}
			if (input[i] == '\0') {
				break;
			}
			
            if (isOperator(input[i])) {                                                                		//if the current character is an operator, same logic from line 47-58 applies here
                if (containsNoChar(placeholder)) {
                    placeholder += input[i];
                } 
                else {
                    if (placeholder != "") {
                    	if (characterError == true) {
							cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    		user_command.clear();
                    		return user_command;
						}
                        user_command.push_back(placeholder); 
                        user_command.push_back(string(1, input[i])); 
                        placeholder = ""; 
                    }
                }
            }
            else if (input[i] == '(' || input[i] == ')') { 
				if (isValid(placeholder)) {
					user_command.push_back(placeholder); 
					if (isalpha(input[i+1])) {
						user_command.push_back(string(1, input[i]));
						placeholder = "";
					}
					else placeholder = input[i];
				}
				else if (isdigit(input[i+1]) || input[i+1] == '(' || input[i+1] == ')' || isOperator(input[i+1])) {
					placeholder += input[i];
				}
				else {
					placeholder += input[i]; 
					user_command.push_back(placeholder);
					placeholder = ""; 
					
				}
			}
            else if (input[i] == '=') {                                                                  //check if the current character is an equal sign 
				if (placeholder != "") {  
					if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}                                                                   				 //checks if the placeholder string is not empty
                    user_command.push_back(placeholder);                                                 //place whatever in the placeholder to the user_command vector
                }
                user_command.push_back("=");                                                             //also, we push the equal sign to the vector
                placeholder = "";                                                                        //reset the placeholder string to empty string
            }
            else {                                                                                     	 //if the current character is not an operator
                if (!isOperator(placeholder[placeholder.size()-1]) && placeholder != "") {               //we check if the last char in the placeholder string is not an operator and the placeholder string is not empty
                    if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}
					user_command.push_back(placeholder);                                                 //if the condition are met, we push whatever the content of placeholder string to the vector 
                    placeholder = "";    
                    placeholder += input[i];                                                                                                                      
                }
                else if (isOperator(placeholder[placeholder.size()-1])) {                              				//else if the current character is an operator
                    if (!isdigit(input[i])) {                                                           			//check if current input is not a digit, then we push placeholder string to the vector
                        if (characterError == true) {
							cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
	                    	user_command.clear();
	                    	return user_command;
						}
						user_command.push_back(placeholder);                                             
                        placeholder = "";
                    }
                    placeholder += input[i]; 
                }  
                else if (isalnum(input[i])) {
                    placeholder += input[i];
                }
				else if (input[i] == '\0') {}  															//Ignore the string terminator character.                                                   
                else {
                	characterError = true; 
        			placeholder += input[i];
                }
            }
        }
        else {
        	characterError = true; 
        	placeholder += input[i];
        }
        i++;
    }

    if (placeholder != "") {                                                                            			//If the placeholder string is not empty after the loop, we add whatever's left to the vector
        user_command.push_back(placeholder);
        placeholder = "";
    }
    
    
    for (int i = 0; i < user_command.size(); i++) {
    	if (isKeyword(user_command.at(i)) && i > 0) {
    		cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl; 
    		user_command.clear(); 
    		return user_command; 
		}
	}
    return user_command;
}

bool isOperator (char ch) {                                                                         					//boolean function that checks if a character is an operator
    if (ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '%') return true;
    else return false;
}

bool isKeyword(string input){
	if(input == "BEG" || input == "PRINT" || input == "MOD") return true;								//if the set variable is a keyword
	else return false;
}

bool containsNoChar (string input) {                                                                					//boolean function that checks if a string contains alphabet charaters
    for (int i = 0; i < input.size(); i++) {
        if (isalpha(input[i])) return false; 
    }
    return true; 
}

bool isValid(string input){
	int i = 0;
	int valid = true;																					//initialize valid as true	

		if(isalpha(input[i])) valid = true;												//if each element of the input has no unnecessary characters														//if the input is only alphanumeric	that start with a letter
		else valid = false;																				//else invalid
		
		if(valid == true){																				//while it is valid
		  	for(i = 1; i < input.size(); i++) {															//scan each element of the string
		  	   	if(!(isalpha(input[i]) || isdigit(input[i]) || input[i] == '_')){						//if there is unneccessary characters then invalid
					valid = false;
					break;
				}
		  	}
		  	   return true; 																			//if the variable is valid, it will return true
		  }
	    else return false;																				//else not
}

bool isUsed(vector<variables> var_list, string input) {
	int i=0;
	int valid = true;
	
	while(i < var_list.size()){
		if(var_list[i].name == input){
			return false;
		}		
		i++;
	}
	return true;
}

bool isInteger(string input){													//boolean function to test if the input is an integer
   	istringstream iss(input); 												//istringstream means to copy the value of the input
    int f; 															//declare an integer variable
    iss >> noskipws >> f; 													//produced an integer type
    return iss.eof() && !iss.fail(); 												//return true if the input is an integer
}

bool isFloat(string input) {													//boolean function to test id the input is a  floating-point
    istringstream iss(input); 													//istringstream means to copy the value of the input
    float f; 															//declare a floating-point variable
    iss >> noskipws >> f; 													//produced a floating-point type
    return iss.eof() && !iss.fail(); 												//return true if the input is a floating-point
}

bool isOperand(string input) {													//boolean function that checks the input if it is an operand
	bool flag = true; 													//initialize flag as true
	if(!isInteger(input) && !isFloat(input)) flag = false;									//if input is not an operand return false
	return flag; 														//return true if the input is an operand
}

bool comparison(string input1, string input2){											//boolean function that compares the data type of two inputs
	if(isInteger(input1) && isInteger(input2)) return true;									//return true if both inputs are integer
	else if(isFloat(input1) && isFloat(input2)) return true;								//return true if both inputs are floating-point
	else return false;													//else they are not the same data type
}

void updateDataType (vector<variables> &var_list, string var_name, string data_type) {
	for (int i = 0; i < var_list.size(); i++) {
		if (var_list.at(i).name == var_name) {
			var_list.at(i).type = data_type;
			break;
		}
	}
} 

string extractDataType (vector<variables> &var_list, string var_name) {
	for (int i = 0; i < var_list.size(); i++) {
		if (var_list.at(i).name == var_name) {
			return var_list.at(i).type;
		}
	}
	return "";
}
