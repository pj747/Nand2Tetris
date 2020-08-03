#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <stdlib.h>

using namespace std;

bool isValid(char a) {
	if (a == ' ' || a == '\t' || a == '\n' || a == '\r') return false;
	return true;
}

bool isNum(string a) {
	for (int i = 0; i < a.size(); i++) {
		if (!isdigit(a[i])) return false;
	}
	return true;
}

int getNum(string a) {
	stringstream temp;
	temp << a;
	int x;
	temp >> x;
	return x;
}

bool isOperator(char a) {
	if (a == '+' || a == '-' || a == '/' || a == '*') return true;
	if (a == '=' || a == '<' || a == '>') return true;
	if (a == '|' || a == '&' || a == '~') return true;
	return false;
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

void generateIntermediate(string in, string out) {
	ifstream inFile(in);
	ofstream intermediate(out);
	bool multiFlag = false;
	//generating intermediate file
	while (inFile) {
		string line;
		line.clear();
		getline(inFile, line);
		bool singleFlag = false;
		int i = 0;
		string assembly = "";
		while (i < line.size()) {
			char curr = line[i];
			string temp = "";   //adding to the line
			if (i + 1 < line.size()) {
				char next = line[i + 1];  //in  this case, opening and closing comments is possible
				if (!singleFlag && !multiFlag) {
					if (curr == '/' && next == '/') {
						singleFlag = true;
						i++;    //skip next character
					}
					else if (curr == '/' && next == '*') {
						multiFlag = true;
						i++;
					}
				}
				else if (multiFlag && curr == '*' && next == '/') {
					multiFlag = false;
					i += 2;
					continue;
				}
			}
			if (isValid(curr) && !singleFlag && !multiFlag) {
				temp += curr;
			}
			if (curr == ' ' && !singleFlag && !multiFlag) {
				while (curr == ' ' || curr == '\t' || curr == '\n') {
					i++;
					curr = line[i];
				}
				temp += " ";
				i--;
			}
			assembly += temp;
			i++;
		}
		if (assembly != "") {
			intermediate << assembly << endl;
		}
	}
	inFile.close();
	intermediate.close();
}

string getToken(string token, string log) {
	string ans = "";
	//cout << token << " processing: " << endl;
	if (isSymbol(token[0])) {
		ans += "<symbol> ";
		if (token == "<") ans += "&lt;";
		else if (token == ">") ans += "&gt;";
		else if (token == "&") ans += "&amp;";
		else ans += token;
		ans += " </symbol>\n";
	}
	else if (isKeyword(token)) {
		ans += "<keyword> ";
		ans += token;
		ans += " </keyword>\n";
	}
	else if (isNum(token)) {
		ans += "<integerConstant> ";
		ans += token;
		ans += " </integerConstant>\n";
	}
	else if (isString(token)) {
		ans += "<stringConstant> ";
		for (int i = 1; i < token.size()-1; i++) {
			 ans += token[i];
		}
		ans += " </stringConstant>\n";
	}
	else if (isIdentifier(token)) {
		ans += "<identifier> ";
		ans += token;
		ans += " </identifier>\n";
	}
	else {
		ofstream logging(log, ios_base::app);
		logging << "ERROR: lexical element unidentified: " << token;
		logging.close();
	}
	return ans;
}

void tokeniser(string in, string log, string out) {
	ifstream inFile(in);
	ofstream Tfile(out);
	//generating intermediate file
	Tfile << "<tokens>\n";
	while (inFile) {
		string line;
		line.clear();
		getline(inFile, line);
		int i = 0;
		string token = "";
		while (i < line.size()) {
			char curr = line[i];
			if (isSymbol(curr)) {
				cout << token;
				if (token != "") {
					//cout << token << endl;
					//cout << getToken(token, log);
					if(token != "") Tfile << getToken(token, log);
					token = "";
				}
				token = curr;
				cout << token;
				//cout << getToken(token, log);
				if (token != "") Tfile << getToken(token, log);
				token = "";
			}
			else if (curr == '\"') {
				token = "\"";
				i++;
				while (i < line.size() && (line[i] != '\"')) {
					token += line[i];
					i++;
				}
				if (line[i] == '\"') token += line[i];
				cout << token;
				//cout << getToken(token, log);
				if (token != "") Tfile << getToken(token, log);
				token = "";
			}
			else if (curr == ' ') {
				cout << token;
				//cout << getToken(token, log);
				if (token != "") Tfile << getToken(token, log);
				token = "";
			}
			else {
				token += curr;
				//cout << token << endl;
			}
			i++;
		}
	}
	Tfile << "</tokens>\n";
	inFile.close();
	Tfile.close();
}


string getFileName(string in, string Rextension) {
	int len = in.size();
	string answer = "";
	string extension = "";
	for (int i = 0; i < len; i++) {
		char c = in[i];
		if (i < len - 5) answer += c;
		else extension += c;
	}
	if (extension == ".jack") return answer + Rextension;
	else return "";
}

int main() {
	int n;
	cin >> n;
	while (n--) {
		string in;
		cin >> in;
		string interim = getFileName(in, ".ir");
		ifstream inFile(in);

		if (!inFile) {
			cerr << "Unable to open file";
			exit(1);   // call system to stop
		}
		generateIntermediate(in, interim);
		string final;
		final = getFileName(in, "T.xml");
		string log;
		log = getFileName(in, ".err");
		tokeniser(interim, log, final);
		remove(interim.c_str());
		string big;
		big = getFileName(in, ".xml");
	}
}