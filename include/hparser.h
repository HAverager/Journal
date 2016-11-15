#pragma once

#include <parser.h>

#include <TROOT.h>
#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include <sstream>
#include "TF1.h"

// Parser for histogram / double arithmetics

const int typeDval = 1; ///< type double
const int typeHval = 2; ///< type histogram

/** Class-container to store type of the object (double or histogram) and object itself.
 *
 */
class HParserNodeResult {
public:
	int type;
	double dVal;
	TH1* hVal;
};

/** Elaborate parsing of the string. Recognise the path and name of the histogram, operations to be performed.
 *  Perform foreseened manipulations.
 */
class hparser: public journal_parser::binarynode {
public:
	/** Contructor
	 *
	 * 	\param s Path to the histogram(s)
	 * 	\param preproc flag stitching on preprocessing of the string `s`.
	 */
	hparser(string s, int preproc = 1);
	virtual ~hparser();

	/** Perform manipulations with objects (double(s) and histogram(s)).
	 *  Return pointer to the resulting histogram, if resulting object is histogram.
	 */
	TH1* eval();
private:
	/** Perform manipulations with objects (double(s) and histogram(s)).
	 *  \return Pointer to result
	 */
	HParserNodeResult evalNode();

	/** Read histogram from rool file / create from text file.
	 * 	Apply modification to the histogram.
	 */
	TH1* sValTohVal();

	/** Perform operations between two doubles.
	 * 	The operation is defined by variable `operation`
	 */
	double operate(double left, double right);

	/** Perform operation between double and histogram
	 *  The operation is defined by variable `operation`
	 */
	TH1* operate(double left, TH1* right);

	/** Perform operations between two histograms
	 *  The operation is defined by variable `operation`
	 */
	TH1* operate(TH1* left, TH1* right);
};

TH1* ApplyModifier(TH1* hist, string Modifier);
