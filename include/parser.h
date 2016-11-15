#ifndef PARSER_H
#define PARSER_H

#include <string>

#include <TROOT.h>
#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include <sstream>
#include "TF1.h"

using std::string;

namespace journal_parser {

const int operationsCount = 5;
const int highestOrder = 2;
// % -- standard division,  # -- binary division
const char operations[] = "+-*%#";
const int operationsOrder[operationsCount] = { 1, 1, 2, 2, 2 };

const int typeOperation = 1;
const int typeDValue = 2;
const int typeSValue = 3;
const int typeUnaryOperation = 4; // sValue contains modifier

class binarynode {
public:
	binarynode(string s, int preproc = 1);
	virtual ~binarynode();
	void print();
	string eval();
	int getType();
protected:
	int type;
	binarynode* leftnode;
	binarynode* rightnode;
	char operation;
	string svalue;

private:
	/** Check if given character is a bracket or one of the operation
	 *
	 *  \return 1 if true, 0 if false
	 */
	int isSeparator(char c);

	/** Pre-process the string s.
	 *
	 */
	string preprocess(string s);

	/**
	 *
	 */
	string expandFileName(string FileName, string s);
};

/** Find parameter in the list of parameters. Fill the value of this parameter in given variable.
 *
 *  \param options string which contains all parameters
 *  \param opt name of parameter to consider
 *  \param var variable to be filled with respect of given string
 *
 *  Example usage
 *  ~~~{.cpp}
 *    double var;
 *    getOption("scale=0.135 ", "scale=", var");
 *  ~~~
 *
 *  In this example variable var will get value 0.135
 */
void getOption(string options, string opt, double &var);
void getOption(string options, string opt, int &var);
void getOption(string options, string opt, string &var);
void getOption(string options, string opt, bool &var);

/** Convert the string until the first delimiter to double.
 *
 *  \param options string to be considered
 *  \param opt delimiter
 *  \return result of conversion string to double
 *
 *  Example usage
 *  ~~~{.cpp}
 *    double var = getVal("23.4:12.4:15", ":");
 *  ~~~
 *
 *  value of var in this example is 23.4
 */
double getVal(string &options, string opt);

/** Split input string to saveral substrungs using given deliminator.
 *  Convert substrings to double and fill in variables.
 *
 */
void FillVal(string a, string split, string prefix,
		double &var1, double &var2);
void FillVal(string a, string split, string prefix,
		double &var1, double &var2, double &var3);
void FillVal(string a, string split, string prefix,
		double &var1, double &var2, double &var3, double &var4);


/** Clone the histogramm and fill it with given values and uncertainties
 *
 *   \param array of values
 *   \param array of uncertainties
 *   \param ref Reference histogram to clone
 *   \return Pointer to resulting histogram
 */
TH1* fillerr(double *v, double *e, TH1* ref);

/** Complete path to histograms.
 *
 *  \param d path to first histogram, which inludes file name
 *  \param m path to second histogram, which may not include file name
 *
 *  \return path to second histogram, which .....
 *
 */
string expandmc(string d, string m);
}




#endif

