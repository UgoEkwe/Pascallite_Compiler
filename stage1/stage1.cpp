// Ugonna Oparaochaekwe and Nathan Karr
// CS 4301
// Stage 1

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <ctype.h>
#include <stage1.h>
#include <ctime>

using namespace std;
int countBool = 0;
int countInt = 0;
time_t now = time(NULL);
const int MAX_SYMBOL_TABLE_SIZE = 256;
bool theEnd = false;
bool nameInSymbolTable(string s);
// constructor
Compiler::Compiler(char **argv){
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}
// destructor
Compiler::~Compiler(){
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader(){
	lineNo += 1;
	listingFile << "STAGE0:" << "  Ugonna Oparaochaekwe and Nathan Karr       ";
	listingFile << ctime(&now) << endl;
	listingFile << "LINE NO." << setw(30) << "SOURCE STATEMENT" << endl << endl;
	listingFile << "    " << lineNo << "|";

}
void Compiler::parser(){
	nextChar();
	nextToken();
	if (token != "program"){
		string err = "keyword \"program\" expected";
		processError(err);
	}
	prog();
}

void Compiler::createListingTrailer(){
	if(errorCount == 0){
		listingFile << endl << endl << "COMPILATION TERMINATED      " << errorCount << " ERRORS ENCOUNTERED" << endl;
	}
	else if(errorCount == 1){
		listingFile << endl << endl << "COMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;
	}
	else{
		listingFile << endl << endl << "COMPILATION TERMINATED      " << errorCount << " ERRORS ENCOUNTERED" << endl;
	}

}
// Methods implementing the grammar productions
// stage 0, production 1
void Compiler::prog(){
	if (token != "program"){
		string err = "keyword \"program\" expected";
		processError(err);
	}
	progStmt();
	if (token == "const"){
		consts();
	}
	if(token == "var"){
		vars();
	}
	if(token != "begin"){
		string err = "keyword \"begin\" expected";
		processError(err);
	}
	beginEndStmt();
	if (!sourceFile.eof()){
		string err = "no text may follow \"end\"";
		processError(err);
	}
}
// stage 0, production 2
void Compiler::progStmt(){
	string x;
	if (token != "program"){
		string err = "keyword \"program\" expected";
		processError(err);
	}
	x = nextToken();
	if (!isNonKeyId(token)){
		string err = "program name expected";
		processError(err);
	}
	nextToken();
	if (token != ";"){
		string err = "semicolon expected";
		processError(err);
	}
	nextToken();
	code("program", x);
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}
// stage 0, production 3
void Compiler::consts(){
	if (token != "const"){
		string err = "keyword \"const\" expected";
		processError(err);
	}
	nextToken();
	if (!isNonKeyId(token)){
		string err = "non-keyword identifier must follow \"const\"";
		processError(err);
	}
	constStmts();
}
// stage 0, production 4
void Compiler::vars(){
	if (token != "var"){
		string err = "keyword \"var\" expected";
		processError(err);
	}
	nextToken();
	if(token[token.length() - 1] == '_'){
		string err = "non-keyword identifier must follow \"var\"";
		processError(err);
	}
	if (!isNonKeyId(token)){
		string err = "non-keyword identifier must follow \"var\"";
		processError(err);
	}
	varStmts();
}
// stage 0, production 5
void Compiler::beginEndStmt(){
	if (token != "begin"){
		string err = "keyword \"begin\" expected";
		processError(err);
	}
	execStmts();
	if (nextToken() != "end"){
		string err = "keyword \"end\" expected";
		processError(err);
	}
	if (nextToken() != "."){
		string err = "period expected";
		processError(err);
	}
	else{
		theEnd = true;
	}
	nextToken();
	code("end", ".");
}
// stage 0, production 6
void Compiler::constStmts(){
	string x, y;
	if(!isNonKeyId(token)){
		string err = "non-keyword indentifier expected";
		processError(err);
	}
	x = token;
	nextToken();
	if (token != "="){
		string err = "\"=\" expected";
		processError(err);
	}
	nextToken();
	y = token;
	// Or statement just refused to work so we did a bunch of nested if's
	if(!isBoolean(y)){
		if(!isNonKeyId(y)){
			if(!isInteger(y)){
				if(y != "+"){
					if(y != "-"){
						if(y != "not"){
							string err = "token to the right of \"=\" illegal";
							processError(err);
						}
					}
				}
			}
		}
	}
	if(y == "+"){
		nextToken();
		if (!isInteger(token)){
			string err = "integer expected after sign";
			processError(err);
		}
		else{
			y = y + token;
		}
	}if(y == "-"){
		nextToken();
		if (!isInteger(token)){
			string err = "integer expected after sign";
			processError(err);
		}
		else{
			y = y + token;
		}
	}
	if (y == "not"){
		nextToken();
		if (!isBoolean(token)){
			string err = "boolean expected after \"not\"";
			processError(err);
		}
		if (token == "true"){
			y = "false";
		}
		else{
			y = "true";
		}
	}
	nextToken();
	if (token != ";"){
		string err = "semicolon expected";
		processError(err);
	}
	if (whichType(y) != BOOLEAN){
		if(whichType(y) != INTEGER){
			string err = "data type of token on the right-hand side must be INTEGER or BOOLEAN";
			processError(err);
		}
	}
	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);
	x = nextToken();
	for(uint i = 0; i < x.length(); i++){
			if(x[i] == '_'){
				if(!isdigit(x[i + 1])){
					if(!isalpha(x[i + 1])){
						string err = "non-keyword identifier, \"begin\", or \"var\" expected";
						processError(err);
					}
				}

			}
		}
	if (!isNonKeyId(x)){
		if(x != "begin"){
			if(x != "var"){
				string err = "non-keyword identifier, \"begin\", or \"var\" expected";
				processError(err);
			}
		}
	}
	if (isNonKeyId(x)){
		constStmts();
	}
}
// stage 0, production 7
void Compiler::varStmts(){
	string x,y;
	if(!isNonKeyId(token)){
		string err = "non-keyword identifier expected";
		processError(err);
	}
	x = ids();
	if (token != ":"){
		string err = "\":\" expected";
		processError(err);
	}
	nextToken();
	if(token != "integer" && token != "boolean"){
		string err = "illegal type follows \":\"";
		processError(err);
	}
	y = token;
	if (nextToken() != ";"){
		string err = "semicolon expected";
		processError(err);
	}
	insert(x, whichType(y), VARIABLE, "", YES, 1);
	nextToken();
	if (token != "begin"){
		if(!isNonKeyId(token)){
			string err = "non-keyword identifier or \"begin\" expected";
			processError(err);
		}
	}
	if (isNonKeyId(token)){
		varStmts();
	}
}
// stage 0, production 8
string Compiler::ids(){
	string temp, tempString;
	if (!isNonKeyId(token)){
		string err = "non-keyword identifer expected";
		processError(err);
	}
	tempString = token;
	temp = token;
	nextToken();
	if (token == ","){
		if (!isNonKeyId(nextToken())){
			string err = "non-keyword identifier expected";
			processError(err);
		}
		tempString = temp + "," + ids();
	}
	return tempString;
}
void Compiler::execStmts()      // stage 1, production 2
{
	while(isNonKeyId(nextToken()) || token == "read" || token == "write"){
		execStmt();
	}
	if(token != "end"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
}
void Compiler::execStmts()      // stage 1, production 2
{
	nextToken();
	while(isNonKeyId(token) || token == "read" || token == "write"){
		execStmt();
	}
	if(token != "end"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
}
void Compiler::execStmt()       // stage 1, production 3
{
	if (isNonKeyId(token)){
		assignStmt();
	}
	if (token == "read"){
		readStmt();
	}
	if (token == "write"){
		writeStmt();
	}
}
void Compiler::assignStmt()     // stage 1, production 4
{
	if (!isNonKeyId(token)){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
	nextToken();
	if (token != ":="){
		string err = ":=";
		processError(err);
	}
	express();
	nextToken();
	if (token != ";"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
}
void Compiler::readStmt()       // stage 1, production 5
{
    if (token != "read"){
		string err = "\"read\" expected";
		processError(err);
    }


	// readList
	string x;

    if (nextToken() != "("){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
    }

    nextToken();         // need to move header to first variable to be read
    x = ids();      // store entire read list in string x

    if (nextToken() != ")"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
    }
	nextToken(); // nextToken should be a ";"
    code("read", x, "");
	// end readList


	if (token != ";"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
}

void Compiler::writeStmt()      // stage 1, production 7
{
    if (token != "write"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
    }


	// writeList
	string x;

	if (nextToken() != "("){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
	nextToken();         // move header to first variable to be written
	x = ids();      // store entire write list in x

	if (nextToken() != ")"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
	nextToken(); // nextToken should be a ";"
	code("write", x, "");
	// end writeList


	if (token != ";"){
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
}

void Compiler::express()       // stage 1, production 9
{
    if (isOP(token)){
        term();
		expresses();
    }
	else if (isInteger(token)){
		term();
		expresses();
    }
    else if (isNonKeyId(token)){
		term();
		expresses();
    }
    else{
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
    }
}
void Compiler::expresses()      // stage 1, production 10
{
	string rOp, lOp;
    if (isRelOp(token)){
		pushOperator(token);
		nextToken();
		term();
		rOp = popOperand();
		lOp = popOperand();
		code(popOperator(), rOp, lOp);
		expresses();
    }

	if (token != ")"){
		if (token != ";"){
			string err = "NonKeyID, \"read\", \"write\", or \"end\"";
			processError(err);
		}
	}
}
void Compiler::term()           // stage 1, production 11
{
	if (isOP(token)){
		factor();
		terms();
    }
	else if (isInteger(token)){
		factor();
		terms();
    }
    else if (isNonKeyId(token)){
		factor();
		terms();
    }
    else{
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
    }
}
void Compiler::terms()          // stage 1, production 12
{
	string rOp, lOp;

    if (isAddLevelOp(token)){
		pushOperator(token);
		nextToken();
		factor();
		rOp = popOperand();
		lOp = popOperand();
		code(popOperator(), rOp, lOp);
		terms();
    }

	if (token != ")"){
		if (token != ";"){
			if (!isRelOp(token)){
				string err = "NonKeyID, \"read\", \"write\", or \"end\"";
				processError(err);
			}
		}
	}
}
void Compiler::factor()         // stage 1, production 13
{
	if (isOP(token)){
		part();
		factors();
    }
	else if (isInteger(token)){
		part();
		factors();
    }
    else if (isNonKeyId(token)){
		part();
		factors();
    }
    else{
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
    }
}
void Compiler::factors()        // stage 1, production 14
{
  	string rOp, lOp;

    if (isMultLevelOp(token)){
		pushOperator(token);
		nextToken();
		part();
		rOp = popOperand();
		lOp = popOperand();
		code(popOperator(), rOp, lOp);
		factors();
    }

	if (token != ")"){
		if (token != ";"){
			if (!isRelOp(token)){
				if (!isAddLevelOp(token)){
					string err = "NonKeyID, \"read\", \"write\", or \"end\"";
					processError(err);
				}
			}
		}
	}
}
void Compiler::part()           // stage 1, production 15
{
	if (token == "not"){
		nextToken();
		if (token == "("){
			nextToken();
			express();
			if (token == ")"){
				code("not", popOperand(), "");
			}
		}
		else if (isBoolean(token)){
			if(token == "true"){
				pushOperand("false");
				nextToken();
			}
			else {
				pushOperand("true");
				nextToken();
			}
		}
		else if (isNonKeyId(token)){
			//code("not", symbolTable.find(token).getInternalName(), ""); //////****************************************************
			nextToken();
		}
	}
	else if (token == "+"){
		nextToken();
		if (token == "("){
			nextToken();
			express();
			if (token != ")"){
				string err = "NonKeyID, \"read\", \"write\", or \"end\"";
				processError(err);
			}
		}
		else if (isInteger(token)){
			pushOperand(token);
			nextToken();
		}
		else if (isNonKeyId(token)){
			pushOperand(token);
			nextToken();
		}
		else{
			string err = "NonKeyID, \"read\", \"write\", or \"end\"";
			processError(err);
		}
	}
	else if (token == "-"){
		nextToken();
		if (token == "("){
			nextToken();
			express();
			if (token != ")"){
				string err = "NonKeyID, \"read\", \"write\", or \"end\"";
				processError(err);
			}
			code("neg", popOperand(), "");
		}

		if (isInteger(token)){
			pushOperand("-" + token);
			nextToken();
		}

		if (isNonKeyId(token)){
			code("neg", token, "");
			nextToken();
		}
	}

	else if (token == "("){
      nextToken();              // grab first token inside ( )
      express();
      if (token != ")"){
			string err = "NonKeyID, \"read\", \"write\", or \"end\"";
			processError(err);
      }
      nextToken();
	}

	else if (isInteger(token)){
		pushOperand(token);
		nextToken();
	}
	else if (isBoolean(token)){
		pushOperand(token);
		nextToken();
	}
	else if (isNonKeyId(token)){
		pushOperand(token);
		nextToken();
	}

	else{
		string err = "NonKeyID, \"read\", \"write\", or \"end\"";
		processError(err);
	}
}
// Helper functions for the Pascallite lexicon
// determines if s is a keyword
bool Compiler::isKeyword(string s) const{
	if(s == "program"){
		return true;
	}
	else if(s == "const"){
		return true;
	}
	else if(s == "var"){
		return true;
	}
	else if(s == "integer"){
		return true;
	}
	else if(s == "boolean"){
		return true;
	}
	else if(s == "begin"){
		return true;
	}
	else if(s == "end"){
		return true;
	}
	else if(s == "true"){
		return true;
	}
	else if(s == "false"){
		return true;
	}
	else if(s == "not"){
		return true;
	}
	else if(s == "mod"){
		return true;
	}
	else if(s == "div"){
		return true;
	}
	else if(s == "and"){
		return true;
	}
	else if(s == "or"){
		return true;
	}
	else if(s == "read"){
		return true;
	}
	else if(s == "write"){
		return true;
	}
	else{
		return false;
	}
}
// determines if c is a special symbol
bool Compiler::isSpecialSymbol(char c) const{
	if (c == '='){
		return true;
	}
	if(c == ':'){
		return true;
	}
	if(c == ','){
		return true;
	}
	if(c == ';'){
		return true;
	}
	if(c == '.'){
		return true;
	}
	if(c == '+'){
		return true;
	}
	if(c == '-'){
		return true;
	}
	else{
		return false;
	}

}
 // determines if s is a non_key_id
bool Compiler::isNonKeyId(string s) const{
	if (isKeyword(s)){
		return false;
	}
	for (uint i = 0; i < s.size(); i++){
		if(!isdigit(s[i])){
			if(!isalpha(s[i])){
				if(s[i] != '_'){
					return false;
				}
			}
		}

	}
	return true;
}
// determines if s is an integer
bool Compiler::isInteger(string s) const{
	for (uint i = 0; i < s.size(); i++){
		if(!isdigit(s.at(i))){
			if(s.at(i) == '-'){
				;
			}
			else if(s.at(i) == '+'){
				;
			}
			else{
				return false;
			}
		}
	}
	return true;
}
// determines if s is a boolean
bool Compiler::isBoolean(string s) const{
	if(s == "true"){
		return true;
	}
	else if(s == "false"){
		return true;
	}
	else if(s == "boolean"){
		return true;
	}
	else{
		return false;
	}
}
// determines if s is a literal
bool Compiler::isLiteral(string s) const{
	if(s == "integer"){
		return true;
	}
	if(isInteger(s)){
		return true;
	}
	if(isBoolean(s)){
		return true;
	}
	if(s == "not"){
		if(isBoolean(token)){
			return true;
		}
	}
	if(s[0] == '+'){
		s.erase(0, 1);
		if(isInteger(token)){
			return true;
		}
	}
	if(s[0] == '-'){
		s.erase(0, 1);
		if(isInteger(token)){
			return true;
		}
	}
	return false;
}

// Action routines
void Compiler::insert(string externalName, storeTypes inType, modes inMode,
            string inValue, allocation inAlloc, int inUnits)
{
	string list;
	list = externalName;
	while(list != ""){
		string name = "";
		while(list[0] != ',' && list != ""){
			name = name + list[0];
			list.erase(0,1);
		}
		while(name.length() > 15){
			name.erase(15, 1);
		}
		if(list[0] == ','){
			list.erase(0,1);
		}
		if(symbolTable.find(name) != symbolTable.end()){
			string err = "symbol " + name + " is multiply defined";
			processError(err);
		}
		if(isKeyword(name)){
			string err = "illegal use of keyword";
			processError(err);
		}
		else{
			if (symbolTable.size() != MAX_SYMBOL_TABLE_SIZE){
				if(isupper(name[0])){
				symbolTable.insert({name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
				name = "";
				}
				else{
				symbolTable.insert({name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
				name = "";
				}
			}
			else{
				string err = "symbol table overflow -- max 256 entries";
				processError(err);
			}
		}
	}
}
// tells which data type a name has
storeTypes Compiler::whichType(string name){
	storeTypes store;
	auto search = symbolTable.find(name);
	if (isLiteral(name)){
		if(isBoolean(name)){
			store = BOOLEAN;
		}
		else{
			store = INTEGER;
		}
	}
	else{
		if(search != symbolTable.end()){
			store = search->second.getDataType();
		}
		else{
			string err = "reference to undefined constant";
			processError(err);
		}
	}
	return store;
}
// tells which value a name has
string Compiler::whichValue(string name){
	string value;
	auto search = symbolTable.find(name);
	if(isLiteral(name)){
		value = name;
	}
	else{
		if(search != symbolTable.end()){
			value = search->second.getValue();
		}
		else{
			string err = "reference to undefined constant";
			processError(err);
		}
	}
	return value;
}
void Compiler::code(string op, string operand1, string operand2){
	if(op == "program"){
		emitPrologue(operand1);
	}
	else if (op == "end"){
		emitEpilogue();
	}
	else if (op == "read"){
		emitReadCode(operand1, operand2);
	}
	else if (op == "write"){
		emitWriteCode(operand1, operand2);
	}
	else if (op == "+"){ // must be binary '+'
		emitAdditionCode(operand1, operand2);
	}
	else if (op == "-"){ // must be binary '-'
		emitSubtractionCode(operand1, operand2);
	}
	else if (op == "neg"){ // must be unary '-'
		emitNegationCode;  //////////////////////////////////////////
	}
	else if (op == "not"){
		emitNotCode;       //////////////////////////////////////////
	}
	else if (op == "*"){
		emitMultiplicationCode(operand1, operand2);
	}
	else if (op == "div"){
		emitDivisionCode(operand1, operand2);
	}
	else if (op == "mod"){
		emitModuloCode(operand1, operand2);
	}
	else if (op == "and"){
		emitAndCode(operand1, operand2);
	}
	else if (op == "or"){
		emitOrCode(operand1, operand2);
	}
	else if (op == "="){
		emitEqualityCode(operand1, operand2);
	}
	else if (op == ":="){
		emitAssignCode(operand1, operand2);
	}
	else if (op == "<>"){
		emitInequalityCode(operand1, operand2);
	}
	else if (op == "<"){
		emitLessThanCode(operand1, operand2);
	}
	else if (op == ">"){
		emitGreaterThanCode(operand1, operand2);
	}
	else if (op == "<="){
		emitLessThanOrEqualToCode(operand1, operand2);
	}
	else if (op == ">="){
		emitGreaterThanOrEqualToCode(operand1, operand2);
	}
	else{
		string err = "compiler error since function code should not be called with illegal arguments";
		processError(err);
	}
}
void Compiler:pushOperator(string op)
{
	operatorStk.push(op);
}
string Compiler:popOperator()
{
	string store;
	if(!operatorStk.empty()){
		store = operatorStk.top();
		operatorStk.pop();
		return store;
	}
	else{
		string err = "compiler error; operator stack underflow";
		processError(err);
	}
}
void Compiler:pushOperand(string operand)
{
	if(isLiteral(operand) && symbolTable.find(name) != symbolTable.end()){
		insert(operand, WhichType(operand), CONSTANT, WhichValue(operand), YES, 1);
	}
	operandStk.push(operand);
}
string Compiler:popOperand()
{
	string store;
	if(!operandStk.empty()){
		store = operandStk.top();
		operandStk.pop();
		return store;
	}
	else{
		string err = "compiler error; operand stack underflow";
		processError(err);
	}
}

// Emit Functions
void Compiler::emit(string label, string instruction, string operands, string comment){
		objectFile << left;
		objectFile << setw(8) << label;
		objectFile << setw(8) << instruction;
		objectFile << setw(24) << operands;
		objectFile << comment << endl;
	}
void Compiler::emitPrologue(string progName, string){
	objectFile << "; Ugonna Oparaochaekwe and Nathan Karr      " << ctime(&now) << endl;
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl << endl;
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	emit("_start:");
}
void Compiler::emitEpilogue(string, string){
	emit("","Exit", "{0}");
	emitStorage();
}
void Compiler::emitStorage(){
	auto printConst = symbolTable.begin();
	auto printVar = symbolTable.begin();
	emit();
	emit("SECTION", ".data");
	while(printConst != symbolTable.end()){
		if(printConst->second.getMode() == CONSTANT && printConst->second.getAlloc() == YES){
			if(printConst->second.getValue() == "false"){
				emit(printConst->second.getInternalName(),"dd", "0","; "+printConst->first);
			}
			else if(printConst->second.getValue() == "true"){
				emit(printConst->second.getInternalName(),"dd", "-1","; "+printConst->first);
			}
			else{
				emit(printConst->second.getInternalName(),"dd", printConst->second.getValue(),"; "+printConst->first);
			}
		}
		printConst++;
	}
	emit();
	emit("SECTION", ".bss");
	while(printVar != symbolTable.end()){
		if(printVar->second.getMode() == VARIABLE && printVar->second.getAlloc() == YES){
			emit(printVar->second.getInternalName(),"resd","1","; "+printVar->first);
		}
		printVar++;
	}
}
void Compiler::emitReadCode(string operand, string = "")
{}
void Compiler::emitWriteCode(string operand, string = "")
{}
void Compiler::emitAssignCode(string operand1, string operand2)         // op2 = op1
{}
void Compiler::emitAdditionCode(string operand1, string operand2)       // op2 +  op1
{
	if (!nameInSymbolTable(operand1) && !Integer(operand1))
   {
      string err = "reference to undefined symbol";
	  processError(err);
   }
   if (!nameInSymbolTable(operand2) && !Integer(operand2))
   {
      string err = "reference to undefined symbol";
	  processError(err);
   }
	if(whichType(operand1) != INTEGER){
		string err = "illegal type";
		processError(err);
	}
	if(whichType(operand2) != INTEGER){
		string err = "illegal type";
		processError(err);
	}
	if(isTemporary(contentsOfAReg) && contentsOfAReg != operand1 && contentsOfAReg != operand2){
		objectFile << setw(6) << " " << "STA" << contentsOfAReg << endl;
		symbolTableEntry(contentsOfAReg, INTEGER, mode, value, YES, 1)/////////////////////
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2){
		contentsOfAReg = operand2;
		objectFile << setw(6) << " " << "LDA " << symbolTable.find(contentsOfAReg).getInternalName << endl; //////////////////
	}
	if (registerA == operand1){

	}
	else if(){

	}
	else{

	}

}
void Compiler::emitSubtractionCode(string operand1, string operand2)   // op2 -  op1
{}
void Compiler::emitMultiplicationCode(string operand1, string operand2) // op2 *  op1
{}
void Compiler::emitDivisionCode(string operand1, string operand2)      // op2 /  op1
{}
void Compiler::emitModuloCode(string operand1, string operand2)         // op2 %  op1
{}
void Compiler::emitNegationCode(string operand1, string = "")           // -op1
{}
void Compiler::emitNotCode(string operand1, string = "")                // !op1
{}
void Compiler::emitAndCode(string operand1, string operand2)            // op2 && op1
{
	if(whichType(operand1) != BOOLEAN){
		string err = "illegal type";
		processError(err);
	}
	if(whichType(operand2) != BOOLEAN){
		string err = "illegal type";
		processError(err);
	}
}
void Compiler::emitOrCode(string operand1, string operand2)             // op2 || op1
{}
void Compiler::emitEqualityCode(string operand1, string operand2)       // op2 == op1
{
	if(whichType(operand1) != whichType(operand2)){
		string err = "incompatible types";
		processError(err);
	}
}
void Compiler::emitInequalityCode(string operand1, string operand2)     // op2 != op1
{}
void Compiler::emitLessThanCode(string operand1, string operand2)       // op2 <  op1
{}
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) // op2 <= op1
{}
void Compiler::emitGreaterThanCode(string operand1, string operand2)    // op2 >  op1
{}
void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) // op2 >= op1
{}

// Lexical routines
// returns the next character or END_OF_FILE marker
char Compiler::nextChar(){
	char next = sourceFile.peek();
	char temp;
	if (sourceFile.eof()){
		ch = END_OF_FILE;
		return ch;
	}
	temp = sourceFile.get();
	ch = temp;
	listingFile << ch;
	if(ch == '\n'){
		if(next != END_OF_FILE){
				lineNo += 1;
				listingFile << setw(5) << right << lineNo << "|";
		}
	}
	return ch;

}
// returns the next token or END_OF_FILE marker
string Compiler::nextToken(){
	token = "";
	while (token == ""){
		if(ch == '{'){
				nextChar();
				while(ch != '}'){
					if(ch == END_OF_FILE){
						string err = "unexpected end of file";
						processError(err);
					}
					else{
						nextChar();
					}
				}
				if (ch == END_OF_FILE){
						string err = "unexpected end of file";
						processError(err);
				}
				else{
						nextChar();
				}
		}
		else if(ch == '}'){
			string err = "\"}\" cannot begin token";
			processError(err);
		}

		else if(isspace(ch)){
			nextChar();
		}
		else if(theEnd && ch != END_OF_FILE){
			string err = "non-white-space character after end statement.";
			processError(err);
		}
		else if(ch == ':'){
			token += ch;
			nextChar();
			if (ch == '='){
				token += ch;
				nextChar();
			}
		}
		else if(ch == '*' || ch == '(' || ch == ')'){
			token += ch;
			nextChar();
		}
		else if(ch == '<'){
			token += ch;
			nextChar();
			if (ch == '>' || ch == '='){
				token += ch;
				nextChar();
			}
		}
		else if(ch == '>'){
			token += ch;
			nextChar();
			if(ch == '='){
				token += ch;
				nextChar();
			}
		}
		else if(isSpecialSymbol(ch)){
			token += ch;
			nextChar();
		}
		else if(islower(ch)){
			token += ch;
			nextChar();
			while(isdigit(ch) || isalpha(ch) || ch == '_'){
				if(ch != END_OF_FILE){
					token += ch;
				}
				nextChar();
			}
			if(ch == END_OF_FILE)
			{
				string err = "unexpected end of file";
				processError(err);
			}
		}
		else if(isdigit(ch)){
			token += ch;
			nextChar();
			while(isdigit(ch) && ch != END_OF_FILE){
				token += ch;
				nextChar();
			}
			if (ch == END_OF_FILE){
				string err = "unexpected end of file";
				processError(err);
			}
		}
		else if(ch == END_OF_FILE){
			token += ch;
		}
		else{
			string err = "illegal symbol";
			processError(err);
		}
	}
	return token;
}

// Other routines
string Compiler::genInternalName(storeTypes stype) const{
	string internName = "";

	if (stype == BOOLEAN){
		string boolString = to_string(countBool);
		internName = "B";
		internName += boolString;
		countBool++;
	}
	if (stype == INTEGER){
		string intString = to_string(countInt);
		internName = "I";
		internName += intString;
		countInt++;
	}
	if (stype == PROG_NAME){
		internName = "p0";
	}
	return internName;
}
void Compiler::processError(string err){
	listingFile << endl;
	listingFile << "Error: Line " << lineNo << ": " << err << endl;
	errorCount++;
	createListingTrailer();
	sourceFile.close();
	listingFile.close();
	objectFile.close();
	exit(EXIT_FAILURE);
}
void Compiler::freeTemp()
{
	currentTempNo--;
	if (currentTempNo < -1){
		string err = "compiler error, currentTempNo should be >= -1";
		processError(err);
	}
}
string Compiler::getTemp()
{
	string temp;
	currentTempNo+++;
	temp = "T" + currentTempNo;
	if (currentTempNo > maxTempNo){
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
		maxTempNo++;
	}
	return temp;
}
string Compiler::getLabel()
{}
bool Compiler::isTemporary(string s) const // determines if s represents a temporary
{
	if(s[0] == 'T'){
		return true;
	}
	else{
		return false;
	}
}
bool nameInSymbolTable(string s){
	auto search = symbolTable.find(s);
	if (symbolTable.find(s) != symbolTable.end()){
		for (uint i = 0; i < symbolTable.size(); i++){
			if(symbolTable(name).getInternalName == name || symbolTable(name).get == name){
				return true;
			}
		}
	}
	return false;
}
