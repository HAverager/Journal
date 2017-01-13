#include "utils.h"
#include "TLine.h"
#include "TMarker.h"
#include "TLatex.h"
#include "TH1.h"
#include "TROOT.h"
#include "TError.h"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;

namespace journal_utils {

TGraphErrors* TH1TOTGraph(TH1 *h1) {

	if(!h1){
		Error("TH1TOTGraph", "histogram not found");
		return NULL;
	}
	TGraphErrors* g1 = new TGraphErrors();

	Double_t x, y, ex, ey;
	for(Int_t i = 0; i < h1->GetNbinsX(); i++) {
		y = h1->GetBinContent(i);
		ey = h1->GetBinError(i);
		x = h1->GetBinCenter(i);
		ex = h1->GetBinWidth(i);

		g1->SetPoint(i, x, y);
		g1->SetPointError(i, ex, ey);

	}

	return g1;
}

void Text(Double_t x, Double_t y, Color_t color, char *text, Double_t tsize, double angle, int font) {

	TLatex l;
	l.SetTextAlign(12);
	if(tsize > 0) {
		l.SetTextSize(tsize);
	}
	l.SetNDC();
	l.SetTextColor(color);
	l.SetTextAngle(angle);
	if(font > 0) {
		l.SetTextFont(font);
	}
	l.DrawLatex(x, y, text);
}

void Text(Double_t x, Double_t y, Color_t color, std::string text, Double_t tsize, double angle, int font) {
	Text(x, y, color, text.c_str(), tsize, angle, font);
}

void BoxText(Double_t x, Double_t y, Double_t boxsize, Int_t mcolor, char *text, Double_t tsize) {

	Double_t y1 = y - 0.25 * tsize;
	Double_t y2 = y + 0.25 * tsize;
	Double_t x2 = x - 0.3 * tsize;
	Double_t x1 = x2 - boxsize;

	TPave *mbox = new TPave(x1, y1, x2, y2, 0, "NDC");
	Text(x,y,1,text);

	mbox->SetFillColor(mcolor);
	mbox->SetFillStyle(1001);
	mbox->Draw();
}

void MarkerText(Double_t x, Double_t y, Int_t color, Int_t mstyle, char *text, Double_t tsize, double msize) {

	//  printf("**myMarker: text= %s\ m ",text);

	TMarker *marker = new TMarker(x - (0.4 * tsize), y, 8);
	marker->SetMarkerColor(color);
	marker->SetNDC();
	marker->SetMarkerStyle(mstyle);
	marker->SetMarkerSize(msize);
	marker->Draw("hist");

	Text(x,y,1,text);
}

void SetJournalStyle(Int_t font, Double_t tsize) {
	static TStyle* journalStyle = 0;
	Info("SetJournalStyle", "Set Journal style settings");
	if(journalStyle == 0) {
		journalStyle = JournalStyle(font, tsize);
	}
	gROOT->SetStyle("Journal");
	gROOT->ForceStyle();
}

TStyle* JournalStyle(Int_t font, Double_t tsize) {
	TStyle *journalStyle = new TStyle("Journal", "Journal style");

	// use plain black on white colors
	Int_t icol = 0; // WHITE
	journalStyle->SetFrameBorderMode(icol);
	journalStyle->SetFrameFillColor(icol);
	journalStyle->SetCanvasBorderMode(icol);
	journalStyle->SetCanvasColor(icol);
	journalStyle->SetPadBorderMode(icol);
	journalStyle->SetPadColor(icol);
	journalStyle->SetStatColor(icol);

	// set the paper & margin sizes
	journalStyle->SetPaperSize(20, 26);

	// set margin sizes
	journalStyle->SetPadTopMargin(0.03);
	journalStyle->SetPadRightMargin(0.05);
	journalStyle->SetPadBottomMargin(0.16);
	journalStyle->SetPadLeftMargin(0.12);

	// Size of Canvas
	journalStyle->SetCanvasDefH(600);
	journalStyle->SetCanvasDefW(600);

	// set title offsets (for axis label)
	journalStyle->SetTitleXOffset(1.4);
	journalStyle->SetTitleYOffset(1.35);

	// use large fonts
	//Int_t font=72; // Helvetica italics
	//Int_t font=42; // Helvetica
	//Double_t tsize=0.05;
	journalStyle->SetTextFont(font);
	journalStyle->SetTextSize(tsize);

	journalStyle->SetLabelFont(font, "x");
	journalStyle->SetTitleFont(font, "x");
	journalStyle->SetLabelFont(font, "y");
	journalStyle->SetTitleFont(font, "y");
	journalStyle->SetLabelFont(font, "z");
	journalStyle->SetTitleFont(font, "z");

	journalStyle->SetLabelSize(tsize, "x");
	journalStyle->SetTitleSize(tsize, "x");
	journalStyle->SetLabelSize(tsize, "y");
	journalStyle->SetTitleSize(tsize, "y");
	journalStyle->SetLabelSize(tsize, "z");
	journalStyle->SetTitleSize(tsize, "z");

	// use bold lines and markers
	journalStyle->SetMarkerStyle(20);
	journalStyle->SetMarkerSize(1.2);
	journalStyle->SetHistLineWidth(2.);
	journalStyle->SetLineStyleString(2, "[12 12]"); // postscript dashes

	// get rid of X error bars
	//journalStyle->SetErrorX(0.001);
	journalStyle->SetEndErrorSize(0.);

	// do not display any of the standard histogram decorations
	journalStyle->SetOptTitle(0);
	journalStyle->SetOptStat(0); // standard 1111
	journalStyle->SetOptFit(0); // standard 1111
	journalStyle->SetLegendFillColor(0);

	// put tick marks on top and RHS of plots
	journalStyle->SetPadTickX(1);
	journalStyle->SetPadTickY(1);

	journalStyle->SetPalette(1);

	return journalStyle;

}

void ConfigureLegend(TLegend* legend, TH1* hist, TPad *pad1, bool log) {

	// Define style
	// should have no influence because of fixed text and symbol size
	double g_legend_width = 0.2;

	// same symbol width and height for legends with different number of entries
	double g_legend_height_per_entry = 0.05;
	double g_legend_symbol_width = 0.05;

	UInt_t g_smaller_text_size = 21 * 0.95; // in pixel
	UInt_t g_text_font = 43; // force font style 4 and size specification in pixels (font%10==3)

	double legend_width = g_legend_width;
	double legend_height = g_legend_height_per_entry * legend->GetNRows();

	double left_space = 0.03;
	double top_space = 0.05;
	double right_space = 0.15;

	//Find position of maximum
	double max = hist->GetMaximum();
	double nMax = hist->GetMaximumBin();
	double max_x = pad1->GetLeftMargin()
			+ (1 - pad1->GetRightMargin() - pad1->GetLeftMargin()) * (nMax / hist->GetNbinsX());

	Double_t legend_x1;
	Double_t legend_y1;
	Double_t legend_x2;
	Double_t legend_y2;

	// Check, if legend can be placed under hist
	// Find legend position
	cout << "Max: " << max_x << "  " << legend_width << "\n";
	if(max_x - (legend_width / 2) < pad1->GetLeftMargin() + left_space) {
		cout << max_x - (legend_width / 2) << "  " << pad1->GetLeftMargin() + left_space << " L \n";
		legend_x1 = pad1->GetLeftMargin() + left_space;
		legend_x2 = legend_x1 + legend_width;
	} else if(max_x + (legend_width / 2) > 1 - pad1->GetRightMargin() - right_space) {
		cout << max_x + (legend_width / 2) << "  " << 1 - pad1->GetRightMargin() - right_space << " R \n";
		legend_x2 = 1 - pad1->GetRightMargin() - right_space;
		legend_x1 = legend_x2 - legend_width;
	} else {
		legend_x1 = max_x - (legend_width / 2);
		legend_x2 = legend_x1 + legend_width;
	}

	legend_y1 = pad1->GetBottomMargin() + top_space;
	legend_y2 = legend_y1 + legend_height;

	// Check available placed under hist
	int lbin_hist = hist->FindBin(GethistX(hist, legend_x1, pad1->GetLeftMargin(), pad1->GetRightMargin()))
			- 1;
	int rbin_hist = hist->FindBin(GethistX(hist, legend_x2, pad1->GetLeftMargin(), pad1->GetRightMargin()))
			- 1;

	// Find local minimum under legend
	double locmin = GetLocalMin(hist, lbin_hist, rbin_hist);
	double locmin_y = pad1->GetBottomMargin()
			+ (1 - pad1->GetTopMargin() - pad1->GetBottomMargin()) * (locmin / max);
	cout << legend_x1 << " x,x " << legend_x2 << " \n";
	cout << lbin_hist << "  " << rbin_hist << " \n";
	cout << locmin_y << " min, y2 " << legend_y2 << " \n";

	// If not ok, try other position
	if(locmin_y < legend_y2) {

		// find best position at top: left, right or middle
		double max_l = GetLocalMax(hist, 1, int(hist->GetNbinsX() / 3) + 1);
		double max_m = GetLocalMax(hist, int(hist->GetNbinsX() / 3) - 1, int(2 * hist->GetNbinsX() / 3) + 1);
		double max_r = GetLocalMax(hist, int(2 * hist->GetNbinsX() / 3) - 1, hist->GetNbinsX());

		if(max_l < max_m && max_l < max_r) {
			legend_x1 = pad1->GetLeftMargin() + left_space;
		} else if(max_r < max_m && max_r < max_l) {
			legend_x1 = 1 - pad1->GetRightMargin() - right_space - legend_width;
		} else {
			legend_x1 = 0.5 - (legend_width / 2);
		}
		legend_y1 = 1 - pad1->GetTopMargin() - top_space - legend_height;
		legend_x2 = legend_x1 + legend_width;
		legend_y2 = legend_y1 + legend_height;

		int lbin_hist = hist->FindBin(
				GethistX(hist, legend_x1, pad1->GetLeftMargin(), pad1->GetRightMargin())) - 1;
		int rbin_hist = hist->FindBin(
				GethistX(hist, legend_x2, pad1->GetLeftMargin(), pad1->GetRightMargin())) - 1;

		// Find local minimum under legend
		double locmax = GetLocalMax(hist, lbin_hist, rbin_hist);
		cout << lbin_hist << "  " << rbin_hist << "\n";
		cout << " locmax " << locmax << "\n";

		// Perform scaling
		double hist_height = 1 - pad1->GetTopMargin() - pad1->GetBottomMargin();
		double scale = (1 / max) * (locmax * hist_height / (legend_y2 - pad1->GetBottomMargin() - top_space));

		if(log) {
			cout << "Log \n";
			scale = pow10(log10(max) * scale) / max;
		}

		cout << "hist_height" << hist_height << "\n";
		cout << "legend_height" << legend_height << "  top_space" << top_space << "\n";
		cout << 1 - (legend_height / hist_height) - top_space + 0.15 << " \n";
		cout << "Max: " << max * scale << "  " << scale << "  " << max << "\n";
		if(scale > 1) {
			hist->SetMaximum(max * scale);
		}

	}

	// Set legend parameters
	legend->SetX1NDC(legend_x1);
	legend->SetX2NDC(legend_x2);
	legend->SetY1NDC(legend_y1);
	legend->SetY2NDC(legend_y2);
	legend->SetTextAlign(12);
	legend->SetMargin(g_legend_symbol_width / g_legend_width);
	legend->SetTextFont(g_text_font);
	legend->SetTextSize(g_smaller_text_size);
	legend->SetFillStyle(0);
	legend->SetBorderSize(0);

}

double GethistX(TH1 *h, double x, double lmargin, double rmargin) {
	double r_hist = (x - lmargin) / (1 - x - rmargin);
	double x_hist = h->GetBinLowEdge(1)
			+ r_hist * (h->GetBinLowEdge(h->GetNbinsX() + 1) - h->GetBinLowEdge(1));

	return x_hist;
}

double GetLocalMax(TH1 *h, int l, int r) {
	double locmax = h->GetMinimum();
	for(int i = l; i <= r; i++) {
		if(locmax < h->GetBinContent(i)) {
			locmax = h->GetBinContent(i);
		}
	}
	return locmax;
}

double GetLocalMin(TH1 *h, int l, int r) {
	double locmin = h->GetMaximum();
	for(int i = l; i <= r; i++) {
		if(locmin > h->GetBinContent(i)) {
			locmin = h->GetBinContent(i);
		}
	}
	return locmin;
}

string FtoStr(float N, int p) {
	std::stringstream out;
	out << std::setprecision(p) << N;
	string s = out.str();
	return s;
}

void SetColor(TH1* hist, int color, int type) {

	int colors[100];
	int markers[100];

	colors[0] = kBlack;
	colors[1] = kRed;
	colors[2] = kBlue;
	colors[3] = kGreen + 2;
	colors[4] = kViolet;
	colors[5] = kCyan + 2;
	colors[6] = kOrange - 3;
	colors[7] = kYellow - 2;
	colors[8] = kRed - 2;
	colors[9] = kBlue - 1;
	colors[10] = kGreen - 1;
	colors[11] = kViolet - 6;
	colors[12] = kCyan - 7;
	colors[13] = kOrange + 4;
	colors[14] = kYellow + 1;
	colors[15] = kPink + 10;
	colors[16] = kGray + 2;
	colors[17] = kSpring + 10;
	colors[18] = kViolet - 8;
	colors[19] = kAzure + 9;
	colors[20] = kSpring + 4;
	colors[21] = kOrange - 7;
	colors[22] = kAzure - 3;
	colors[23] = kMagenta - 1;
	colors[24] = kRed - 8;
	colors[25] = kBlue - 10;
	colors[26] = kTeal + 2;
	colors[27] = kViolet + 2;
	colors[28] = kGreen + 4;
	colors[29] = kBlue - 1;
	colors[30] = kGreen - 1;
	colors[31] = kViolet - 6;
	colors[32] = kCyan - 7;
	colors[33] = kOrange + 4;
	colors[34] = kYellow + 1;
	colors[35] = kPink + 10;
	colors[36] = kGray + 2;
	colors[37] = kSpring + 10;
	colors[38] = kViolet - 8;
	colors[39] = kAzure + 9;
	colors[40] = kSpring + 4;
	colors[41] = kOrange - 7;
	colors[42] = kAzure - 3;
	colors[43] = kMagenta - 1;
	colors[44] = kRed - 8;
	colors[45] = kBlue - 10;
	colors[46] = kTeal + 2;
	colors[47] = kViolet + 2;
	colors[48] = kGreen + 4;
	for(int i = 49; i < 99; i++)
		colors[i] = 1;

	colors[99] = 0;

	markers[0] = 20;
	markers[1] = 24;
	markers[2] = 25;
	markers[3] = 26;
	markers[4] = 27;
	markers[5] = 28;
	markers[6] = 30;
	markers[7] = 31;
	markers[8] = 32;
	markers[9] = 21;
	markers[10] = 22;
	markers[11] = 23;
	markers[12] = 29;
	markers[13] = 33;
	markers[14] = 34;
	markers[15] = 5;
	markers[16] = 2;
	markers[17] = 24;
	markers[18] = 25;
	markers[19] = 26;
	markers[20] = 27;
	markers[21] = 28;
	markers[22] = 30;
	markers[23] = 31;
	markers[24] = 32;
	markers[25] = 21;
	markers[26] = 22;
	markers[27] = 23;
	markers[28] = 29;
	markers[29] = 22;
	markers[30] = 23;
	markers[31] = 29;
	markers[32] = 33;
	markers[33] = 34;
	markers[34] = 5;
	markers[35] = 2;
	markers[36] = 24;
	markers[37] = 25;
	markers[38] = 26;
	markers[39] = 27;
	markers[40] = 28;
	markers[41] = 30;
	markers[42] = 31;
	markers[43] = 32;
	markers[44] = 21;
	markers[45] = 22;
	markers[46] = 23;
	markers[47] = 29;
	markers[48] = 21;
	for(int i = 49; i < 100; i++)
		markers[i] = 20;

	hist->SetLineColor(colors[color]);
	hist->SetMarkerColor(colors[color]);
	hist->SetMarkerStyle(markers[color]);
	if(type == 2) {
		hist->SetFillColor(colors[color]);
	}

}

}
