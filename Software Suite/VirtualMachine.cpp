#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>

using namespace std;

bool isValid(char a) {
	if (a == ' ' || a == '\t' || a == '\n' || a == '/' || a == '\r' || a == '*') return false;
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
					i++;
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

string generateLabel(string filename, int c) {
	string ans = "";
	ans += filename;
	string num = to_string(c);
	ans += "_" + num;
	return ans;
}


string arithmetic(string instruction, string label) {
	string ans = "";
	//if (!isalnum(instruction[instruction.length() - 1])) instruction = instruction.substr(instruction.length() - 1);
	if (instruction == "add") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "A = A - 1\n";
		ans += "M = D + M\n";
	}
	else if (instruction == "sub") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "A = A - 1\n";
		ans += "M = M - D\n";
	}
	else if (instruction == "and") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "A = A - 1\n";
		ans += "M = M & D\n";
	}
	else if (instruction == "or") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "A = A - 1\n";
		ans += "M = M | D\n";
	}
	else if (instruction == "eq") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "A = A - 1\n";
		ans += "D = M - D\n";
		ans += "M = 0\n";
		ans += "@" + label + "\n";
		ans += "D; JNE\n";
		ans += "@SP\n";
		ans += "A = M - 1\n";
		ans += "M = -1\n";
		ans += "(" + label + ")" + "\n";
	}
	else if (instruction == "lt") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "A = A - 1\n";
		ans += "D = M - D\n";
		ans += "M = 0\n";
		ans += "@" + label + "\n";
		ans += "D; JGE\n";
		ans += "@SP\n";
		ans += "A = M - 1\n";
		ans += "M = -1\n";
		ans += "(" + label + ")" + "\n";
	}
	else if (instruction == "gt") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "A = A - 1\n";
		ans += "D = M - D\n";
		ans += "M = 0\n";
		ans += "@" + label + "\n";
		ans += "D; JLE\n";
		ans += "@SP\n";
		ans += "A = M - 1\n";
		ans += "M = -1\n";
		ans += "(" + label + ")" + "\n";
	}
	else if (instruction == "not") {
		ans += "@SP\n";
		ans += "A = M - 1\n";
		ans += "M = !M\n";
	}
	else if (instruction == "neg") {
		ans += "@SP\n";
		ans += "A = M - 1\n";
		ans += "M = -M\n";
	}
	//cout << ans;
	return ans;
}

string functionStatements(string instruction, string filename, int counter) {
	string word[3];
	word[0] = "";
	word[1] = "";
	word[2] = "";
	int i = 0;
	for (int j = 0; j < 3; j++) {
		while (i < instruction.size() && instruction[i] != ' ') {
			word[j] += instruction[i++];
		}
		i++;
		if (i == instruction.size()) break;
	}
	string ans = "";
	if (word[0] == "function") {
		int a = getNum(word[2]);
		ans += "(" + word[1] + ")\n";
		ans += "D = 0\n";
		ans += "(pushingLocalsin" + word[1] + ")\n";
		ans += "@SP\n";
		ans += "M = M + 1\n";
		ans += "A = M - 1\n";
		ans += "M = 0\n";
		ans += "@" + word[2] + "\n";
		ans += "D = A - D\n";
		ans += "D = D + 1\n";
		ans += "@pushingLocalsin" + word[1] + "\n";
		ans += "D;JLE\n";
	}
	else if (word[0] == "call") {
		string RA = filename + "_returning_" + to_string(counter);
		string pushD = "@SP\nAM = M + 1\nA = A - 1\nM = D\n";
		ans += "@" + RA + "\n";
		ans += "D = A\n";
		ans += pushD;
		ans += "@LCL\n";
		ans += "D = M";
		ans += pushD;
		ans += "@ARG\n";
		ans += "D = M";
		ans += pushD;
		ans += "@THIS\n";
		ans += "D = M";
		ans += pushD;
		ans += "@THAT\n";
		ans += "D = M";
		ans += pushD;
		ans += "@SP\n";
		ans += "D = M\n";
		ans += "@LCL\n";
		ans += "M = D\n";
		ans += "@" + word[2] + "\n";
		ans += "D = A\n";
		ans += "@5\n";
		ans += "D = D + A\n";
		ans += "@SP\n";
		ans += "D = M - D\n";
		ans += "@ARG\n";
		ans += "M = D\n";
		ans += "@" + word[1] + "\n";
		ans += "0;JMP\n";
		ans += "(" + RA + ")\n";
	}
	else if (word[0] == "return") {
		ans += "@LCL\n";
		ans += "D = M\n";
		ans += "@R13\n";
		ans += "M = D\n";
		ans += "@5\n";
		ans += "D = A\n";
		ans += "@R13\n";
		ans += "A = M - D\n";
		ans += "D = M\n";
		ans += "@R14\n";
		ans += "M = D\n";
		ans += "@ARG\n";
		ans += "D = M\n";
		ans += "@R15\n";
		ans += "M = D\n";
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "@R15\n";
		ans += "A = M\n";
		ans += "M = D\n";
		ans += "@ARG\n";
		ans += "D = M\n";
		ans += "@SP\n";
		ans += "M = D\n";
		string ans1 = "";
		ans1 += "@R14\n";
		ans1 += "M = M -1\n";
		ans1 += "A = M\n";
		ans1 += "D = M\n";
		ans += ans1;
		ans += "@THAT\n";
		ans += "M = D\n";
		ans += ans1;
		ans += "@THIS\n";
		ans += "M = D\n";
		ans += ans1;
		ans += "@ARG\n";
		ans += "M = D\n";
		ans += ans1;
		ans += "@LCL\n";
		ans += "M = D\n";
		ans += "@R13\n";
		ans += "A = M\n";
		ans += "0;JMP\n";
	}
	return ans;
}

string MemoryAccess(string file, string instruction) {
	string word[3];
	word[0] = "";
	word[1] = "";
	word[2] = "";
	int i = 0;
	for (int j = 0; j < 3; j++) {
		while (i < instruction.size() && instruction[i] != ' ') {
			word[j] += instruction[i++];
		}
		i++;
		if (i == instruction.size()) break;
	}
	//cout << word[0] << word[1] << word[2] << endl;
	string ans = "";
	string part;
	if (word[1] == "local") {
		part = "LCL";
	}
	else if (word[1] == "argument") {
		part = "ARG";
	}
	else if (word[1] == "this") {
		part = "THIS";
	}
	else if (word[1] == "that") {
		part = "THAT";
	}
	else {
		part = "NA";
	}
	if (word[0] == "push") {
		if (part != "NA") {
			ans += "@" + word[2] + "\n";
			ans += "D = A\n";
			ans += "@" + part + "\n";
			ans += "A = M + D\n";
			ans += "D = M\n";
			ans += "@SP\n";
			ans += "AM = M + 1\n";
			ans += "A = A - 1\n";
			ans += "M = D\n";
		}
		else {
			if (word[1] == "constant") {
				ans += "@" + word[2] + "\n";
				ans += "D = A\n";
				ans += "@SP\n";
				ans += "AM = M + 1\n";
				ans += "A = A - 1\n";
				ans += "M = D\n";
			}
			else if (word[1] == "pointer") {
				if (word[2] == "0") {
					ans += "@THIS\n";
					ans += "D = M\n";
					ans += "@SP\n";
					ans += "AM = M + 1\n";
					ans += "A = A - 1\n";
					ans += "M = D\n";
				}
				else if (word[2] == "1") {
					ans += "@THAT\n";
					ans += "D = M\n";
					ans += "@SP\n";
					ans += "AM = M + 1\n";
					ans += "A = A - 1\n";
					ans += "M = D\n";
				}
				else {
					return "$ERROR - pointer index invalid\n";
				}
			}
			else if (word[1] == "temp") {
				ans += "@R" + to_string(5 + getNum(word[2])) + "\n";
				ans += "D = M\n";
				ans += "@SP\n";
				ans += "AM = M + 1\n";
				ans += "A = A - 1\n";
				ans += "M = D\n";
			}
			else if (word[1] == "static") {
				ans += "@" + file + '.' + word[2];
				ans += "\nD = M\n";
				ans += "@SP\n";
				ans += "AM = M + 1\n";
				ans += "A = A - 1\n";
				ans += "M = D\n";
			}
			else {
				return "$ERROR" + instruction;
			}
		}
	}
	else if (word[0] == "pop") {
		if (part != "NA") {
			ans += "@" + word[2] + "\n";
			ans += "D = A\n";
			ans += "@" + part + "\n";
			ans += "D = M + D\n";
		}
		else if (word[1] == "static") {
			ans += "@" + file + '.' + word[2];
			ans += "\nD = A\n";
		}
		else if (word[1] == "temp") {
			ans += "@R" + to_string(5 + getNum(word[2])) + "\n";
			ans += "D = A\n";
		}
		if (word[1] == "pointer") {
			if (word[2] == "0") {
				ans += "@SP\n";
				ans += "AM = M - 1\n";
				ans += "D = M\n";
				ans += "@THIS\n";
				ans += "M = D\n";
			}
			else if (word[2] == "1") {
				ans += "@SP\n";
				ans += "AM = M - 1\n";
				ans += "D = M\n";
				ans += "@THAT\n";
				ans += "M = D\n";
			}
		}
		else {
			ans += "@R13\n";
			ans += "M = D\n";
			ans += "@SP\n";
			ans += "AM = M - 1\n";
			ans += "D = M\n";
			ans += "@R13\n";
			ans += "A = M\n";
			ans += "M = D\n";
		}

	}
	//cout << ans;
	return ans;

}

string controlFlow(string instruction, string curr) {
	string word[3];
	word[0] = "";
	word[1] = "";
	word[2] = "";
	int i = 0;
	for (int j = 0; j < 3; j++) {
		while (i < instruction.size() && instruction[i] != ' ') {
			word[j] += instruction[i++];
		}
		i++;
		if (i == instruction.size()) break;
	}
	string ans = "";
	string lab = curr + "$" + word[1];
	if (word[0] == "label") {
		ans += "(" + lab + ")\n";
	}
	else if (word[0] == "goto") {
		ans += "@" + lab + "\n";
		ans += "0;JMP\n";
	}
	else if (word[0] == "if-goto") {
		ans += "@SP\n";
		ans += "AM = M - 1\n";
		ans += "D = M\n";
		ans += "@" + lab + "\n";
		ans += "D;JNE\n";
	}
	return ans;
}

string getIntermediateName(string in) {
	int len = in.size();
	string answer = "";
	string extension = "";
	for (int i = 0; i < len; i++) {
		char c = in[i];
		if (i < len - 3) answer += c;
		else extension += c;
	}
	if (extension == ".vm") return answer + ".ir";
	else return "";
}

void createAssembly(string in, string final) {
	ifstream inFile(in);
	ofstream outFile(final);
	int ifcounter = 0;
	int functioncounter = 0;
	string curr = "";
	while (inFile) {
		string line;
		line.clear();
		getline(inFile, line);
		string ans1 = arithmetic(line, generateLabel(in, ifcounter));
		string ans2 = MemoryAccess(in, line);
		string ans3 = controlFlow(line, curr);
		string ans4 = functionStatements(line, in, functioncounter);
		string toWrite = "";
		if (ans1 != "") {
			if (line == "eq" || line == "lt" || line == "gt") ifcounter++;
			toWrite = ans1;
		}
		else if(ans2 != "") {
			if (ans2[0] != '$') {
				toWrite = ans2;
			}
			else {
				cout << ans2 << endl;
			}
		}
		else if (ans4 != "") {
			string word[3];
			word[0] = "";
			word[1] = "";
			word[2] = "";
			int i = 0;
			for (int j = 0; j < 3; j++) {
				while (i < line.size() && line[i] != ' ') {
					word[j] += line[i++];
				}
				i++;
				if (i == line.size()) break;
			}
			if (word[0] == "call" || word[0] == "function") curr = word[1];
			if (word[0] == "call") functioncounter++;
		}
		else {
			continue;
		}
		cout << toWrite;
		outFile << toWrite;
	}

	inFile.close();
	outFile.close();
}

string getOutputName(string in) {
	int len = in.size();
	string answer = "";
	string extension = "";
	for (int i = 0; i < len; i++) {
		char c = in[i];
		if (i < len - 3) answer += c;
		else extension += c;
	}
	if (extension == ".vm") return answer + ".asm";
	else return "";
}

int main() {
	string in; string out;
	cin >> in;
	out = getIntermediateName(in);
	ifstream inFile(in);

	if (!inFile) {
		cerr << "Unable to open file";
		exit(1);   // call system to stop
	}
	ofstream intermediate = ofstream(out);
	inFile.close();
	intermediate.close();
	generateIntermediate(in, out);
	string final;
	final = getOutputName(in);
	createAssembly(out, final);
}
