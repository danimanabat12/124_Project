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
//			if (errorChecking(user_command)) {
				if (processElements(user_command, var_list)) {
					variable_history(var_list, user_command);
	         		semantics(var_list, user_command);  
				}
//			}		
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


int search(string &input, string &varname ){
	int size = string_size(input);	
	for(int i=0; i < size;i++){ 
		string holder = "";
		
		//concatenate until an operator is found
		while(1){
			holder=holder + input[i];
			i++;
			if(i == size|| isOperator(input[i])){ cout<<input[i]<<endl;
				break;
			}
		}
		

		std::size_t found = holder.find(varname); //checks if the 2 strings (i.e. the inputted variable and the actual variable are the same)
		
		
		if(found !=std::string::npos){ //returns subscript of the start of the variable of the string
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
			
				if(found != -1){
					 
					input.replace(found, string_size(var_list[k].name), var_list[k].value);
					cout<<input<<endl;
				}else{
					k++;
				}
				
			}			
		}
		i++;
	}
	
	cout<<endl<<"post eval:"<<input<<endl;
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
				cout << var_list[i].value << endl; 
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
			//evaluate arithmetic here
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
					cout<<input<<endl;
					
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
		arithmetic(evaluate(user_command[0], var_list));
		//place arithmetic function here
	}
}


bool processElements(vector<string> user_command, vector<variables> &var_list) {
	
	int i; 
	string input; 
	string bin = ""; 
	string dataType; 
	string var_name = "";
	bool encounteredEqual = false; 
	bool encounteredOperator = false;
	bool undefinedVars = false; 
	
	
	vector<string> undefined; 
	vector<string> varDataTypes; 
	
	for (i = 0; i < user_command.size(); i++) {
		input = user_command.at(i);
		
		//Kulang panig cases for enclosed parentheses
		if (input != "(" && input != ")") {
//			if (user_command.size() > 3 && input.size() == 1 && i > 1 && !isOperator(user_command.at(i)[0]) && !isOperator(user_command.at(i-1)[0])) {
//				cout << input << endl;
//				cout << "hakdog" << endl; 
//				cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
//				return false; 
//			}
//			
//			if (user_command.size() > 3 && isValid(input) && encounteredEqual == true && i>1 && (!isOperator(user_command.at(i-1)[0]) && user_command.at(i-1) != "=")) {
//				cout << input << endl;
//				cout << "SNOL> Unknown command! Does not match any valid command of the language." << endl;
//				return false; 
//			}
		} 
		
		if(isValid(input) && !isKeyword(input)) {
			if(!isUsed(var_list, input)) {
				varDataTypes.push_back(extractDataType(var_list, input));	
			}
			else {
				if (i > 0 || isOperator(user_command.at(i+1)[0])) {
					undefined.push_back(input);
					undefinedVars = true;
				} 
			}
		}
		
		else if (input == "=") {
			encounteredEqual = true;
		}
		
		else if (!isValid(input) && containsNoChar (input)) {
			
			for (int j = 0; j < input.size(); j++) {
				if (isdigit(input.at(j))) {
					bin += input.at(j);
				}
				
				else if (input.at(j) == '.') {
					bin += input.at(j);
				}
				
				else {
					if (isOperator) {
						encounteredOperator = true;
					
					}
					if(!isOperand(bin) && bin != "") {
						cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; 
						return false;
					}
					if (!isInteger(bin) && bin !="") {
						varDataTypes.push_back("Float"); 
						bin = "";
					}	
					else if (isInteger(bin) && bin != "") {
						varDataTypes.push_back("Int");
						bin = "";
					}
				}
			}
			
			if(!isOperand(bin) && bin != "") {
				cout << "SNOL> [" << bin << "] is not a valid integer nor float!" << endl; 
					return false;
				}
				if (!isInteger(bin) && bin !="") {
					varDataTypes.push_back("Float"); 
					bin = "";
				}	
				else if (isInteger(bin) && bin != "") {
					varDataTypes.push_back("Int");
					bin = "";
				}
			}
		
		else if (isKeyword(input)) {
			;
		}
		//add og else diri na if wala sa mga if statements diha, edi naay mali na syntax
	}
	
	if (undefinedVars == true ) {
		if (undefined.size() == 1 && user_command.at(0) == "BEG") {;
		}
		else {
			cout << "SNOL> Error! Undefined variables: "; 
			for (int i = 0; i < undefined.size(); i++) {
				cout << "[" << undefined.at(i) << "] ";
			}
			cout << "\n"; 
			return false;
		}
	}
	
	if(encounteredEqual == true && !isUsed(var_list, user_command.at(0))) {
		varDataTypes.erase(varDataTypes.begin());
	}
	
	if (varDataTypes.size() > 1) {
		for (int i = 0; i < varDataTypes.size() - 1; i++) {
			string str1 = varDataTypes.at(i);
			string str2 = varDataTypes.at(i+1);
			cout << str1 << " and " << str2 << endl;
			if(str1 != str2) {
				if (!(encounteredEqual == true && user_command.size() == 3 && encounteredOperator == false)) {			//to be fixed for num = 2+3. Dapat di siya base sa size sa command.
					cout << "SNOL> Error! Operands must be of the same type in an arithmetic operation!" << endl;
					return false;
				} 
			}
		}
	} 
	
	return true; 
}

bool errorChecking(vector<string> user_command){																		//this function only checks user input for preliminary examinations
	
	cout << "SIZE: " <<user_command.size() << endl;																		//prints the size of the vector containing the input from user (for checking purposes)
	
	if(user_command.size()<=2){
		for(int i=0; i<user_command.size(); i++){																		//for loop that scans the entire vector
	
			bool stringChecker = false;																					//bool variable responsible for the identification if an input is valid/invalid
			
			if(user_command.size()==2 && user_command.at(i)=="BEG"){													//BEG function error-checker, proceeds if BEG function is called
				string tempString = user_command.at(i+1);																//assign the next string after the BEG keyword
				//cout << "Variable: " << tempString << endl;
				
				for(int j=0; j<tempString.length(); j++){																//for loop that scans each character for variable initialization anomalies
					if(isdigit(tempString[0])){																			//checks if the first character of the variable is an integer
						//cout << "Invalid Character" << endl;											
						stringChecker = false;																			//return a false value, indicating invalid variable					
						break;																							//break the loop
					}else if(isalnum(tempString[j]) || tempString[j]=='_'){																	//checks the string if it contains alphanumeric characters
							stringChecker = true;																		//return a true value, indicating valid variable
					}else{
						stringChecker = false;																			//the loop encountered and invalid character, returns a false value
						break;																							//break out of the loop
					}
				}
				if(stringChecker==true) cout << "Variable: " << tempString << "- Valid variable " << endl;				//for loop returns a true value - prints valid variable (for checking purposes)
					else {
						cout << "Unknown Word [" << tempString << "]" << endl;											//for loop returns a false value - prints the invalid input
						return false;
					} 
					break; //newly added - break out of the if-statement											
			}else if(user_command.size()!=2 && user_command.at(i)!="BEG"){												//else statement prints "Unknown Command! Does not match any valid command of the language"
				cout << "Unknown Command! Does not match any valid command of the language" << endl;					//this code snippet can be removed
				return false;
			}else if(user_command.size()==2 && user_command.at(i)=="PRINT"){												//PRINT function error-checker, proceeds if PRINT function is called
				
				string tempString = user_command.at(i+1);																//assign the next string after the PRINT keyword

				if(isdigit(tempString[0])){		 																		//if the first character of the string is an integer/digit, execute this code snippet
					for(int j=0; j<tempString.length(); j++){															//for loop scanning every character of the string from the vector
						if(isdigit(tempString[j])){																		//if the loop encounters an integer, it returns a true value
							stringChecker = true;
						}else{	
							stringChecker = false;																		//if the loop encounters an invalid character, it returns a false value
							break;																						//break out of the loop
						}
					}
					if(stringChecker==true) cout << "Variable: " << tempString << " - Valid Variable" << endl;			//for loop returns a true value - prints the valid variable (for checking purposes)
						else {
							cout << "Unknown Word [" << tempString << "]" << endl;										//for loop returns a false value - prints the invalid input
							return false;
						}
						break; //newly added - break out of the if-statement
				}else{																									//if the first character of the string is not an integer/digit, execute this code snippet
					for(int j=0; j<tempString.length(); j++){															//for loop scanning every character of the string from the vector
						if(isalnum(tempString[j]) || tempString[j]=='_'){																		//if the loop encounters an alphanumeric character, it returns a true value
							stringChecker = true;
						}else{																							
							stringChecker = false;																		//if the loop encounters an invalid character, it returns a false value
							break;																						//break out of the loop
						}
					}
					if(stringChecker==true) cout << "Variable: " << tempString << " - Valid Variable" << endl;			//for loops return a true value - prints the valid variable (for checking purpose)
						else {
							cout << "Unknown Word [" << tempString << "]" << endl;										//for loope returns a false value - prints the invalid input
							return false;
						}
						break; //newly added - break out of the else statement
				}	
			}else if(user_command.size()!=2 && user_command.at(i)!="PRINT"){												//else statement prints "Unknown Command! Does not match any valid command of the language"
				cout << "Unknown Command! Does not match any valid command of the language" << endl;					//this code snippet can be removed (convert to else instead of else if)
				return false;
			}else {
				cout << "Unknown Command! Does not match any valid command of the language" << endl;			//this code snippet can be removed (convert to else instead of else if)
				return false;																									//comment this out
			}
		}
	} else{ 																												//execute this code if the size of vector is more than 2
			string tempVariable = user_command.at(0);																	//store the first string - check variable
			string tempEqual = user_command.at(1);																		//store second string - check if the user used '='
			
			bool variableChecker = true;																				//bool variableChecker - return a value to determine if variable is valid/invalid
			bool stringChecker = false;																					//bool stringChecker - return a value to determine if the string is valid/invalid
			bool equalChecker = true;																					//bool equalChecker - returns a value to determine if the expression included '='
			
			int openParenthesis = 0;
			int closeParenthesis = 0;
			
//			cout << "tempVariable: " << tempVariable << endl;															//for checking purposes
//			cout << "tempEqual: " << tempEqual << endl;
//			cout << tempVariable[0] << endl;	
																		
			for(int i=0; i<tempVariable.length(); i++){																	//for loop that scans every character on the string tempVariable
				if(isdigit(tempVariable[0])){																			//if the tempVariable starts with a digit, it returns a false value
					variableChecker = false;																			//return a false value via variableChecker
					break;																								//break out of the loop
				}else if(isalnum(tempVariable[i])){																		//if the loop does not break, proceed to check for alphanumeric characters
					variableChecker = true;																				//returns a value of true after all characters are extinguished
				}
			}
			
			if(tempEqual=="=" ||tempEqual=="+"||tempEqual=="-"||tempEqual=="*"||tempEqual=="/"||tempEqual=="%") equalChecker= true;	//newly added ( modified) //checks if the string tempEqual contains a valid operator
				else equalChecker= false;																				//return a false value if the "equal sign" does not exist
			
			if(variableChecker==true && equalChecker==true){															//if variableChecker and equalChecker are both true, execute this code snippet	
				for(int i=2; i<user_command.size(); i++){																//for loop scans every string inside the vector
				
					//cout <<  "["<< i << "]" << "SIZE:" << user_command.size() << endl;								//for checking purposese
					string tempString = user_command.at(i);																//tempString will contain every string left on the vector, to be scanned per character
					
					for(int j=0; j<tempString.length(); j++){															//for loop that checks every character inside the string (string taken from vector)															
						
						if(isdigit(tempString[0])|| tempString[0]=='+'|| tempString[0]=='-'|| tempString[0]=='*'|| tempString[0]=='/'|| tempString[0]=='%'||tempString[0]=='('){	//if the string consists of an integer or an operator or parenthesis
							if(tempString[0]=='(') openParenthesis++; //newly added
						
							if(isdigit(tempString[j]) || tempString[j]=='+'|| tempString[j]=='-'|| tempString[j]=='*'|| tempString[j]=='/'|| tempString[j]=='%' ||tempString[j]=='.'||tempString[j]=='('||tempString[j]==')'){//if a string starts with integer for checking valid literal value / invalid variable
								if(tempString[j]=='(') openParenthesis++; //newly added
									else if(tempString[j]==')') closeParenthesis++; //newly added				
								//cout << "first digit is integer [" << tempString[j] << "]" << endl;					//for checking purposes
								stringChecker = true;																	//return a true value indicating the string contain a direct literal value
							}else{
								//cout << "Unknown Word [" << tempString << "]" << endl;
								stringChecker = false;																	//return a false value indicating that a variable is invalid
								//cout << "first digit is integer [" << tempString[j] << "]" << endl;
								break;																					//break out of the loop
							}
						}else{
							if(isalpha(tempString[0]) ||tempString[0]=='_'||tempString[0]=='('){	//if statement to check if the variable starts with a letter or underscore or parenthesis
								if(tempString[0]=='(') openParenthesis++;
								if(isalnum(tempString[j]) || tempString[j]=='+'|| tempString[j]=='-'|| tempString[j]=='*'|| tempString[j]=='/'|| tempString[j]=='%'||tempString[j]=='('||tempString[j]==')'){	//if tempString contain these characters
									if(tempString[j]=='(') openParenthesis++; //newly added
									else if(tempString[j]==')') closeParenthesis++; //newly added				
									stringChecker = true;																//return a true value via the stringChecker, loop continues
									//cout << "[" << tempString[j] << "]" << endl;										//for checking purposes
								}else{
									stringChecker = false;																//if the loop encounters an invalid character, return false via stringChecker
									//cout << "(" << tempString[j] << ")" << endl;										//for checking purposes
									break;																				//break out of the loop
								}
							}else{																						//the variable starts with an invalid character
								stringChecker=false;																	//return a false value indicating an error/anomaly
								if(tempString[j]=='=') break;															//if another '=' is encountered, it automatically breaks the loop to signify anomaly/error
								else cout << "Invalid Variable [" << tempString << "]" << endl;								//print the invalid variable 
								return false;																			//break out of the loop
							}
						}
					}
					if(stringChecker==false)break; //newly added														//immediately break out of the loop once an anomaly is encountered
				}
				
				if(openParenthesis!=closeParenthesis)stringChecker=false;
				
			}else{
				
				if(variableChecker==false) {
					cout << "Invalid Variable [" << tempVariable << "]" << endl;
					return false;
				} 
				//else if(equalChecker==false)cout << "Unknown Command! Does not match any valid command of the language" << endl;
			} 
			
			//cout << "tempVariable: " << tempVariable << endl;															//for checking purposes
			//cout << "tempEqual: " << tempEqual << endl;
			
			if(stringChecker==true){} //cout << "Valid Assignment Operation" << endl;										//for loop returns a true value - assignment operation is valid
				else {
					cout << "Unknown Command! Does not match any valid command of the language" << endl;				//for loop returns a false value - assignment operation is invalid
					return false;
				} 
	}

	return true;				
}

vector<string> processInput(string input) {                                                            //function that process that user command
    vector<string> user_command;                                                                      //vector of type string which stores the user command string into substrings
    string placeholder="";                                                                              
    int i = 0; 
    bool flag = true; 
    bool characterError = false;

    while (i < input.length()) {                                                                      //runs until the length of user command string

        if (isalnum(input[i]) || input[i] == '.') {                                                   //checks if the current character is alphanumeric or a dot
            if (isOperator(placeholder[placeholder.size()-1])) {                                        //if yes, it firstly checks if the last character on the placeholder string is an operator   
                if(isdigit(input[i]) || input[i] =='(' || input[i] == ')' ) {                                                                 //if the upper if-statement is evaluated, it checks if the current char is a digit
                    placeholder += input[i];                                                              //if the current char is digit, it concatenates the digit to the placeholder string
                }
                else {
					if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}                                                                                  //if the current char is not a digit, it pushes the placeholder onto the vector user_command
                    user_command.push_back(placeholder); 
                    placeholder = input[i];                                                              //re-set the placeholder string to the current char
                }
            }
            else {                                                                                      //else if the last charater on the placeholder string is not an operator
                placeholder += input[i];                                                                  //we concatenate the current char to the placeholder string
            }
        }
		
		else if (input[i] == '(' || input[i] == ')') {
			if (isValid(placeholder)) {
				user_command.push_back(placeholder); 
				placeholder = input[i];
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
		
        else if (isOperator(input[i])) {                                                               //checks if the current character is an operator
            if (containsNoChar(placeholder)) {                                                           //if yes, we check if the placeholder string contains no character
                placeholder += input[i];                                                                 //if it doesn't contains any character, we concatenate the current input to the placeholder string
            }                                                                                               
            else {                                                                                     //else,
                if (placeholder != "") {                                                                 //we check if the placeholder string is empty
                    if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}
					user_command.push_back(placeholder);                                                 //if not, we push the placeholder to the user_command vector
                    user_command.push_back(string(1, input[i]));                                         //furthermore, we add the current operator to the user_command vector
                    placeholder = "";                                                                    //reset the placeholder string to an empty string
                }
            }
        }

        else if (input[i] == '=') {                                                                    //check if the current character is an equal sign
            if (placeholder != "") {                                                                     //checks if the placeholder string is not empty
                if (characterError == true) {
					cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                   	user_command.clear();
                   	return user_command;
				}
				user_command.push_back(placeholder);                                                     //place whatever in the placeholder to the user_command vector
            }
            user_command.push_back("=");                                                                 //also, we push the equal sign to the vector
            placeholder = "";                                                                            //reset the placeholder string to empty string
        }

        else if (input[i] == ' ') {                                                                    //checks if the current character is a whitespace
            while (input[i] == ' ') {                                                                    //This loop runs until the character is no longer a white space (eliminating extra ws)
                i++;                                                                                     //increment i so we can access the next character in the input string
			}
			if (input[i] == '\0') {
				break;
			}
			
            if (isOperator(input[i])) {                                                                //if the current character is an operator, same logic from line 47-58 applies here
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
				placeholder = input[i];
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
            else if (input[i] == '=') {                                                                    //check if the current character is an equal sign 
				if (placeholder != "") {  
					if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}                                                                   					//checks if the placeholder string is not empty
                    user_command.push_back(placeholder);                                                     //place whatever in the placeholder to the user_command vector
                }
                user_command.push_back("=");                                                                 //also, we push the equal sign to the vector
                placeholder = "";                                                                            //reset the placeholder string to empty string
            }
            else {                                                                                     //if the current character is not an operator
                if (!isOperator(placeholder[placeholder.size()-1]) && placeholder != "") {               //we check if the last char in the placeholder string is not an operator and the placeholder string is not empty
                    if (characterError == true) {
						cout << "SNOL> Unknown word [" << placeholder <<"]" << endl; 
                    	user_command.clear();
                    	return user_command;
					}
					user_command.push_back(placeholder);                                                 //if the condition are met, we push whatever the content of placeholder string to the vector 
                    placeholder = "";    
                    placeholder += input[i];                                                                 //reset the placeholder string to empty                                                       
                }
                else if (isOperator(placeholder[placeholder.size()-1])) {                              //else if the current character is an operator
                    if (!isdigit(input[i])) {                                                           //check if current input is not a digit, then we push placeholder string to the vector
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

    if (placeholder != "") {                                                                            //If the placeholder string is not empty after the loop, we add whatever's left to the vector
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
    cout << "debug:" << endl; 
    
    for (int i = 0; i < user_command.size(); i++) {
    	cout << i <<": " << user_command.at(i) << endl;
	}

    return user_command;
}

bool isOperator (char ch) {                                                                         //boolean function that checks if a character is an operator
    if (ch == '+' || ch == '-' || ch == '/' || ch == '*' || ch == '%') return true;
    else return false;
}

bool isKeyword(string input){
	if(input == "BEG" || input == "PRINT" || input == "MOD") return true;							//if the set variable is a keyword
	else return false;
}

bool containsNoChar (string input) {                                                                //boolean function that checks if a string contains alphabet charaters
    for (int i = 0; i < input.size(); i++) {
        if (isalpha(input[i])) return false; 
    }
    return true; 
}

bool isValid(string input){
	int i = 0;
	int valid = true;															//initialize valid as true	

		if(isalpha(input[i])) valid = true;										//if the input is only alphanumeric	that start with a letter
		else valid = false;														//else invalid
		
		if(valid == true){														//while it is valid
		  	for(i = 1; i < input.size(); i++) {									//scan each element of the string
		  	   	if(!(isalpha(input[i]) || isdigit(input[i]) || input[i] == '_')){	//if there is unneccessary characters then invalid
					valid = false;
					break;
				}
		  	}
		  	   return true; 													//if the variable is valid, it will return true
		  }
	    else return false;															//else not
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

bool isInteger(string input){
   	istringstream iss(input); 
    int f; 
    iss >> noskipws >> f; 
    return iss.eof() && !iss.fail(); 
}

bool isFloat(string input) {
    istringstream iss(input); 
    float f; 
    iss >> noskipws >> f; 
    return iss.eof() && !iss.fail(); 
}

bool isOperand(string input) {
	bool flag = true; 
	if(!isInteger(input) && !isFloat(input)) flag = false;
	return flag; 
}

bool comparison(string input1, string input2){
	if(isInteger(input1) && isInteger(input2)) return true;
	else if(isFloat(input1) && isFloat(input2)) return true;
	else return false;
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
