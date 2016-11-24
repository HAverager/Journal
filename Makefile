CXX=g++
CXXFLAGS = -fPIC
LD_FLAGS = -shared

INCLUDEFLAGS = -I./include -I../include

ROOTLIBS = `root-config --libs` -lFoam -lMinuit -lTMVA   
ROOTCFLAGS = $(shell root-config --cflags)
ROOTGLIBS = $(shell root-config --glibs)

OBJS = hparser.o  journal.o  parser.o  utils.o
TARGET_LIB = journal.so
TARGET_DICT = $(TARGET_LIB:.so=dict.cxx)

all: $(TARGET_LIB) $(TARGET_DICT)

$(TARGET_LIB):
	$(CXX) src/hparser.C  $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDEFLAGS) -c $< 
	$(CXX) src/journal.C $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDEFLAGS) -c $<	
	$(CXX) src/parser.C   $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDEFLAGS) -c $<
	$(CXX) src/utils.C $(CXXFLAGS) $(ROOTCFLAGS) $(INCLUDEFLAGS) -c $<
	$(CXX) -o lib$(TARGET_LIB) $(OBJS) $(ROOTLIBS) $(LD_FLAGS)
	rm -f $(OBJS)

$(TARGET_DICT):
	@rootcint lib$(TARGET_DICT) -c $(INCLUDEFLAGS) $(OBJS:.o=.h)


clean:
	rm -f $(OBJS)
	rm -f lib$(TARGET_LIB) lib$(TARGET_DICT) lib$(TARGET_DICT:.cxx=_rdict.pcm)

