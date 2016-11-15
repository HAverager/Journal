#ifndef JOURNALUTILS_H
#define JOURNALUTILS_H

#include "TStyle.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include <TLegend.h>
#include <TROOT.h>
#include <TCanvas.h>

namespace journal_utils {

/// Convert TH1 to TGraphErrors
TGraphErrors* TH1TOTGraph(TH1 *h1);

/// Draw text on canvas
void Text(Double_t x, Double_t y, Color_t color, char *text,
		Double_t tsize = 0, double angle = 0, int font = 0);

/// Draw text on canvas
void Text(Double_t x, Double_t y, Color_t color, std::string text,
		Double_t tsize = 0, double angle = 0, int font = 0);

/// Draw text on canvas inside the box
void BoxText(Double_t x, Double_t y, Double_t boxsize, Int_t mcolor, char *text, Double_t tsize = 18);

/// Draw marker with text caption
void MarkerText(Double_t x, Double_t y, Int_t color, Int_t mstyle, char *text, Double_t tsize = 18,
		double msize = 1);

/// Set journal style settings
void SetJournalStyle(int font = 43, double tsize = 21);

/// Return journal style settings
TStyle* JournalStyle(Int_t font, Double_t tsize);

/// Convert float to string
std::string FtoStr(float N, int p = 3);

/** Set properties of legend for better visualization taking into
 *  account properties of the histogram and pad.
 *
 *  \param legend link to gegend to be configured
 *  \param hist histogram, which is drown on the same pad with legend and
 *  demonstrate typical size of shown histograms
 *  \param pad1 pad, on which this histogram is drown
 *  \param log flag, which shows if the y scale of the plot is logarithmic
 *
 *
 */
void ConfigureLegend(TLegend* legend, TH1* hist, TPad *pad1, bool log);

/** Return value of the X axis, which corresponds to given x coordinate of the canvas.
 *
 */
double GethistX(TH1 *h, double x, double lmargin, double rmargin);

/** Search for a local maximum of the histogram
 *
 *  \param h Pointer to 1D histogram
 *  \param l,r Minimum and maximum bin of the histogram, which select the considered interval.
 *  \return value of maximum
 */
double GetLocalMax(TH1 *h, int l, int r);

/** Search for a local minimum of the histogram
 *
 *  \param h Pointer to 1D histogram
 *  \param l,r Minimum and maximum bin of the histogram, which select the considered interval.
 *  \return value of minimum
 */
double GetLocalMin(TH1 *h, int l, int r);

/** Set color of the histogram and marker type
 *
 */
void SetColor(TH1* hist, int color, int type = 1);

}


#endif
