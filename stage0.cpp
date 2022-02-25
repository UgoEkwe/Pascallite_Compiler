// Ugonna Oparaochaekwe and Nathan Karr
// CS 4301
// Stage 0

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <ctype.h>
#include <stage0.h>
#include <ctime>

using namespace std;
int countBool = 0;
int countInt = 0;
time_t now = time(NULL);
const int MAX_SYMBOL_TABLE_SIZE = 256; /******/
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
	lineNo++;
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
	listingFile << endl << endl << "COMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;
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
	if (nextToken() != "end"){
		string err = "keyword \"end\" expected";
		processError(err);
	}
	if (nextToken() != "."){
		string err = "period expected";
		processError(err);
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
		token = nextToken();
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
	//x = nextToken();
	if (nextToken() != "begin" && token != "var" && !isNonKeyId(token)){
				string err = "non-keyword identifier, \"begin\", or \"var\" expected";
				processError(err);
	}
	if (isNonKeyId(token)){
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

// Helper functions for the Pascallite lexicon
// determines if s is a keyword
bool Compiler::isKeyword(string s) const{
	if(s == "program"){
		return true;
	}
	if(s == "const"){
		return true;
	}
	if(s == "var"){
		return true;
	}
	if(s == "integer"){
		return true;
	}
	if(s == "boolean"){
		return true;
	}
	if(s == "begin"){
		return true;
	}
	if(s == "end"){
		return true;
	}
	if(s == "true"){
		return true;
	}
	if(s == "false"){
		return true;
	}
	if(s == "not"){
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
				return false;
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
			if (symbolTable.size() != MAX_SYMBOL_TABLE_SIZE) { /******/

				if(isupper(name[0])){
					symbolTable.insert({name, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
					name = "";
				}
				else{
					symbolTable.insert({name, SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
					name = "";
				}
			}/******/
			else {/******/
			  string err = "Symbol Table overflow. Cannot allocate more than 256 entries";
			  processError(err);
			}/******/
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
	else{
		string err = "compiler error since function code should not be called with illegal arguments";
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
	objectFile << "; Ugonna Oparaochaekwe and Nathan Karr       " << ctime(&now) << endl;
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl << endl;
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	emit("_start:");
}
void Compiler::emitEpilogue(string, string){
	emit("","exit", "{0}");
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
				emit(printConst->second.getInternalName(),"dd", "0",";  "+printConst->first);
			}
			else if(printConst->second.getValue() == "true"){
				emit(printConst->second.getInternalName(),"dd", "-1",";  "+printConst->first);
			}
			else{
				emit(printConst->second.getInternalName(),"dd", printConst->second.getValue(),";  "+printConst->first);
			}
		}
		printConst++;
	}
	emit();
	emit("SECTION", ".bss");
	while(printVar != symbolTable.end()){
		if(printVar->second.getMode() == VARIABLE && printVar->second.getAlloc() == YES){
			emit(printVar->second.getInternalName(),"resd","1",";  "+printVar->first);
		}
		printVar++;
	}
}

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
				lineNo++;
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
		else if(isSpecialSymbol(ch)){
			token = ch;
			nextChar();
		}
		else if(islower(ch)){
			token = ch;
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
			token = ch;
			nextChar();
			while(isdigit(ch) && ch == END_OF_FILE){
				token += ch;
			}
			if (ch == END_OF_FILE){
				string err = "unexpected end of file";
				processError(err);
			}
		}
		else if(ch == END_OF_FILE){
			token = ch;
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
		internName = "B";
		internName += (countBool + '0');
		countBool++;
	}
	if (stype == INTEGER){
		internName = "I";
		internName += (countInt + '0');
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
	exit(EXIT_FAILURE); /******/

}
