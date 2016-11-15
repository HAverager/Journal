#include <hparser.h>
#include <iostream>
#include <TROOT.h>
#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include <sstream>
#include "TF1.h"
#include <fstream>
#include <TKey.h>
#include <TRegexp.h>
#include "TError.h"

using std::cout;
using std::endl;

using namespace journal_parser;

hparser::hparser(string s, int preproc) :
		binarynode(s, preproc) {
}

hparser::~hparser() {
}

TH1* hparser::eval() {
	HParserNodeResult tmp = this->evalNode();
	if(tmp.type == typeDval) {
		Info("hparser::eval", "Result = %f", tmp.dVal);
		throw 23;
		return NULL;
	} else {
		return tmp.hVal;
	}
}

HParserNodeResult hparser::evalNode() {
	HParserNodeResult result = { 0, 0, NULL };

	if(type == typeOperation) {
		HParserNodeResult leftRes = static_cast<hparser*>(leftnode)->evalNode();
		HParserNodeResult rightRes = static_cast<hparser*>(rightnode)->evalNode();
		// Cases
		if((leftRes.type == typeDval) && (rightRes.type == typeDval)) {
			result.type = typeDval;
			result.dVal = operate(leftRes.dVal, rightRes.dVal);
		} else if((leftRes.type == typeHval) && (rightRes.type == typeHval)) {
			result.type = typeHval;
			result.hVal = operate(leftRes.hVal, rightRes.hVal);
		} else if((leftRes.type == typeDval) && (rightRes.type == typeHval)) {
			result.type = typeHval;
			result.hVal = operate(leftRes.dVal, rightRes.hVal);
		} else if((leftRes.type == typeHval) && (rightRes.type == typeDval)) {
			result.type = typeHval;
			result.hVal = operate(rightRes.dVal, leftRes.hVal);
		}
		// Cleanup
		if(leftRes.hVal != NULL)
			delete leftRes.hVal;
		if(rightRes.hVal != NULL)
			delete rightRes.hVal;
	}

	else if(type == typeDValue) {
		result.type = typeDval;
		result.dVal = strtod(svalue.c_str(), NULL);
	}

	else if(type == typeSValue) {
		result.type = typeHval;
		result.hVal = sValTohVal();
	}

	else if(type == typeUnaryOperation) {
		HParserNodeResult Res = static_cast<hparser*>(leftnode)->evalNode();
		if(Res.type == typeDval) {
			result.type = typeDval;
			result.dVal = Res.dVal;
		} else {
			result.type = typeHval;
			result.hVal = ApplyModifier(Res.hVal, svalue);
		}
	}

	return result;

}

double hparser::operate(double left, double right) {
	double result = 0;
	if(operation == '+') {
		result = left + right;
	} else if(operation == '-') {
		result = left - right;
	} else if(operation == '*') {
		result = left * right;
	} else if(operation == '%') {
		result = left / right;
	} else if(operation == '#') {
		result = left / right;
	} else {
		Warning("hparser::operate", "Wrong operation %s. Return 0.0", &operation);
		return result;
	}
	return result;
}

TH1* hparser::operate(double left, TH1 * right) {
	TH1* result = (TH1*) (right->Clone());
	if(operation == '+') {
		// Do nothing for now
		Warning("hparser::operate", "Do not add constant to a histo");
		return result;
	} else if(operation == '-') {
		Warning("hparser::operate", "Do not add constant to a histo");
		return result;
	} else if(operation == '*') {
		result->Scale(left);
		return result;
	} else if(operation == '%') {
		result->Scale(1. / left);
		return result;
	} else if(operation == '#') {
		result->Scale(1. / left);
		return result;
	} else {
		Warning("hparser::operate", "Wrong operation %s. Do nothing.", &operation);
		return result;
	}
}

TH1* hparser::operate(TH1* left, TH1* right) {

	TH1* result = (TH1*) (left->Clone());
	if(operation == '+') {
		result->Add(right);
		return result;
	} else if(operation == '-') {
		result->Add(right, -1.);
		return result;
	} else if(operation == '*') {
		result->Multiply(right);
		return result;
	} else if(operation == '%') {
		result->Divide(right);
		return result;
	} else if(operation == '#') {
		result->Divide(result, right, 1., 1., "B");
		return result;
	} else {
		Warning("hparser::operate", "Wrong operation %s. Do nothing.", &operation);
		return result;
	}
}

TH1* hparser::sValTohVal() {
	TH1* h = NULL;

	string a = svalue;
	string fPath;
	if(a.find("//") == 0) {
		a = a.substr(2, a.length());

		int i = 0;
		// Check for the root file
		if ((i = a.find(".root/")) > 0) {
			fPath = a.substr(0, i+5);
			a = a.substr(i+6, a.length());

			TFile* file = NULL;
			file = gROOT->GetFile(fPath.c_str());
			if (file != NULL) {
				file->cd();
			} else {
				// attempt to open file ...
				file = new TFile(fPath.c_str());
				if (file->IsZombie()) {
					Error("sValTohVal", "File NOT found");
					return h;
				} else {
					Info("hparser::sValTohVal", "Open file: %s", fPath.c_str());
					file->cd();
				}
			}

			// Check for modifiers
			string hPath = a;
			if(a.find('.')!= string::npos) {
				hPath = a.substr(0, a.find('.'));
				a = a.substr(a.find('.'), a.length());
			} else {
				a = "";
			}

			// Check for bootstrap
			if (hPath.find('^') != string::npos) {
				// Read all histograms with names satisfied templates
				Info("hparser::sValTohVal", "Do boostrap");

				TString reg = hPath;
				reg.ReplaceAll("^", "*");
				TRegexp re(reg, kTRUE);
				TIter next(file->GetListOfKeys());
				TKey *key;
				int counter = 0;
				int nB = 0;
				double *fB;
				double *fB2;
				TH1* tmp2;
				while((key = (TKey*) next())) {
					TString st = key->GetName();
					if(st.Index(re) == kNPOS){
						continue;
					}
					TH1* tmp = (TH1*) gDirectory->Get(st);
					tmp2 = ApplyModifier(tmp, a);

					// Create arrays for the first entry
					if(nB == 0) {
						nB = tmp2->GetNbinsX();
						fB = new double[nB];
						fB2 = new double[nB];

						for(int i = 0; i < nB; i++) {
							fB[i] = 0.;
							fB2[i] = 0.;
						}
					} else {
						if(nB != tmp2->GetNbinsX()) {
							Error("hparser::sValTohVal", "Histograms for bootstrap have different size");
						}
					}

					for(int i = 0; i < nB; i++) {
						fB[i] += tmp2->GetBinContent(i + 1);
						fB2[i] += tmp2->GetBinContent(i + 1) * tmp2->GetBinContent(i + 1);
					}
					tmp->Delete();
					counter++;
				}

				if(nB == 0) {
					Error("hparser::sValTohVal", "No histograms for bootstrap template %s found",
							hPath.c_str());
					return h;
				}

				for(int i = 0; i < nB; i++) {
					fB[i] /= counter;
					fB2[i] /= counter;
					fB2[i] = sqrt(fabs(fB2[i] - fB[i] * fB[i]));
				}

				TH1 *res = journal_parser::fillerr(fB, fB2, tmp2);
				tmp2->Delete();
				res->SetName("RMS");
				return res;
			} else {
				h = (TH1*) gDirectory->Get(hPath.c_str());
				if(h == NULL) {
					Error("hparser::sValTohVal", "Histogram: %s in file: %s is NOT found",
							hPath.c_str(), fPath.c_str());
					return h;
				}
			}
		}
		// Check for the text file
		else if((i = a.find(".txt[")) > 0) {
			fPath = a.substr(0, i);
			double nBins,l,r;
			journal_parser::FillVal(a, ",", ".txt", nBins, l, r);

			//Reading text file
			std::ifstream filestream(fPath.c_str(), std::ios::in);
			if(!filestream.is_open()) {
				Error("hparser::sValTohVal", "Failed to open file: %s", fPath.c_str());
				return h;
			}

			Info("hparser::sValTohVal", "Create hist: %f %f %f", nBins, l, r);
			h = new TH1D("h", "h", nBins, l, r);

			double tmp;
			while(filestream.good()) {
				filestream >> tmp;
				h->Fill(tmp);
			}
			return ApplyModifier(h, "");
		} else {
			Error("hparser::sValTohVal", "No supported file extensions: (root or txt)");
			return h;
		}
	} else {
		Error("hparser::sValTohVal", "File path is not given!!");
		return h;
	}

	return ApplyModifier(h, a);

}

TH1* ApplyModifier(TH1* h, string a) {
	a = a+".";

	double rebin = 1;
	double xmin = 0;
	double xmax = 0;

	bool doProjectionX = false;
	bool doProjectionY = false;

	bool resx = false;
	bool resy = false;
	bool px = false;
	bool py = false;
	bool binSize = false;
	bool UbinSize = false;
	bool doAbs = false;
	bool doUnc = false;
	bool doCalcUnc = false;
	bool doNorm = false;
	double doZero = 0.0;
	bool doUbin = false;

	double prominx = 0;
	double promaxx = 0;
	double prominy = 0;
	double promaxy = 0;
	double prominz = 0;
	double promaxz = 0;

	bool EffectiveNev = false;
	bool UnderOverFlowIn = false;

	// Zoom
	if(a.find(".z")!= string::npos) {
		journal_parser::FillVal(a, ":", ".z", xmin, xmax);
	}

	// X projection
	if(a.find(".px")!= string::npos){
		journal_parser::FillVal(a, ":", ".px", prominy, promaxy, prominz, promaxz);
		doProjectionX = true;
	}

	// Y projection
	if(a.find(".py")!= string::npos){
		journal_parser::FillVal(a, ":", ".py", prominx, promaxx, prominz, promaxz);
		doProjectionY = true;
	}

	getOption(a, ".ppx", px);
	getOption(a, ".ppy", py);
	getOption(a, ".bs", binSize);
	getOption(a, ".ubs", UbinSize);
	getOption(a, ".resx", resx);
	getOption(a, ".resy", resy);
	getOption(a, ".abs", doAbs);
	getOption(a, ".err", doUnc);
	getOption(a, ".calcerr", doCalcUnc);
	getOption(a, ".norm", doNorm);
	getOption(a, ".zero.", doZero);
	getOption(a, ".ubin", doUbin);
	getOption(a, ".neff", EffectiveNev);
	getOption(a, ".uoin", UnderOverFlowIn);
	getOption(a, ".r.", rebin);

	// Implementation of options
	static TH1* fhist;

	if(doProjectionX && (h->GetNbinsZ() > 1 || h->GetNbinsY() > 1)){
		Info("ApplyModifier","Do projection X");

		// Get boundaries
		int ybmin = h->GetYaxis()->FindBin(prominy+0.00001);
		int ybmax = h->GetYaxis()->FindBin(promaxy-0.00001);
		int zbmin = h->GetZaxis()->FindBin(prominz+0.00001);
		int zbmax = h->GetZaxis()->FindBin(promaxz-0.00001);

		cout << ybmin << "  " << ybmax << "  " << zbmin << "  " << zbmax << endl;
		cout << prominy << "  " << promaxy << "  " << prominz << "  " << promaxz  << endl;

		if(prominy!=prominy){
			ybmin = 0;
		}
		if(prominz!=prominz){
			zbmin = 0;
		}
		if(promaxy==promaxy){
			ybmax = h->GetNbinsY();
		}
		if(promaxz==promaxz){
			zbmax = h->GetNbinsZ();
		}

		if(h->GetNbinsZ() == 1) {
			cout << "2D projection " << ybmin << "  " << ybmax;
			TH2 *tmp = (TH2*) h->Clone();
			fhist = tmp->ProjectionX(" ", ybmin, ybmax);
			tmp->Delete();
		} else {
			cout << "3D projection \n";
			TH3 *tmp = (TH3*) h->Clone();
			fhist = tmp->ProjectionX(" ", ybmin, ybmax, zbmin, zbmax);
			tmp->Delete();
		}
		fhist->SetName("");
	} else if(doProjectionY && (h->GetNbinsZ() > 1 || h->GetNbinsY() > 1)){
		Info("ApplyModifier","Do projection Y");

		// Get boundaries
		int xbmin = h->GetXaxis()->FindBin(prominx + 0.00001);
		int xbmax = h->GetXaxis()->FindBin(promaxx - 0.00001);
		int zbmin = h->GetZaxis()->FindBin(prominz + 0.00001);
		int zbmax = h->GetZaxis()->FindBin(promaxz - 0.00001);
		// avoid effective 2D hists
		if(h->GetNbinsZ() == 1){
			TH2 *tmp = (TH2*) h->Clone();
			fhist = tmp->ProjectionY(" ", xbmin, xbmax);
			tmp->Delete();
		} else {
			TH3 *tmp = (TH3*) h->Clone();
			fhist = tmp->ProjectionY(" ", xbmin, xbmax, zbmin, zbmax);
			tmp->Delete();
		}
		fhist->SetName("");
	} else if(resx && h->GetNbinsY() > 1) {
		int nBinX = h->GetNbinsX();
		for(int i = 1; i <= nBinX; i++) {
			TH2 *tmp = (TH2*) h->Clone();
			if(i == 1) {
				fhist = tmp->ProjectionX("X ", i, i);
			}
			TH1D *tmp1d = tmp->ProjectionY(" ", i, i);

			fhist->SetBinContent(i, tmp1d->GetRMS());
			fhist->SetBinError(i, tmp1d->GetRMSError());

			tmp1d->Delete();
			tmp->Delete();
		}
	} else if(px && h->GetNbinsY() > 1) {
		int nBinX = h->GetNbinsX();
		for(int i = 1; i <= nBinX; i++) {
			TH2 *tmp = (TH2*) h->Clone();
			if(i == 1) {
				fhist = tmp->ProjectionX("X ", i, i);
			}
			TH1 *tmp1d = tmp->ProjectionY(" ", i, i);

			fhist->SetBinContent(i, tmp1d->GetMean());
			fhist->SetBinError(i, tmp1d->GetMeanError());

			tmp1d->Delete();
			tmp->Delete();
		}
	}else if(resy && h->GetNbinsY() > 1) {
		int nBinY = h->GetNbinsY();
		for(int i = 1; i <= nBinY; i++) {
			TH2 *tmp = (TH2*) h->Clone();
			if(i == 1) {
				fhist = tmp->ProjectionY("Y ", i, i);
			}
			TH1D *tmp1d = tmp->ProjectionX(" ", i, i);

			fhist->SetBinContent(i, tmp1d->GetRMS());
			fhist->SetBinError(i, tmp1d->GetRMSError());

			tmp1d->Delete();
			tmp->Delete();
		}
	}else if(py && h->GetNbinsY() > 1) {
		int nBinY = h->GetNbinsY();
		for(int i = 1; i <= nBinY; i++) {
			TH2 *tmp = (TH2*) h->Clone();
			if(i == 1) {
				fhist = tmp->ProjectionY("Y ", i, i);
			}
			TH1D *tmp1d = tmp->ProjectionX(" ", i, i);

			fhist->SetBinContent(i, tmp1d->GetMean());
			fhist->SetBinError(i, tmp1d->GetMeanError());

			tmp1d->Delete();
			tmp->Delete();
		}
	} else {
		TH1 *tmp = (TH1*) h->Clone();
		fhist = tmp;
	};

	// Include overflow and underflow bins
	if(UnderOverFlowIn) {
		int nbins = fhist->GetNbinsX();
		double underflow = fhist->GetBinContent(0);
		double overflow = fhist->GetBinContent(nbins + 1);
		Info("ApplyModifier", "underflow = %f, overflow = %f ", underflow, overflow);
		fhist->AddBinContent(1, underflow);
		fhist->AddBinContent(nbins, overflow);
	}

	if(EffectiveNev) {
		for(int i = 1; i <= fhist->GetNbinsX(); i++) {
			double error = fhist->GetBinError(i);
			double value = fhist->GetBinContent(i);
			if(error > 0) {
				fhist->SetBinContent(i, (value / error) * (value / error));
			}
		}
	}

	if(rebin > 1) {
		fhist->Rebin(int(rebin));
	}
	// Do Zoom
	if(xmin < xmax) {
		// Find number of bins, which will be keeped
		int xminBin = fhist->FindBin(xmin) - 1;
		int xmaxBin = fhist->FindBin(xmax);
		double *Bins = new double[xmaxBin-xminBin];
		for(int i=xminBin; i<xmaxBin; i++){
			Bins[i-xminBin] = fhist->GetBinLowEdge(i+1);
		}
		if(xmaxBin > xminBin){
			TH1* fhistClone = (TH1*) fhist->Clone();
			fhist->SetBins(xmaxBin - xminBin-1, Bins);
			for(int i = 1; i <= (xmaxBin - xminBin - 1); i++){
				fhist->SetBinContent(i, fhistClone->GetBinContent(xminBin + i));
				fhist->SetBinError(i, fhistClone->GetBinError(xminBin + i));
			}
			fhistClone->Delete();

		}
		delete Bins;
	}

	// make signed bining absolute
	if(doAbs){
		int nBins = fhist->GetNbinsX();
		if( fhist->GetBinLowEdge( (nBins/2)+1 )==0){
			int xminBin = nBins/2;
			int xmaxBin = nBins;

			double *Bins = new double[xmaxBin-xminBin+1];
			double *Vals = new double[xmaxBin-xminBin];

			for(int i=xminBin; i<=xmaxBin; i++){
				Bins[i-xminBin] = fhist->GetBinLowEdge(i+1);
			}
			for(int i=xminBin+1; i<=xmaxBin; i++){
				Vals[i-(xminBin+1)] = fhist->GetBinContent(i) + fhist->GetBinContent(xmaxBin-i+1);
			}
			fhist->SetBins(xmaxBin-xminBin, Bins);
			for(int i=xminBin+1; i<=xmaxBin; i++){
				fhist->SetBinContent(i-xminBin, Vals[i-(xminBin+1)]);
			}
			delete Bins;
			delete Vals;
		}
	}


	if (doUbin) {
		int nBins = fhist->GetNbinsX();
		double *Vals = new double[nBins];

		for (int i = 1; i <= nBins; i++) {
			Vals[i-1] = fhist->GetBinContent(i);
		}
		fhist->SetBins(nBins, 0, nBins);
		for (int i = 1; i <= nBins; i++) {
			fhist->SetBinContent(i, Vals[i-1]);
		}
		delete Vals;
	}

	// move first bin from 0 to 0.1 for nice plotting in log scale
	if(doZero > 0) {
		int nBins = fhist->GetNbinsX();
		if(fhist->GetBinLowEdge(1) == 0) {
			if(fhist->GetBinLowEdge(2) < doZero) {
				Warning("ApplyModifier", "Option zero failed, first bin ends at %f < %f",
						fhist->GetBinLowEdge(2), doZero);
			} else {
				int xmaxBin = nBins;

				double *Bins = new double[nBins + 1];
				double *Vals = new double[nBins];
				double *Errs = new double[nBins];
				Bins[0] = doZero;

				for(int i = 1; i <= nBins; i++) {
					Bins[i] = fhist->GetBinLowEdge(i + 1);
					Vals[i - 1] = fhist->GetBinContent(i);
					Errs[i - 1] = fhist->GetBinError(i);
				}

				fhist->SetBins(nBins, Bins);
				for(int i = 1; i <= xmaxBin; i++) {
					fhist->SetBinContent(i, Vals[i - 1]);
					fhist->SetBinError(i, Errs[i - 1]);
				}
				delete Bins;
				delete Vals;
				delete Errs;
			}
		} else {
			Warning("ApplyModifier", "Option zero works only for histogram starts at 0");
		}
	}

	// Get histogram X bins and divide by bin sizes
	if(binSize || doNorm) {
		Int_t nB = fhist->GetNbinsX();
		for(Int_t i = 0; i < nB; ++i) {
			Double_t bS = fhist->GetBinWidth(i + 1);
			Double_t vA = fhist->GetBinContent(i + 1);
			Double_t eR = fhist->GetBinError(i + 1);
			fhist->SetBinContent(i + 1, vA / bS);
			fhist->SetBinError(i + 1, eR / bS);
		}
		// Get histogram X bins and divide by bin sizes
		if(doNorm) {
			fhist->Scale(1. / fhist->Integral("width"));
		}
	}
	if(UbinSize ) {
		Int_t nB = fhist->GetNbinsX();
		for(Int_t i = 0; i < nB; ++i) {
			Double_t bS = fhist->GetBinWidth(i + 1);
			Double_t vA = fhist->GetBinContent(i + 1);
			Double_t eR = fhist->GetBinError(i + 1);
			fhist->SetBinContent(i + 1, vA * bS);
			fhist->SetBinError(i + 1, eR * bS);
		}
	}

	// Create histogram with uncertainties instead of values
	if(doUnc){
		//for(Int_t i = 1; i <= fhist->GetNbinsX(); ++i) {
		for(Int_t i = 0; i <= (fhist->GetNbinsX() + 2)*(fhist->GetNbinsY() + 2); ++i) {
			if(fhist->GetBinContent(i) != 0){
				fhist -> SetBinContent(i, fhist->GetBinError(i)/ fhist->GetBinContent(i));
				fhist -> SetBinError(i, 0.00001);
			}
		}
	}

	if(doCalcUnc){
		for(Int_t i = 0; i <= (fhist->GetNbinsX() + 2)*(fhist->GetNbinsY() + 2); ++i) {
			if(fhist->GetBinContent(i) != 0){
				fhist -> SetBinContent(i, sqrt(fhist->GetBinContent(i)) );
				fhist -> SetBinError(i, 0.00001);
			}
		}
	}

	return fhist;
}
