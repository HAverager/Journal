{
	gSystem->AddIncludePath(" -I$HOME/journal ");
	gROOT->LoadMacro("$HOME/journal/AtlasStyle.C+");
	gROOT->LoadMacro("$HOME/journal/AtlasUtils.C+");
	gROOT->LoadMacro("$HOME/journal/parser.C+");
	gROOT->LoadMacro("$HOME/journal/hparser.C+");
	gROOT->LoadMacro("$HOME/journal/journal.C+");
	journal();
}
