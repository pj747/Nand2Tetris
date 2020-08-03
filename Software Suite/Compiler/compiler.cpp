#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <stdlib.h>

using namespace std;

bool isOperator(char a) {
	if (a == '+' || a == '-' || a == '/' || a == '*') return true;
	if (a == '=' || a == '<' || a == '>') return true;
	if (a == '|' || a == '&' || a == '~') return true;
	return false;
}

bool isNum(string a) {
	for (int i = 0; i < a.size(); i++) {
		if (!isdigit(a[i])) return false;
	}
	return true;
}

bool isBrace(char a) {
	if (a == '(' || a == ')' || a == '{' || a == '}') return true;
	if (a == '[' || a == ']') return true;
	return false;
}

bool isSymbol(char a) {
	if (a == '.' || a == ',' || a == ';') return true;
	if (isBrace(a)) return true;
	if (isOperator(a)) return true;
	return false;
}

bool isKeyword(string token) {
	string accepted[] = { "class", "constructor", "function", "method", "field", "static", "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do", "if", "else", "while", "return" };
	for (int i = 0; i < 21; i++) {
		if (token == accepted[i]) return true;
	}
	return false;
}

bool isString(string token) {
	if (token[0] == '\"' && token[token.size() - 1] == '\"' && token.size() > 2) return true;
	return false;
}

bool isIdentifier(string token) {
	if (isdigit(token[0])) return false;
	for (int i = 0; i < token.size(); i++) {
		if (!isdigit(token[i]) && !(token[i] - 'a' < 26 && token[i] - 'a' >= 0) && !(token[i] - 'A' < 26 && token[i] - 'A' >= 0) && (token[i] != '_')) return false;
	}
	return true;
}

class Entry {
public:
	string kind;
	string type;
	int index;
	Entry() {
		kind = "";
		type = "";
		index = 0;
	}
	Entry(string k, string t, int i) {
		kind = k;
		type = t;
		index = i;
	}
};

class ClassSymbolTable {
public:
	int total_count;
	int static_count;
	int field_count;
	map <string, Entry> table;
	ClassSymbolTable() {
		total_count = 0;
		static_count = 0;
		field_count = 0;
	}
	void AddEntry(string name, string kind, string type) {
		total_count++;
		if (kind == "field") {
			Entry temp("this", type, field_count++);
			table[name] = temp;
		}
		else {
			Entry temp(kind, type, static_count++);
			table[name] = temp;
		}
	}
	void Display() {
		for (auto elem : table) {
			cout << elem.first << " " << elem.second.type << " " << elem.second.kind << " " << elem.second.index << endl;
		}
		cout << endl;
	}
	

};

class SubroutineSymbolTable {
public:
	int total_count;
	int argument_count;
	int local_count;
	map <string, Entry> table;
	void Initialise() {
		total_count = 0;
		argument_count = 0;
		local_count = 0;
		table.clear();
	}
	void AddEntry(string name, string kind, string type) {
		total_count++;
		if (kind == "argument") {
			Entry temp(kind, type, argument_count++);
			table[name] = temp;
		}
		else {
			Entry temp(kind, type, local_count++);
			table[name] = temp;
		}
	}
	void Display() {
		for (auto elem : table) {
			cout << elem.first << " " << elem.second.type << " " << elem.second.kind << " " << elem.second.index << endl;
		}
		cout << endl;
	}
};

class Token {
public:
	string type;
	string name;
	void setToken(string line) {
		int i = 1;
		type = "";
		name = "";
		while (i < line.size() && line[i] != '>') {
			type += line[i];
			i++;
		}
		i += 2;
		while (i + 1 < line.size() && line[i + 1] != '<') {
			name += line[i];
			i++;
		}
	}
	string getTag() {
		return "<" + type + "> " + name + " </" + type + ">\n";
	}
};

class compileEngine {
	ofstream outFileX;
	ofstream outFileV;
	ofstream log;
	Token a;
	vector <Token> incomingTokens;
	int count;
	bool valid;
	int labelNum;
	string currentClassName;
	string currentSubroutineName;
	string currentSubroutineType;
	
public:
	ClassSymbolTable classTable;
	SubroutineSymbolTable subTable;
	compileEngine(string in) {
		valid = true;
		int len = in.size();
		string answer = "";
		string extension = "";
		for (int i = 0; i < len; i++) {
			char c = in[i];
			if (i < len - 5) answer += c;
			else extension += c;
		}
		string outX;
		string outV;
		string l;
		if (extension == "T.xml") {
			l = answer + ".err";
			outX = answer + ".xml";
			outV = answer + ".vm";
		}
		else if (extension == ".jack") {
			in = answer + "T.xml";
			l = answer + ".err";
			outX = answer + ".xml";
			outV = answer + ".vm";
		}
		else {
			exit(0);
		}
		ifstream inFile(in);
		string temp;
		getline(inFile, temp);
		while (inFile) {
			string line;
			getline(inFile, line);
			Token temp;
			temp.setToken(line);
			incomingTokens.push_back(temp);
		}
		getline(inFile, temp);
		
		inFile.close();
		log.open(l.c_str());
		outFileX.open(outX.c_str());
		outFileV.open(outV.c_str());

		count = 0;
		labelNum = 0;
		//cout << "h\n";
		
	}

	bool isType(Token b);
	bool isBinaryOperator(Token b);
	bool isUnaryOperator(Token b);
	void indenter(int spaces);
	void syntaxError(string message);
	void writeToken(int spaces);
	void writeOpenTag(int spaces, string tag);
	void writeCloseTag(int spaces, string tag);
	void nextToken();
	Token peekAtNext();
	Entry SymbolTable(string, bool);
	void dumpOp(string op);
	void compileClass(int spaces);
	void compileClassVarDec(int spaces);
	void compileSubroutineDec(int spaces);
	void compileParameterList(int spaces);
	void compileSubroutineBody(int spaces);
	void compileVarDec(int spaces);
	void compileStatements(int spaces);
	void compileLetStatement(int spaces);
	void compileIfStatement(int spaces);
	void compileWhileStatement(int spaces);
	void compileDoStatement(int spaces);
	void compileReturnStatement(int spaces);
	void compileExpression(int spaces);
	void compileSubroutineCall(int spaces, bool term);
	int compileExpressionList(int spaces);
	void compileTerm(int spaces);

};

Entry compileEngine::SymbolTable(string name, bool var) {
	if (subTable.table.count(name) > 0) {
		return subTable.table[name];
	}
	else if (classTable.table.count(name) > 0) {
		return classTable.table[name];
	}
	else if(var){
		log << "Declaration Error: " << name << " undeclared \n";
		log.close();
		exit(0);
	}
	else {
		Entry temp;
		return temp;
	}
}

bool compileEngine::isBinaryOperator(Token b) {
	if (b.name == "+" || b.name == "-" || b.name == "*" || b.name == "/") return  true;
	if (b.name == "&amp;" || b.name == "|" || b.name == "&lt;" || b.name == "&gt;" || b.name == "=") return true;
	return false;
}

bool compileEngine::isUnaryOperator(Token b) {
	if (b.name == "-" || b.name == "~") return true;
	return false;
}

bool compileEngine::isType(Token b) {
	if (b.name == "int" || b.name == "char" || b.name == "boolean") return true;
	if (b.type == "identifier") return true;
	return false;
}

void compileEngine::syntaxError(string message) {
	string type = "";
	if (isSymbol(message[0])) type = "symbol";
	else if (isKeyword(message)) type = "keyword";
	else if (isNum(message)) type = "integerConstant";
	else if (isString(message)) type = "stringConstant";
	else if (isIdentifier(message)) type = "identifier";
	else type = message;

	if (message == "identifier") {
		log << "ERROR: Expecting <identifier> but " << a.name << endl;
		log.close();
		exit(0);
	}
	else {
		if (a.type == type) {
			log << "ERROR: " << a.name << endl;
			log.close();
			exit(0);
		}
		else {
			log << "ERROR: Expecting <" << type <<  "> but " << a.name << endl;
			log.close();
			exit(0);
		}
	}

}

void compileEngine::dumpOp(string op) {
	if (op == "+") outFileV << "add" << endl;
	else if (op == "-") outFileV << "sub" << endl;
	else if (op == "&amp;") outFileV << "and" << endl;
	else if (op == "|") outFileV << "or" << endl;
	else if (op == "&gt;") outFileV << "gt" << endl;
	else if (op == "&lt;") outFileV << "lt" << endl;
	else if (op == "=") outFileV << "eq" << endl;
	else if (op == "*") outFileV << "call Math.multiply 2" << endl;
	else if (op == "/") outFileV << "call Math.divide 2" << endl;
}

void compileEngine::indenter(int spaces) {
	string s = "";
	for (int i = 0; i < spaces; i++) s += " ";
	outFileX << s;
	//cout << s;
}

void compileEngine::writeToken(int spaces) {
	indenter(spaces);
	outFileX << a.getTag();
	//cout << a.getTag();
}

void compileEngine::writeOpenTag(int spaces, string tag) {
	indenter(spaces);
	outFileX << "<" << tag << ">\n";
	//cout << "<" << tag << ">\n";
}

void compileEngine::writeCloseTag(int spaces, string tag) {
	indenter(spaces);
	outFileX << "</" << tag << ">\n";
	//cout << "</" << tag << ">\n";
}

void compileEngine::nextToken() {
	a = incomingTokens[count++];
}

Token compileEngine::peekAtNext() {
	return incomingTokens[count];
}

void compileEngine::compileClass(int spaces) {
	writeOpenTag(spaces, "class");
	nextToken();
	if (a.name == "class" && a.type == "keyword") writeToken(spaces + 2);
	else syntaxError("class");
	nextToken();
	if (a.type == "identifier") {
		writeToken(spaces + 2);
		currentClassName = a.name;
	}
	else syntaxError("identifier");
	nextToken();
	if (a.name == "{") writeToken(spaces + 2);
	else syntaxError("{");
	while (true) {
		Token b = peekAtNext();
		if (b.name == "static" || b.name == "field") compileClassVarDec(spaces + 2);
		else break;
	}
	while (true) {
		Token b = peekAtNext();
		if (b.name == "constructor" || b.name == "function" || b.name == "method" || b.name == "void") compileSubroutineDec(spaces + 2);
		else break;
	}
	nextToken();
	if (a.name == "}") writeToken(spaces + 2);
	else syntaxError("}");
	writeCloseTag(spaces, "class");
	outFileX.close();
	log.close();
}

void compileEngine::compileClassVarDec(int spaces) {
	writeOpenTag(spaces, "classVarDec");
	nextToken();
	string kind = a.name;
	writeToken(spaces + 2);
	nextToken();
	if (isType(a)) writeToken(spaces + 2);
	else syntaxError("type");
	string type = a.name;
	nextToken();
	if (a.type == "identifier") writeToken(spaces + 2);
	else syntaxError("identifier");
	string name = a.name;
	classTable.AddEntry(name, kind, type);
	while (true) {
		Token b = peekAtNext();
		if (b.name == ",") {
			nextToken();
			writeToken(spaces + 2);
			nextToken();
			if (a.type == "identifier") writeToken(spaces + 2);
			else syntaxError("identifier");
			classTable.AddEntry(a.name, kind, type);
		}
		else break;
	}
	nextToken();
	if (a.name == ";") writeToken(spaces + 2);
	else syntaxError(";");
	writeCloseTag(spaces, "classVarDec");
}

void compileEngine::compileSubroutineDec(int spaces) {
	subTable.Initialise();
	writeOpenTag(spaces, "subroutineDec");
	nextToken();
	currentSubroutineType = a.name;
	if (currentSubroutineType == "method") {
		subTable.AddEntry("this", "argument", currentClassName);
	}
	writeToken(spaces + 2);
	nextToken();
	if (a.name == "void" || isType(a)) writeToken(spaces + 2);
	else syntaxError("type");
	nextToken();
	if (a.type == "identifier") writeToken(spaces + 2);
	else syntaxError("identifier");
	currentSubroutineName = a.name;
	nextToken();
	if (a.name == "(") writeToken(spaces + 2);
	else syntaxError("identifier");
	compileParameterList(spaces + 2);
	nextToken();
	if (a.name == ")") writeToken(spaces + 2);
	else syntaxError(")");
	compileSubroutineBody(spaces + 2);
	writeCloseTag(spaces, "subroutineDec");
}

void compileEngine::compileParameterList(int spaces) {
	writeOpenTag(spaces, "parameterList");
	Token b = peekAtNext();
	if (isType(b)) {
		nextToken();
		writeToken(spaces + 2);
		string type = a.name;
		nextToken();
		if (a.type == "identifier") writeToken(spaces + 2);
		else syntaxError("identifier");
		string name = a.name;
		subTable.AddEntry(name, "argument", type);
		while (true) {
			Token b = peekAtNext();
			if (b.name == ",") {
				nextToken();
				writeToken(spaces + 2);
				nextToken();
				if (isType(a)) writeToken(spaces + 2);
				else syntaxError("type");
				type = a.name;
				nextToken();
				if (a.type == "identifier") writeToken(spaces + 2);
				else syntaxError("identifier");
				name = a.name;
				subTable.AddEntry(name, "argument", type);
			}
			else break;
		}
	}
	writeCloseTag(spaces, "parameterList");
}

void compileEngine::compileSubroutineBody(int spaces) {
	writeOpenTag(spaces, "subroutineBody");
	nextToken();
	if (a.name == "{") writeToken(spaces + 2);
	else syntaxError("{");
	Token b = peekAtNext();
	while (b.name == "var") { compileVarDec(spaces + 2); b = peekAtNext(); }
	outFileV << "function " << currentClassName << "." << currentSubroutineName << " " << subTable.local_count << endl;
	if (currentSubroutineType == "constructor") {
		outFileV << "push constant " << classTable.field_count << endl;
		outFileV << "call Memory.alloc 1\n";
		outFileV << "pop pointer 0\n";
	}
	else if (currentSubroutineType == "method") {
		outFileV << "push argument 0\n";
		outFileV << "pop pointer 0\n";
	}
	compileStatements(spaces + 2);
	nextToken();
	if (a.name == "}") writeToken(spaces + 2);
	else syntaxError("}");
	writeCloseTag(spaces, "subroutineBody");
}

void compileEngine::compileVarDec(int spaces) {
	writeOpenTag(spaces, "varDec");
	nextToken();
	if (a.name == "var") writeToken(spaces + 2);
	else syntaxError("var");
	nextToken();
	if (isType(a)) writeToken(spaces + 2);
	else syntaxError("type");
	string type = a.name;
	nextToken();
	if (a.type == "identifier") writeToken(spaces + 2);
	else syntaxError("identifier");
	string name = a.name;
	subTable.AddEntry(name, "local", type);
	nextToken();
	while (a.name == ",") {
		writeToken(spaces + 2);
		nextToken();
		if (a.type == "identifier") writeToken(spaces + 2);
		else syntaxError("identifier");
		subTable.AddEntry(a.name, "local", type);
		nextToken();
	}
	if (a.name == ";") writeToken(spaces + 2);
	else syntaxError(";");
	writeCloseTag(spaces, "varDec");
}

void compileEngine::compileStatements(int spaces) {
	writeOpenTag(spaces, "statements");
	Token b = peekAtNext();
	while (true) {
		if (b.name == "let") compileLetStatement(spaces + 2);
		else if (b.name == "if") compileIfStatement(spaces + 2);
		else if (b.name == "while") compileWhileStatement(spaces + 2);
		else if (b.name == "do") compileDoStatement(spaces + 2);
		else if (b.name == "return") compileReturnStatement(spaces + 2);
		else break;
		b = peekAtNext();
	}
	writeCloseTag(spaces, "statements");
}

void compileEngine::compileLetStatement(int spaces) {
	writeOpenTag(spaces, "letStatement");
	nextToken();
	if (a.name == "let") writeToken(spaces + 2);
	nextToken();
	if (a.type == "identifier") writeToken(spaces + 2);
	else syntaxError("identifier");
	Entry currVar = SymbolTable(a.name, true);
	nextToken();
	bool flag = false;
	if (a.name == "[") {
		flag = true;
		writeToken(spaces + 2);
		compileExpression(spaces + 2);
		nextToken();
		if (a.name == "]") writeToken(spaces + 2);
		else syntaxError("]");
		//VM Code DUMP
		outFileV << "push " << currVar.kind << " " << currVar.index << endl;
		outFileV << "add\n";
		//end DUMP
		nextToken();
	}
	if (a.name == "=") writeToken(spaces + 2);
	else syntaxError("=");
	compileExpression(spaces + 2);
	nextToken();
	if (a.name == ";") writeToken(spaces + 2);
	else syntaxError(";");
	//VM Code DUMP
	if (flag) {
		outFileV << "pop temp 0" << endl;
		outFileV << "pop pointer 1" << endl;
		outFileV << "push temp 0" << endl;
		outFileV << "pop that 0" << endl;
	}
	else {
		outFileV << "pop " << currVar.kind << " " << currVar.index << endl;
	}
	//end DUMP
	writeCloseTag(spaces, "letStatement");
}

void compileEngine::compileIfStatement(int spaces) {
	writeOpenTag(spaces, "ifStatement");
	nextToken();
	if (a.name == "if") writeToken(spaces + 2);
	else syntaxError("if");
	nextToken();
	if (a.name == "(") writeToken(spaces + 2);
	else syntaxError("(");
	compileExpression(spaces + 2);
	nextToken();
	if (a.name == ")") writeToken(spaces + 2);
	else syntaxError(")");
	nextToken();
	int TlabelNum = labelNum;
	labelNum += 2;
	if (a.name == "{") writeToken(spaces + 2);
	else syntaxError("{");
	//VM Code DUMP
	outFileV << "not\n";
	outFileV << "if-goto " << currentClassName << "." << TlabelNum << endl;
	//end DUMP
	compileStatements(spaces + 2);
	nextToken();
	if (a.name == "}") writeToken(spaces + 2);
	else syntaxError("}");
	Token b = peekAtNext();
	//VM Code DUMP
	outFileV << "goto " << currentClassName << "." << TlabelNum + 1 << endl;
	outFileV << "label " << currentClassName << "." << TlabelNum << endl;
	//end DUMP
	if (b.name == "else") {
		nextToken();
		writeToken(spaces + 2);
		nextToken();
		if (a.name == "{") writeToken(spaces + 2);
		else syntaxError("{");
		compileStatements(spaces + 2);
		nextToken();
		if (a.name == "}") writeToken(spaces + 2);
		else syntaxError("}");
	}
	//VM Code DUMP
	outFileV << "label " << currentClassName << "." << TlabelNum + 1 << endl;
	//end DUMP
	writeCloseTag(spaces, "ifStatement");
}

void compileEngine::compileWhileStatement(int spaces) {
	writeOpenTag(spaces, "whileStatement");
	nextToken();
	if (a.name == "while") writeToken(spaces + 2);
	else syntaxError("while");
	nextToken();
	if (a.name == "(") writeToken(spaces + 2);
	else syntaxError("(");
	int TLabelNum = labelNum;
	labelNum += 2;
	//VM Code DUMP
	outFileV << "label " << currentClassName << "." << TLabelNum << endl;
	//end DUMP
	compileExpression(spaces + 2);
	nextToken();
	if (a.name == ")") writeToken(spaces + 2);
	else syntaxError(")");
	nextToken();
	//VM Code DUMP
	outFileV << "not\n";
	outFileV << "if-goto " << currentClassName << "." << TLabelNum+1 << endl;
	//end DUMP
	if (a.name == "{") writeToken(spaces + 2);
	else syntaxError("{");
	compileStatements(spaces + 2);
	nextToken();
	if (a.name == "}") writeToken(spaces + 2);
	else syntaxError("}");
	//VM Code DUMP
	outFileV << "goto " << currentClassName << "." << TLabelNum<< endl;
	outFileV << "label " << currentClassName << "." << TLabelNum + 1<< endl;
	//end DUMP
	writeCloseTag(spaces, "whileStatement");
}

void compileEngine::compileReturnStatement(int spaces) {
	writeOpenTag(spaces, "returnStatement");
	nextToken();
	if (a.name == "return") writeToken(spaces + 2);
	else syntaxError("return");
	Token b = peekAtNext();
	if (b.name != ";") {
		compileExpression(spaces + 2);
		//VM Code DUMP
		outFileV << "return\n";
		//end DUMP
	}
	else {
		//VM Code DUMP
		outFileV << "push constant 0\n";
		outFileV << "return\n";
		//end DUMP
	}
	nextToken();
	if (a.name == ";") writeToken(spaces + 2);
	else syntaxError(";");
	writeCloseTag(spaces, "returnStatement");
}

void compileEngine::compileDoStatement(int spaces) {
	writeOpenTag(spaces, "doStatement");
	nextToken();
	if (a.name == "do") writeToken(spaces + 2);
	else syntaxError("do");
	compileSubroutineCall(spaces + 2, false);
	nextToken();
	if (a.name == ";") writeToken(spaces + 2);
	else syntaxError(";");
	writeCloseTag(spaces, "doStatement");
}

void compileEngine::compileSubroutineCall(int spaces, bool term) {
	nextToken();
	if (a.type == "identifier") writeToken(spaces);
	else syntaxError("identifier");
	string id1 = a.name;
	nextToken();
	if (a.name == ".") {
		writeToken(spaces);
		nextToken();
		if (a.type == "identifier") writeToken(spaces);
		else syntaxError("identifier");
		string id2 = a.name;
		Entry temp = SymbolTable(id1, false);
		bool flag = false;
		if (temp.type != "") {
			flag = true;
			//VM Code DUMP
			outFileV << "push " << temp.kind << " " << temp.index << endl;
			//end DUMP
		}
		nextToken();
		if (a.name == "(") writeToken(spaces);
		else syntaxError("(");
		int nP = compileExpressionList(spaces);
		nextToken();
		if (a.name == ")") writeToken(spaces);
		else syntaxError(")");
		//VM Code DUMP
		if (flag) {
			outFileV << "call " << temp.type << "." << id2 << " " << nP + 1 << endl;
		}
		else {
			outFileV << "call " << id1 << "." << id2 << " " << nP << endl;
		}
		//end DUMP
	}
	else if (a.name == "(") {
		writeToken(spaces);
		//VM Code DUMP
		outFileV << "push pointer 0" << endl;
		//end DUMP
		int nP = compileExpressionList(spaces);
		nextToken();
		if (a.name == ")") writeToken(spaces);
		else syntaxError(")");
		//VM Code DUMP
		outFileV << "call " << currentClassName << "." << id1 << " " << nP + 1 << endl;
		//end DUMP
	}
	else syntaxError("(");
	if (!term) {
		outFileV << "pop temp 0" << endl;
	}
}

int compileEngine::compileExpressionList(int spaces) {
	writeOpenTag(spaces, "expressionList");
	Token b = peekAtNext();
	int count = 0;
	if (b.name != ")") {
		count = 1;
		compileExpression(spaces + 2);
		b = peekAtNext();
		while (b.name == ",") {
			count++;
			nextToken();
			writeToken(spaces + 2);
			compileExpression(spaces + 2);
			b = peekAtNext();
		}
	}
	writeCloseTag(spaces, "expressionList");
	return count;
}

void compileEngine::compileExpression(int spaces) {
	writeOpenTag(spaces, "expression");
	compileTerm(spaces + 2);
	Token b = peekAtNext();
	while (isBinaryOperator(b)) {
		string op = b.name;
		nextToken();
		writeToken(spaces + 2);
		compileTerm(spaces + 2);
		dumpOp(op);
		b = peekAtNext();
	}
	writeCloseTag(spaces, "expression");
}

void compileEngine::compileTerm(int spaces) {
	writeOpenTag(spaces, "term");
	Token b = peekAtNext();
	if (b.type == "integerConstant") {
		nextToken();
		writeToken(spaces + 2);
		//VM Code DUMP
		outFileV << "push constant " << a.name << endl;
		//end DUMP
	}
	else if (b.type == "stringConstant") {
		nextToken();
		writeToken(spaces + 2);
		//VM Code DUMP
		string line = a.name;
		outFileV << "push constant " << line.size() << endl;
		outFileV << "call String.new 1" << endl;
		for (int i = 0; i < line.size(); i++) {
			outFileV << "push constant " << (int) line[i] << endl;
			outFileV << "call String.appendChar 2" << endl;
		}
		//end DUMP
	}
	else if (b.name == "true" || b.name == "false" || b.name == "null" || b.name == "this") {
		nextToken();
		writeToken(spaces + 2);
		//VM Code DUMP
		if (a.name == "true") {
			outFileV << "push constant 0" << endl;
			outFileV << "not" << endl;
		}
		else if (a.name == "this") {
			outFileV << "push pointer 0" << endl;
		}
		else {
			outFileV << "push constant 0" << endl;
		}
		//end DUMP
	}
	else if (isUnaryOperator(b)) {
		nextToken();
		writeToken(spaces + 2);
		string op = a.name;
		compileTerm(spaces + 2);
		//VM Code DUMP
		if (op == "-") outFileV << "neg" << endl;
		else if (op == "~") outFileV << "not" << endl;
		//end DUMP
	}
	else if (b.type == "identifier") {
		Token c = incomingTokens[count + 1];
		if (c.name == "(" || c.name == ".") {
			compileSubroutineCall(spaces + 2, true);
		}
		else if (c.name == "[") {
			nextToken();
			writeToken(spaces + 2);
			string name = a.name;
			Entry currVar = SymbolTable(name, true);
			nextToken();
			writeToken(spaces + 2);
			compileExpression(spaces + 2);
			nextToken();
			if (a.name == "]") {
				writeToken(spaces + 2);
			}
			else {
				syntaxError("]");
			}
			//VM Code DUMP
			outFileV << "push " << currVar.kind << " " << currVar.index << endl;
			outFileV << "add" << endl;
			outFileV << "pop pointer 1" << endl;
			outFileV << "push that 0" << endl;
			//end DUMP
		}
		else {
			nextToken();
			writeToken(spaces + 2);
			string name = a.name;
			Entry currVar = SymbolTable(name, true);
			//VM Code DUMP
			outFileV << "push " << currVar.kind << " " << currVar.index << endl;
			//end DUMP
		}
	}
	else if (b.name == "(") {
		nextToken();
		writeToken(spaces + 2);
		compileExpression(spaces + 2);
		nextToken();
		if (a.name == ")") writeToken(spaces + 2);
		else {
			syntaxError(")");
		}
	}
	else {
		syntaxError("term");
	}
	writeCloseTag(spaces, "term");
}

int main() {
	int t;
	cin>> t;
	while (t--) {
		string file;
		cin >> file;
		compileEngine b(file);
		b.compileClass(0);
	}	
}