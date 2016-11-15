/** Several functions to produce plots with one line of code
 *
 */

#ifndef JOURNAL_
#define JOURNAL_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <TH1.h>

// Declare functions:
using std::endl;
using std::string;
using std::cout;

/// Pring message, that packate loaded
void journal();

/** Perform the parsing of the histogram name
 *
 *  \param b path to histogram
 *  \return pointer to the resulting histogram
 */
TH1* process(string b);

/** Read histogram(s) from file, perform manupulations and plot.
 *   Funtion is suitable to plot only one histogram per call on the canvas.
 *
 *  \param a path to the histogram in the file
 *  \param options options to modify the histogram and the plot
 *  \param xtit,ytit title of x and y axises
 *  \param title title of the histogram. User as a file name by saving
 *
 *  Example usage
 *  ~~~{.cpp}
 *    hpl("//file.root/hist1", "xOffset=1 yOffset=1 same=0 color=1",
 *    xtit = "pT", ytit="N Evt", title="ZptDistr")
 *  ~~~
 */
void hpl(string a, string options = "xOffset=1 yOffset=1 same=0 color=1 logy=0 logx=0", string xtit = "",
		string ytit = "", string title = "");

/** Read histogram from file and perform manipulation
 *
 *	\param a path to histogram in the file
 *	\param name name of the returning histogram
 *  \return pointer to resulting histogram
 *
 */
TH1* hcopy(string a, string name = "hcopy");

/** Read several histograms from file, perform modifications and plot them
 *   Similar to hpl function, but suitable for plotting several histograms on one canvas.
 *
 *  \param d path to the histogram in the file
 *
 *  \param options options to modify the histogram and the plot
 */
void overall(string d, string m, string options = "scale=0 ratio=0 chi2=1");

/// Print help of the funtions
void overall();


#endif
