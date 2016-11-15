#include <journal.h>
#include <utils.h>
#include <parser.h>
#include <hparser.h>

#include <TROOT.h>
#include <TCanvas.h>
#include <TError.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TLegend.h>

using std::endl;
using journal_utils::SetColor;
using journal_utils::FtoStr;
using journal_parser::getOption;

void journal() {
	Info("journal", "Load journal package 00001");
}

TH1* process(string b) {
	hparser * hpars = new hparser(b, 1);
	TH1 *h = hpars->eval();
	delete hpars;
	return h;
}

// Plot single histogram
void hpl(string a, string options, string xtit, string ytit, string title) {
	TH1 *h = hcopy(a, "hpl");

	// Read and Set properties of the hist.
	double minx = h->GetXaxis()->GetXmin();
	double maxx = h->GetXaxis()->GetXmax();
	double miny = h->GetMinimum();
	double maxy = h->GetMaximum() * 1.1;
	double minz = h->GetMinimum();
	double maxz = h->GetMaximum();
	double xOffset = 0;
	double yOffset = 0;
	double x = 600;
	double y = 600;
	int color = 1;
	int logy = 0;
	int logx = 0;
	int logz = 0;
	string hdopt = "";

	getOption(options, "minx=", minx);
	getOption(options, "maxx=", maxx);
	getOption(options, "miny=", miny);
	getOption(options, "maxy=", maxy);
	getOption(options, "minz=", minz);
	getOption(options, "maxz=", maxz);
	getOption(options, "xOffset=", xOffset);
	getOption(options, "yOffset=", yOffset);
	getOption(options, "X=", x);
	getOption(options, "Y=", y);
	getOption(options, "logy=", logy);
	getOption(options, "logx=", logx);
	getOption(options, "logz=", logz);
	getOption(options, "color=", color);
	getOption(options, "hopt=", hdopt);
	getOption(options, "tity=", ytit);
	getOption(options, "titx=", xtit);
	getOption(options, "title=", title);

	// Set drawing properties
	if(miny != maxy) {
		h->GetYaxis()->SetRangeUser(miny, maxy);
	}
	if(minx != maxx) {
		h->GetXaxis()->SetRangeUser(minx, maxx);
	}
	if(xOffset > 0) {
		h->GetXaxis()->SetTitleOffset(xOffset);
	}
	if(yOffset > 0) {
		h->GetYaxis()->SetTitleOffset(yOffset);
	}
	if(xtit != "") {
		h->SetXTitle(xtit.c_str());
	}
	if(ytit != "") {
		h->SetYTitle(ytit.c_str());
	}
	if(title != "") {
		h->SetTitle(title.c_str());
	} else {
		title = "Test";
	}
	if(color) {
		h->SetLineColor(color);
	}

	// Drawing
	// Create new canvas
	// If it already exists, old will be deleted
	TCanvas* can = new TCanvas(title.c_str(), title.c_str(), x, y);
	can->cd();
	can->Clear();
	if(hdopt == "ColZ") {
		h->SetMaximum(maxz);
		h->SetMinimum(minz);
		gPad->SetRightMargin(0.16);
		gPad->SetTopMargin(0.06);
		gPad->SetLogz(logz);
	}
	if(hdopt == "same") {
		cout << "Warning: options 'same' is not supported " << endl;
	}

	h->DrawCopy(hdopt.c_str());

	gPad->SetLogy(logy);
	gPad->SetLogx(logx);
	gPad->Update();

	can->Print((title + ".eps").c_str());

}

TH1* hcopy(string a, string name) {

	while (gDirectory->Get("templocal")) {
		gDirectory->Get("templocal")->Delete();
	}

	TH1 *h = process(a);
	if(h==NULL){
		Error("hcopy", "Failed to process line %s", a.c_str());
		exit(1);
	}
	h->SetName(name.c_str());
	return h;
}

void overall(){

	Printf("Function to plot several histograms on top of each other ");
	Printf(" \"//histdata.root/hist\", \"//histmc.root/hist&&//histmc2.root/hist\" ");
	Printf("If name of MC hist is not given, it will use the name of data hist ");
	Printf(" \"//histdata.root/hist\", \"//histmc.root/&&//histmc2.root/\" ");

	Printf("\n After the name of histogram some modified can be used: like  hname.z.[0: 30] ");
	Printf("Following modifier are available: ");
	Printf(".z.[-2:4] \n\t zoom histogram in x axis between -2 and 4 ");
	Printf(".px.[3:5.3] \n\t cut an X slice of 2D histogram, where Y value is taken between 3 and 5.3 ");
	Printf(".py.[3:5.3] \n\t similar as .px ");
	Printf(".resx \n\t create 1D hist using slices of 2D hist, where value is RMS() of slices ");
	Printf(".ppx \n\t create 1D hist using slices of 2D hist, where value is Mean() of slices ");
	Printf(".abs \n\t create histograms with absolute binning from hist with sing binning: [-2, 2] => [0, 2] ");
	Printf(".bs \n\t divite values to size of the bins ");
	Printf(".ubs \n\t multiply values to size of the bins ");
	Printf(".norm \n\t Normalize histogram to integral and divide to bin size ");
	Printf(".ubin \n\t Create histogram with unified binning ");

	Printf("\n Following options are available: ");
	Printf("scale=0 \n\t scale second histogram to the first. scale=0 -- no scale");
	Printf("chi2=1 \n\t Plot Chi2 ");
	Printf("logy=0 \n\t log scale for Y axis");
	Printf("logx=0 \n\t log scale for X axis");
	Printf("ratio=0 \n\t Several plotting options. 0 - overlay plot, 1 - ration, 2 - both");
	Printf("minx= \n\t min value for X axis");
	Printf("maxx= \n\t max value for X axis ");
	Printf("miny= \n\t min value for Y axis");
	Printf("maxy= \n\t max value for Y axis");
	Printf("minr=0.79 \n\t min value for ration plot in case of option ratio=2");
	Printf("maxr=1.21 \n\t max value for ration plot in case of option ratio=2");
	Printf("tsize= \n\t font size");
	Printf("title=data \n\t title of canvas");
	Printf("dataname= \n\t name of first hist");
	Printf("bgname= \n\t name of 3rd hist (if exist)");
	Printf("mcname0(1,2,3,..)= \n\t name of secont(3rd, ..) hist");
	Printf("FColor0(1,2,3,..)= \n\t color of secont(3rd, ..) hist");
	Printf("legend=0 \n\t 1 - plot legend, 2 - find place for legend ");
	Printf("tity= \n\t Y axis title");
	Printf("titx= \n\t X axis title");
	Printf("titr= \n\t Title for ration plot for option ratio=2");
	Printf("rebin= \n\t rebinng of the histogram");
	Printf("dataopt= \n\t option for plotting of the first hist (like p, hist, ..)");
	Printf("mcopt= \n\t option for plotting of the second hist (like p, hist, ..)");
	Printf("binanal='.px[min, step, N1, N2]' \n\t plot histograms sliced over X axis in canvas NxN from value min");
	Printf("split=0 \n\t Plot binanal on individual canvas");
}

void overall(string d, string m, string options) {
	// Fill "data" histogram
	TH1 *dh = hcopy(d, "data");

	// Fill vector of "MC" histograms
	std::vector<TH1*> mhv;
	int counter = 0;
	string mcstr = m;
	while(mcstr.length() > 3) {
		counter++;
		int k = mcstr.find("&&");
		string sc = journal_parser::expandmc(d, mcstr.substr(0, k));
		mhv.push_back(hcopy(sc, "mc" + FtoStr(counter)));
		mcstr = mcstr.substr(k + 2, mcstr.length());
	}

	// Default options:
	double scale = 0;
	int ratio = 0;
	int rebin = 1;
	int split = 0;
	string binanal = "";

	// zoom
	double xmin = dh->GetXaxis()->GetXmin();
	double xmax = dh->GetXaxis()->GetXmax();
	double ymin = dh->GetMinimum() - (0.15 * (dh->GetMaximum() - dh->GetMinimum()));
	double ymax = dh->GetMaximum() + (0.15 * (dh->GetMaximum() - dh->GetMinimum()));
	double rmin = 0.89;
	double rmax = 1.11;

	// stat. box
	int stat1 = 1111;
	int stat2 = 0;

	// chi2 test:
	int ichi2 = 1;

	// titles
	string title = dh->GetName();
	string xtit = "";
	string ytit = "";
	string rtit = "";

	// names for stat. box:
	string dataname = "";
	string mcname[10];

	// Drawing options
	string dhopt = "pe";
	string mhopt = "hist";

	// log y/x
	int logy = 0;
	int logx = 0;

	// legend
	int legend = 0;

	// tsize
	double tsize = 0;

	// Fill color
	int fcolor[10];

	// Get all options
	getOption(options, "scale=", scale);
	getOption(options, "chi2=", ichi2);
	getOption(options, "logy=", logy);
	getOption(options, "logx=", logx);
	getOption(options, "ratio=", ratio);
	getOption(options, "minx=", xmin);
	getOption(options, "maxx=", xmax);
	getOption(options, "miny=", ymin);
	getOption(options, "maxy=", ymax);
	getOption(options, "minr=", rmin);
	getOption(options, "maxr=", rmax);
	getOption(options, "tsize=", tsize);
	getOption(options, "stat1=", stat1);
	getOption(options, "stat2=", stat2);
	getOption(options, "FColor=", fcolor[0]);
	for(unsigned int nh = 0; nh < mhv.size(); nh++) {
		getOption(options, "FColor" + FtoStr(nh) + "=", fcolor[nh]);
	}
	getOption(options, "title=", title);
	getOption(options, "dataname=", dataname);
	getOption(options, "mcname=", mcname[0]);
	for(unsigned int nh = 0; nh < mhv.size(); nh++) {
		getOption(options, "mcname" + FtoStr(nh) + "=", mcname[nh]);
	}
	getOption(options, "dataopt=", dhopt);
	getOption(options, "mcopt=", mhopt);
	getOption(options, "legend=", legend);
	getOption(options, "tity=", ytit);
	getOption(options, "titx=", xtit);
	getOption(options, "titr=", rtit);
	getOption(options, "rebin=", rebin);
	getOption(options, "split=", split);
	getOption(options, "binanal=", binanal);

	// Set new default if case of plotting ratio
	if(ratio == 1) {
		ymin = 0.5;
		ymax = 1.5;
	}

	// Drawing
	TCanvas* can = NULL;
	// Check, if plotting slices
	if(binanal != "" && binanal != "X") {
		// If plotting all hists in one canvas
		if(split == 0) {
			can = new TCanvas(title.c_str(), title.c_str());
		}
		double promin, step, N1, N2;
		string prox = "";
		TH1* h;
		if(binanal.find("px") == 1) {
			prox = ".px";
			journal_parser::FillVal(binanal, ":", ".px", promin, step, N1, N2);
			h = process(d + ".py");
		}
		if(binanal.find("py") == 1) {
			prox = ".py";
			journal_parser::FillVal(binanal, ":", ".py", promin, step, N1, N2);
			h = process(d + ".px");
		}
		if(m.length() > 3) {
			m = journal_parser::expandmc(d, m);
		}

		Info("overall", "Plot histograms in %s slices \n", prox.c_str());
		Info("overall", "Plot %f X %f with step: %f, starting from value: %f \n", N1, N2, step, promin);

		// Plotting
		h->Rebin(step);
		prox = prox + ".[";

		// If plotting in one canvas
		if(split == 0) {
			can->SetBottomMargin(0);
			can->SetTopMargin(0.0);
			can->SetRightMargin(0.0);
			can->Divide(N1, N2);
		}

		// Loop over all pads
		int prominbin = h->FindBin(promin);
		for(int i = 1; i <= N1 * N2; i++) {
			if(i > h->GetNbinsX()) {
				break;
			}
			if(split == 0) {
				can->cd(i);
			}
			double min = h->GetBinLowEdge(prominbin + i - 1);
			double max = h->GetBinLowEdge(prominbin + i);
			// create new path
			string d2 = d + prox + FtoStr(min) + ":" + FtoStr(max) + "]";
			string m2 = "";
			if(m.length() > 0) {
				m2 = m + prox + FtoStr(min) + ":" + FtoStr(max) + "]";
			}
			if(split == 0) {
				overall(d2, m2, "binanal='X' title='" + FtoStr(min) + "' " + options);

				journal_utils::Text(0.65, 0.9, 1, (FtoStr(min) + "-" + FtoStr(max)).c_str());

			} else {
				overall(d2, m2,
						"binanal='' title='" + title + "_" + FtoStr(min) + "_" + FtoStr(max) + "' "
								+ "tity='' titx='" + xtit + ", at " + ytit + ": " + FtoStr(min) + "-"
								+ FtoStr(max) + "'" + options);
			}
		}
		if(split == 0) {
			can->Print((title + ".eps").c_str());
		}
		return;
	}
	if(binanal == "") {
		cout << "Create canvas \n";
		can = new TCanvas(title.c_str(), title.c_str());
	}

	// Chi2 test
	double chi2;
	int ndf;
	int igood;
	if(chi2 == 1 && mhv.size() > 0) {
		dh->Chi2TestX(mhv.at(0), chi2, ndf, igood);
		Info("overall", "Chi2/dof=%f / %i", chi2, ndf);
	}

	// Apply options:
	if(xtit != "") {
		dh->SetXTitle(xtit.c_str());
	}
	if(ytit != "") {
		dh->SetYTitle(ytit.c_str());
	}
	if(title != "") {
		dh->SetTitle(title.c_str());
	}
	if(rebin > 1) {
		dh->Rebin(rebin);
		for(unsigned int nh = 0; nh < mhv.size(); nh++) {
			mhv.at(nh)->Rebin(rebin);
		}
	}

	dh->SetAxisRange(xmin, xmax);
	dh->SetMinimum(ymin);
	dh->SetMaximum(ymax);
	Info("overall", "Mean: %f RMS: %f  Int  %f \n", dh->GetMean(), dh->GetRMS(), dh->Integral());
	if(scale == 0) {
		for(unsigned int nh = 0; nh < mhv.size(); nh++) {
			Info("overall", "Scale = %f\n", dh->GetSumOfWeights() / mhv.at(nh)->GetSumOfWeights());

			mhv.at(nh)->Scale(dh->GetSumOfWeights() / mhv.at(nh)->GetSumOfWeights());
			Info("overall", "Mean: %f RMS: %f  Int  %f  \n", mhv.at(nh)->GetMean(), mhv.at(nh)->GetRMS(),
					mhv.at(nh)->Integral());
		}
	} else {
		for(unsigned int nh = 0; nh < mhv.size(); nh++) {
			mhv.at(nh)->Scale(scale);
			Info("overall", "Mean: %f RMS: %f  Int  %f  \n", mhv.at(nh)->GetMean(), mhv.at(nh)->GetRMS(),
					mhv.at(nh)->Integral());
		}
	}

	if(dataname != "") {
		dh->SetName(dataname.c_str());
	}

	for(unsigned int nh = 0; nh < mhv.size(); nh++) {
		if(mcname[nh] != "") {
			mhv.at(nh)->SetName(mcname[nh].c_str());
			mhv.at(nh)->SetTitle(mcname[nh].c_str());
		}
	}

	SetColor(dh, 0);
	for(unsigned int nh = 0; nh < mhv.size(); nh++) {
		if(fcolor[nh] > 0 && fcolor[nh] < 100) {
			cout << "SetColor: " << fcolor[nh] << "  " << nh << "\n";
			SetColor(mhv.at(nh), fcolor[nh]);
		} else {
			SetColor(mhv.at(nh), 1 + nh);
		}
	}

	if(tsize > 0) {
		dh->SetTitleSize(tsize, "X");
	}

	// Create legend
	TLegend *l = new TLegend();
	l->SetFillColor(0);
	l->SetBorderSize(0);
	string lmarker = "l";
	if(mhopt == "") {
		lmarker = "lp";
	}
	l->AddEntry(dh, dataname.c_str(), lmarker.c_str());
	for(unsigned int nh = 0; nh < mhv.size(); nh++) {
		l->AddEntry(mhv.at(nh), mcname[nh].c_str(), lmarker.c_str());
	}

	// Plotting
	if(ratio == 1) {
		Printf("Plot simple ratio \n");
		TPad *pad1 = new TPad("pad1", "pad1", 0, 0, 1., 1.);
		pad1->Draw();
		pad1->cd();
		pad1->SetLogy(logy);
		pad1->SetLogx(logx);

		dh->Sumw2();
		dh->Divide(mhv.at(0));
		dh->Draw("e");
		for(unsigned int nh = 1; nh < mhv.size(); nh++) {
			TH1D *dhBot = (TH1D*) mhv.at(nh)->Clone();
			dhBot->Divide(mhv.at(0));
			dhBot->Draw("e same");
		}
		if(legend > 0) {
			if(legend == 2) {
				journal_utils::ConfigureLegend(l, dh, pad1, logy);
			} else {
				l->SetX1NDC(0.5);
				l->SetX2NDC(0.75);
				l->SetY1NDC(0.65);
				l->SetY2NDC(0.85);
			}
			l->Draw("same");
		}

	} else if(ratio == 2) {
		Printf("Plot nice plot \n");
		TPad *pad1 = new TPad("pad1", "pad1", .0, .3, 1., 1.);
		TPad *pad2 = new TPad("pad2", "pad2", .0, .0, 1., .3);

		pad1->SetTopMargin(0.05);
		pad1->SetBottomMargin(0.01);
		pad1->SetRightMargin(0.05);
		pad1->SetLeftMargin(2.1 * 0.05);

		pad2->SetTopMargin(0.0);
		pad2->SetBottomMargin(2.1 * 0.05 / 0.3);
		pad2->SetRightMargin(0.05);
		pad2->SetLeftMargin(2.1 * 0.05);

		pad2->SetGrid(0, 2);
		pad2->Draw();
		pad1->Draw();

		// Plot top pad
		pad1->cd();
		pad1->SetLogy(logy);
		pad1->SetLogx(logx);

		dh->Draw(dhopt.c_str());
		for(unsigned int nh = 0; nh < mhv.size(); nh++) {
			mhv.at(nh)->Draw(("same " + mhopt).c_str());
		}
		dh->Draw(("same " + dhopt).c_str());

		if(legend > 0) {
			if(legend == 2) {
				journal_utils::ConfigureLegend(l, dh, pad1, logy);
			} else {
				l->SetX1NDC(0.5);
				l->SetX2NDC(0.75);
				l->SetY1NDC(0.65);
				l->SetY2NDC(0.85);
			}
			l->Draw("same");
		}

		// Plot bottom pad
		pad2->cd();
		pad2->SetLogx(logx);

		TH1D *dhBot = (TH1D*) dh->Clone();
		dhBot->Sumw2();
		dhBot->Divide(mhv.at(0));
		dhBot->Draw("e");

		if(rmin == 0) {
			double max = dhBot->GetBinContent(dhBot->GetMaximumBin());
			double min = dhBot->GetBinContent(dhBot->GetMinimumBin());

			rmin = min - 0.15 * (max - min);
			rmax = max + 0.15 * (max - min);
		}

		for(unsigned int nh = 1; nh < mhv.size(); nh++) {
			TH1D *dhBot = (TH1D*) dh->Clone();
			dhBot->Divide(mhv.at(nh));
			SetColor(dhBot, nh + 1);
			dhBot->Draw("e same");
		}

		dhBot->GetYaxis()->SetNdivisions(6, 0, 0);
		dhBot->GetYaxis()->SetTitle(rtit.c_str());
		dhBot->GetXaxis()->SetTitleOffset(2.8);
		dhBot->GetYaxis()->SetRangeUser(rmin, rmax);

	} else if(ratio == 0) {
		TPad *pad1 = new TPad("pad1", "pad1", 0, 0, 1., 1.);
		pad1->Draw();
		pad1->cd();
		pad1->SetLogy(logy);
		pad1->SetLogx(logx);

		// Set drawing options for binanal
		if(binanal == "X") {
			pad1->SetMargin(0.14, 0.01, 0.14, 0.01);
			dh->GetYaxis()->SetLabelSize(16);
			dh->GetYaxis()->SetMoreLogLabels(1);
			dh->GetXaxis()->SetLabelSize(16);
			dh->SetMarkerSize(0.7);
		}

		dh->Draw(dhopt.c_str());

		if(stat2 == 0) {
			for(unsigned int nh = 0; nh < mhv.size(); nh++) {
				mhv.at(nh)->Draw(("same " + mhopt).c_str());
			}
		} else {
			gStyle->SetStatY(0.7);
			gStyle->SetOptStat(stat2);

			for(unsigned int nh = 0; nh < mhv.size(); nh++) {
				mhv.at(nh)->Draw(("sames " + mhopt).c_str());
			}
		}
		dh->Draw(("same " + dhopt).c_str());

		if(legend) {
			if(legend == 2) {
				journal_utils::ConfigureLegend(l, dh, pad1, logy);
			} else {
				l->SetX1NDC(0.5);
				l->SetX2NDC(0.75);
				l->SetY1NDC(0.65);
				l->SetY2NDC(0.85);
			}
			l->Draw("same");
		}
	} else {
		cout << "Only options 0,1,2 of the ratio are supported " << endl;
	}

	if(ichi2) {
		double x = gPad->PixeltoX(gPad->UtoPixel(0.1));
		double y = gPad->PixeltoY(gPad->VtoPixel(0.925) - gPad->GetWh());

		char buf[50];
		sprintf(buf, "#chi^{2}/dof=%5.1f/%d", chi2, ndf);

		journal_utils::Text(x,y,1,buf);

	}
	if(binanal == "") {
		can->Print((title + ".eps").c_str());
	}
}


