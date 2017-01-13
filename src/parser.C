#include <parser.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <limits>

#include <TROOT.h>
#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include <sstream>
#include "TF1.h"
#include "TError.h"

using std::cout;
using std::endl;
using std::runtime_error;
namespace journal_parser {
int binarynode::isSeparator(char c){
    for (int i=0; i<operationsCount; i++) {
        if ( c == operations[i] ) {
            return 1;
        }
    }
    if ((c == '(') || (c == ')')) {
        return 1;
    }
    else {
        return 0;
    }
}

// Main parsing code.
binarynode::binarynode(string sin, int preproc) {

	string s = preproc ? preprocess(sin) : sin;

	leftnode = NULL;
	rightnode = NULL;
	svalue = "";
	type = -1;  // not defined yet

	//int iChar = 0;       // navigation along the string

	// check if we start with (
	while (s[0] == '(') {
		// Find closing ")"

		int OpenCount = 1;
		unsigned int i = 1;
		for (; i < s.length(); i++) {
			if (s[i] == '(')
				OpenCount++;
			if (s[i] == ')')
				OpenCount--;
			if (OpenCount == 0)
				break;
		}

		//	cout << s << endl;
		if (OpenCount == 0) {
			if (i == s.length() - 1) {
				// cout << " ) at the end of line "<<endl;
				s = s.substr(1, i - 1);
			} else {
				//  Check what follows:
				if (s[i + 1] == '.') {
					unsigned int j = i + 1;
					// check if it goes to the end of the string
					int OpenCnt = 0;
					for (; j < s.length(); j++) {
						if (s[j] == '[')
							OpenCnt++;
						if (s[j] == ']')
							OpenCnt--;
						if (isSeparator(s[j]) && (OpenCnt == 0))
							break;
					}

					if (j == s.length()) {
						// Store "modifier" node
						rightnode = NULL;
						leftnode = new binarynode(s.substr(1, i - 1), 0);
						svalue = s.substr(i + 1, s.length());
						type = typeUnaryOperation;

						return;
					} else {
						break;
					}
				} else {
					break;
				}
			}
		} else {
			cout << "unbalanced parentheses in string " << s << endl;
			throw 21;
			// Error .. unbalanced parentheses.
		}
	}

	// Now search for operation following operations order, from low to high
	for (int iOrd = 1; iOrd <= highestOrder; iOrd++) {
		for (int iOp = 0; iOp < operationsCount; iOp++) {

			//      cout << "bla" << operations[iOp] <<endl;
			if (operationsOrder[iOp] > iOrd)
				break;
			// Search for the operation:
			int location = 0;
			int OpenCount = 0;
			int OpenCount2 = 0;
			string LastOpen = " ";
			for (unsigned int i = 0; i < s.length(); i++) {
				//	cout << i << " " << s[i] << operations[iOp] << endl;
				// Check for opening parenthesis
				if (s[i] == '(') {
					OpenCount++;
					LastOpen += "(";
				}

				if (s[i] == '[') {
					OpenCount2++;
					LastOpen += "[";
				} else if (s[i] == ')') {
					OpenCount--;
					if (LastOpen[LastOpen.length() - 1] != '(') {
						cout << "Syntax error: wrong sequence of [)" << endl;
						throw 1;
					}
					LastOpen = LastOpen.substr(0, LastOpen.length() - 1);
				} else if (s[i] == ']') {
					OpenCount2--;
					if (LastOpen[LastOpen.length() - 1] != '[') {
						cout << "Syntax error: wrong sequence of (]" << endl;
						throw 2;
					}
					LastOpen = LastOpen.substr(0, LastOpen.length() - 1);
				} else if ((s[i] == operations[iOp]) && (OpenCount == 0) && (OpenCount2 == 0)) {
					location = i;
					break;
				}
			}

			if ((OpenCount != 0) || (OpenCount2 != 0)) {
				cout << "Unbalanced parentheses " << endl;
				throw 3;
			}

			//      s.find(operation[iOp]);
			if (location > 0) {
				type = 1;
				// cout << " got operation" <<  operations[iOp] <<endl;
				operation = operations[iOp];
				//  Got operation with current low order
				leftnode = new binarynode(s.substr(0, location), 0);
				rightnode = new binarynode(s.substr(location + 1, s.length()), 0);
				return;
			}
		}
	}
	// No operations, must be a dvalue or svalue
	if (isdigit(s[0])) {
		type = typeDValue;
		svalue = s;
		// cout << "got dval " << svalue <<endl;
	} else {
		type = typeSValue;
		svalue = s;
		// cout << "got sval " << svalue <<endl;
	}
}

binarynode::~binarynode(){
    delete leftnode;
    delete rightnode;
}

void binarynode::print() {
  
    if (type == typeOperation) {
        cout<<"Operation:"<<operation<<endl;
        if (leftnode  != NULL) {cout<< "Left: ";  leftnode->print();}
        if (rightnode != NULL) {cout<< "Right: "; rightnode->print();}
    }
    else if (type == typeDValue ){
        cout << "DValue:" << svalue <<endl;
    }
    else if (type == typeSValue ){
        cout << "SValue:" << svalue <<endl;
    }

    else if (type == typeUnaryOperation) {
        cout << " Unary operation" << svalue<< " on "; leftnode->print();      
    }
    else {
        cout << "Invalid type=" << type << endl;
    }
}

string binarynode::eval() {
	if (type == typeOperation) {
		string left = leftnode->eval();
		string right = rightnode->eval();

		// cout << "eval left,right:" << left <<","<<right<<endl;

		if ((leftnode->getType() == typeDValue) && (rightnode->getType() == typeDValue)) {

			double dleft = strtod(left.c_str(), NULL);
			double dright = strtod(right.c_str(), NULL);
			double result = 0;
			if (operation == '+') {
				result = dleft + dright;
			}
			if (operation == '-') {
				result = dleft - dright;
			}
			if (operation == '*') {
				result = dleft * dright;
			}
			if (operation == '%') {
				result = dleft / dright;
			}

			if (operation == '#') {
				result = dleft / dright;
			}

			std::ostringstream o;
			o << result;
			return o.str();
		} else {
			string out("(");
			out += left;
			out += operation;
			out += right;
			out += ")";
			return out;
		}
	} else if ((type == typeSValue) || (type == typeDValue)) {
		return svalue;
	} else if (type == typeUnaryOperation) {
		string s = leftnode->eval();
		s += svalue;
		return s;
	} else {
		return "Invalid type";
	}

}

int binarynode::getType(){
    if (type != typeOperation ) {
        return type;
    }
    else {
        int left  = leftnode  -> getType();
        int right = rightnode -> getType();
        if ( (left == typeDValue) && (right == typeDValue)) {
            return typeDValue;
        }
        else {
            return typeSValue;
        }
    }
}

string binarynode::preprocess( string s) {

	// cout << "Preprocess In: " << s << endl;
	// Get rid of white spaces.
	// Special treatment for //fileName/( ) syntaxis.

	string nowhite = "";
	// Remove white spaces
	for (unsigned int i = 0; i < s.length(); i++) {
		if (s[i] != ' '){
			nowhite += s[i];
		}
	}

    // Now search for //XXX/( ) patterns
    string expandFile = "";
    unsigned int iChar = 0;
	while (iChar < nowhite.length()) {
		if (nowhite.substr(iChar, 2) == "//") {
			// locate closing /
			unsigned int i = iChar + 2;
			for (; i < nowhite.length(); ++i) {
				if (nowhite.substr(i, 2) == "/(")
					break;
			}
			if (i < nowhite.length()) {
				// Ok, found the closing '/'
				string fileName = nowhite.substr(iChar + 2, i - iChar - 2);
				// Check if what follows is (
				i++;
				if (i == nowhite.length()) {
					cout << "nothing follows file name " << fileName << endl;
					throw 31;
				}

				if (nowhite[i] == '(') {
					// locate the closing )
					int OpenCount = 0;
					unsigned int j = i;

					for (; j < nowhite.length(); ++j) {
						if (nowhite[j] == '(')
							++OpenCount;
						if (nowhite[j] == ')')
							--OpenCount;
						if (OpenCount == 0)
							break;
					}
					if (OpenCount == 0) {
						// Ok
						expandFile += expandFileName(fileName, nowhite.substr(i + 1, j - i - 1));
						iChar = j + 1;

					} else {
						// Error .. unbalanced parentheses.
						cout << "unbalanced parentheses in string " << nowhite << endl;
						throw 33;
					}
				} else {
					// Continue as usual
					expandFile += nowhite[iChar];
					++iChar;
				}
			} else {
				// Continue as usual
				expandFile += nowhite[iChar];
				++iChar;
				//	      cout <<"un-closed file name for string " << nowhite << endl;
				//throw 34;
			}
		} else {
			expandFile += nowhite[iChar];
			++iChar;
		}
	}
	// cout << "Preprocess Out: " << expandFile << endl;
    return expandFile;
}

string binarynode::expandFileName(string FileName, string s) {
	// Add FileName to all sValues.

	cout << " expandFileName: input: " << FileName << ","<<s<<endl;

	string out = "";
	unsigned int iChar = 0;
	// svalues: 1) start with non-digit  2) limitted by +-%*()

	while (iChar < s.length()) {
		if (isSeparator(s[iChar])) {
			out += s[iChar];
			iChar++;
		}
		// Start of S or D value. Find the end
		unsigned int i = iChar;
		for (; i < s.length(); ++i) {
			if (isSeparator(s[i]))
				break;
		}
		string dsValue = s.substr(iChar, i - iChar);
		iChar = i;

		if (dsValue != "") {
			if (isdigit(dsValue[0])) {
				// DValue, no modification
				out += dsValue;
			} else {
				// SValue, append file
				out += "//";
				out += FileName;
				out += '/';
				out += dsValue;
			}
		}
	}

	cout << " expandFileName: output: " << out <<endl;
	return out;
}

void getOption(string options, string opt, double &var) {
	int i = options.find(opt);
	if(i >= 0) {
		string sc = options.substr(i + opt.length(), options.length());
		var = strtod(sc.c_str(), NULL);
	}
}

void getOption(string options, string opt, int &var) {
	int i = options.find(opt);
	if(i >= 0) {
		string sc = options.substr(i + opt.length(), options.length());
		var = strtod(sc.c_str(), NULL);
	}
}

void getOption(string options, string opt, bool &var) {
	int i = options.find(opt);
	if(i >= 0) {
		var = true;
	}
}

void getOption(string options, string opt, string &var) {
	int i = options.find(opt);
	if(i >= 0) {
		string sc = options.substr(i + opt.length(), options.length());
		if(sc[0] == '\'') {
			sc = sc.substr(1, sc.length());
			i = sc.find("\'");
			if(i < 0) {
				Warning("getOption", "No closing quite in name");
				var="";
			}
			var = sc.substr(0, i);
		}
	}
}

double getVal(string &options, string opt) {
	double val = std::numeric_limits<double>::quiet_NaN();
	string sval;
	if(options.find(opt)!=string::npos){
		sval = options.substr(0, options.find(opt));
		options = options.substr(options.find(opt)+1,options.length());
	} else {
		sval = options;
		options = "";
	}
	if (sval!=""){
		val = strtod(sval.c_str(), NULL);
	}
	return val;
}

void FillVal(string options, string split, string prefix,
		double &var1, double &var2, double &var3, double &var4) {
	string opt = "";
	prefix = prefix+"[";
	if(options.find(prefix)!=string::npos &&
			options.find(']', options.find(prefix))!=string::npos){
		opt = options.substr(options.find(prefix)+prefix.length(),
			options.find(']', options.find(prefix)) - options.find(prefix)-prefix.length());
	}

	var1 = getVal(opt, split);
	var2 = getVal(opt, split);
	var3 = getVal(opt, split);
	var4 = getVal(opt, split);
}

void FillVal(string options, string split, string prefix,
		double &var1, double &var2, double &var3) {
	double var4;

	FillVal(options, split, prefix, var1, var2, var3, var4);
}

void FillVal(string options, string split, string prefix,
		double &var1, double &var2) {
	double var3;
	double var4;

	FillVal(options, split, prefix, var1, var2, var3, var4);
}

TH1* fillerr(double *v, double *e, TH1* ref) {
	int nb = ref->GetNbinsX();
	TH1D* t = (TH1D*) ref->Clone();
	t->SetName("mean");
	for (int i = 0; i < nb; i++) {
		t->SetBinContent(i + 1, v[i]);
		t->SetBinError(i + 1, e[i]);
	}
	return t;
}

string expandmc(string d, string m) {
	if(m[m.length() - 1] == '/') {
		cout << "Need substitude" << endl;
		// Count "/" in m
		int NSlash = 0;
		for(unsigned int i = 0; i < m.length(); i++) {
			if(m[i] == '/')
				++NSlash;
		}
		// Count the same amount of "/" in d
		string dpart = "";
		for(unsigned int i = 0; i < d.length(); i++) {
			if(d[i] == '/') {
				--NSlash;
				if(NSlash == 0) {
					dpart = d.substr(i + 1, d.length());
					break;
				}
			}
		}
		m += dpart;
		cout << "new m= " << m << endl;
		return m;
	} else {
		return m;
	}
}
}
