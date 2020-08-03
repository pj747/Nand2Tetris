#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <stdlib.h>


using namespace std;

bool isValid(char a){
    if(a == ' '  || a == '\t' || a == '\n' || a == '/' || a == '\r' || a == '*') return false;
    return true;
}

bool isNum(string a){
    for(int i = 0; i < a.size();i++){
        if(!isdigit(a[i])) return false;
    }
    return true;
}

int getNum(string a){
    stringstream temp;
    temp << a;
    int x;
    temp >> x;
    return x;
}

void generateIntermediate(string in, string out){
    ifstream inFile(in);
	ofstream intermediate(out);
    bool multiFlag = false;
    //generating intermediate file
	while(inFile){
        string line;
        line.clear();
        getline(inFile, line);
        bool singleFlag = false;
        int i = 0;
        string assembly = "";
        while(i  < line.size()){
            char curr = line[i];
            string temp = "";   //adding to the line
            if(i+1<line.size()){
                char next = line[i+1];  //in  this case, opening and closing comments is possible
                if(!singleFlag && !multiFlag){
                    if(curr == '/' && next == '/'){
                        singleFlag = true;
                        i++;    //skip next character
                    }
                    else if(curr == '/' && next == '*'){
                        multiFlag = true;
                        i++;
                    }
                }
                else if(multiFlag && curr == '*' && next == '/'){
                        multiFlag = false;
                        i++;
                }
            }
            if(isValid(curr) && !singleFlag && !multiFlag){
                temp+=curr;
            }
            assembly += temp;
            i++;
        }
        if(assembly != ""){
            intermediate << assembly << endl;
        }
	}
	inFile.close();
	intermediate.close();
}

string convertToBinary(int n){
	stack <int> temp;
	string answer = "";
	while(n){
		int c = n % 2;
		temp.push(c);
		n/=2;
	}
	int size = temp.size();
	if (size > 15) return "OVERFLOW ERROR";
	for(int i = 1; i <= 15- size;i++) temp.push(0);
	while(!temp.empty()){
        int t = temp.top();
        char c = t > 0 ? '1' : '0';
		temp.pop();
		answer+=c;
	}
	return answer;
}


string getJumpBits(string line){
    map<string, string> jump;
    jump["JMP"] = "111";
    jump["JLT"]="100";
    jump["JGT"]="001";
    jump["JEQ"] = "010";
    jump["JGE"] = "011";
    jump["JNE"] = "101";
    jump["JLE"] = "110";
    jump[""] = "000";
    if(jump.count(line) == 0) return "ERROR WITH JUMP";
    else return jump[line];
}

string getDestinationBits(string line){
	map <string, string> dest;
	dest["A"] = "100";
	dest["M"] = "001";
	dest["D"] = "010";
	dest["MD"] = "011";
	dest["DM"] = "011";
	dest["AM"] = "101";
	dest["MA"] = "101";
	dest["AD"] = "110";
	dest["DA"] = "110";
	dest["MAD"] = "111";
	dest["MDA"] = "111";
	dest["ADM"] = "111";
	dest["AMD"] = "111";
	dest["DAM"] = "111";
	dest["DMA"] = "111";
	dest[""] = "000";
	if(dest.count(line) == 0) return "ERROR WITH DEST";
	else return dest[line];
}

string getComputeBits(string line){
	map <string, string> ALU;
	ALU["0"] = "0101010";
	ALU["1"] = "0111111";
	ALU["-1"] = "0111010";
	ALU["D"] = "0001100";
	ALU["A"] = "0110000";
	ALU["!D"] = "0001101";
	ALU["!A"] = "0110001";
	ALU["-D"] = "0001111";
	ALU["-A"] = "0110011";
	ALU["D+1"] = "0011111";
	ALU["1+D"] = "0011111";
	ALU["A+1"] = "0110111";
	ALU["1+A"] = "0110111";
	ALU["D-1"] = "0001110";
	ALU["A-1"] = "0110010";
	ALU["D+A"] = "0000010";
	ALU["A+D"] = "0000010";
	ALU["D-A"] = "0010011";
	ALU["A-D"] = "0000111";
	ALU["D&A"] = "0000000";
	ALU["A&D"] = "0000000";
	ALU["D|A"] = "0010101";
	ALU["A|D"] = "0010101";
	ALU["M"] = "1110000";
	ALU["!M"] = "1110001";
	ALU["-M"] = "1110011";
	ALU["M+1"] = "1110111";
	ALU["1+M"] = "1110111";
	ALU["M-1"] = "1110010";
	ALU["D+M"] = "1000010";
	ALU["M+D"] = "1000010";
	ALU["D-M"] = "1010011";
	ALU["M-D"] = "1000111";
	ALU["D&M"] = "1000000";
	ALU["M&D"] = "1000000";
	ALU["D|M"] = "1010101";
	ALU["M|D"] = "1010101";
	if(ALU.count(line)) return  ALU[line];
	else return "COMPUTATION NOT DEFINED";
}

string getIntermediateName(string in){
    int len = in.size();
    string answer = "";
    string extension = "";
    for(int i = 0; i < len; i++){
        char c = in[i];
        if(i < len - 4) answer += c;
        else extension +=c;
    }
    if(extension == ".asm") return answer + ".ir";
    else return "";
}

string getOutputName(string in){
	int len = in.size();
    string answer = "";
    string extension = "";
    for(int i = 0; i < len; i++){
        char c = in[i];
        if(i < len - 4) answer += c;
        else extension +=c;
    }
    if(extension == ".asm") return answer + ".hack";
    else return "";
}

map <string, int> generateSymbolTable(string out){
    vector <string> orderedTable;
    map <string, int> symbolTable;
    ifstream irFile(out);
    int linecount = 0;
    while(irFile){
        string line;
        line.clear();
        getline(irFile, line);
        int i = 0;
        symbolTable["SP"] = 0;
        symbolTable["LCL"] = 1;
        symbolTable["ARG"] = 2;
        symbolTable["THIS"] = 3;
        symbolTable["THAT"] = 4;
        symbolTable["SCREEN"] = 16384;
        symbolTable["KBD"] = 24576;
        for(int j = 0; j < 16; j++){
            stringstream temp;
            temp << "R" << j;
            symbolTable[temp.str()] = j;
        }
        string address;
        if (line[i] == '@'){
            while(++i < line.size()) address += line[i];
            if(isNum(address)){
                symbolTable[address] = getNum(address);
            }
            if(!symbolTable.count(address)) {
                symbolTable[address] = -1;
                orderedTable.push_back(address);
            }
        }
        i = 0;
        if (line[i] == '('){
            while(++i < line.size()&& line[i]!= ')') address += line[i];
            symbolTable[address] = linecount--;
        }
        linecount++;
    }
    int counter = 0;
    for(int j = 0; j < orderedTable.size(); j++){
        if(symbolTable[orderedTable[j]] == -1){
            symbolTable[orderedTable[j]] = 16 + counter++;
        }
    }
    cout << endl;
    irFile.close();
    return symbolTable;
}

void Pass2(string out, string final, map<string, int> symbolTable){
	ifstream irFile = ifstream(out);
    ofstream log = ofstream("log.txt");
    ofstream hack = ofstream(final);
    int linecount = 0;
    while(irFile){
    	linecount++;
        string line;
        string instruction = "";
    	getline(irFile, line);
    	if(line[0] == '(') continue;
    	if(line == "") continue;
    	char type = line[0];
    	if(type == '@'){
            instruction += '0';
    		string address;
    		for(int i = 1; i < line.size();i++) address+=line[i];
    		int a = symbolTable[address];
    		string bin = convertToBinary(a);
    		if(bin != "OVERFLOW ERROR") instruction+=bin;
    		else log << linecount << " " << bin << endl;
    	}
        else{
            instruction = "111";
            int equal = -1; int semicolon = -1;
            string comp = ""; string dest = ""; string jump = "";
            for(int i = 0; i < line.size(); i++) {
            	if (line[i] == '=') equal = i;
            	else if(line[i] == ';') semicolon = i;
            }
            if(equal == -1 && semicolon == -1){
                log << linecount << ": " << "LINE HAS NO STRUCTURE" << endl;
            }
            else if(equal != -1 && semicolon == -1){
                for(int i = 0; i < equal;i++) dest += line[i];
                for(int i = equal + 1; i < line.size(); i++) comp += line[i];
            }
            else if(equal == -1 && semicolon != -1){
                for(int i = 0; i < semicolon; i++) comp += line[i];
                for(int i = semicolon + 1; i < line.size(); i++) jump += line[i];
            }
            else{
            	log << linecount << "WARNING - line jump and destination " << endl;
                for(int i = 0; i < equal;i++) dest += line[i];
                for(int i = equal + 1; i < semicolon; i++) comp += line[i];
                for(int i = semicolon + 1; i < line.size(); i++) jump += line[i];
            }

            string compBits = getComputeBits(comp);
        	string destBits = getDestinationBits(dest);
        	string jumpBits = getJumpBits(jump);
        	if(compBits != "COMPUTATION NOT DEFINED") instruction += compBits;
        	else log << linecount << ": " << compBits << endl;
        	if(destBits != "ERROR WITH DEST") instruction += destBits;
        	else log << linecount << ": " << destBits << endl;
        	if(jumpBits != "ERROR WITH JUMP") instruction += jumpBits;
        	else log << linecount << ": " << jumpBits << endl;
        }

        if(instruction.size() == 16) hack << instruction << endl;
        else {
        	hack.close();
        	remove(final);
        	break;
        }
    }
}

int main(){
	string in; string out;
    cin >> in;
    out = getIntermediateName(in);
	ifstream inFile = ifstream(in);

    if (!inFile) {
    cerr << "Unable to open file";
    exit(1);   // call system to stop
    }
    ofstream intermediate = ofstream(out);
    inFile.close();
    intermediate.close();
    generateIntermediate(in, out);
    map <string, int> symbolTable = generateSymbolTable(out);

    //PASS 2
    string final;
    final = getOutputName(in);
    Pass2(out, final, symbolTable);
}
