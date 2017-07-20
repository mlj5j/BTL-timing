#include "TFile.h"
#include "TTree.h"
#include <iostream>

struct timetree
{
    unsigned long long ninotime;
    unsigned long long tofpettime;
};

void ninoflat()
{
//    TFile *fnew = new TFile("flatnino.root");
    

    TFile *f = TFile::Open("NINO/8076/1.root");
    if (f==0)
    {
	cout << "Error: cannot find file!" << endl;
	return;
    }
    else
	cout << "File found and loaded!" << endl;

    TTree* tree = (TTree*) f->Get("H4tree");
    unsigned long long evtTime[2];
    unsigned long long shift = 0;
    unsigned int evtTimeBoard[2];
    tree->SetBranchAddress("evtTime",&evtTime);
    tree->SetBranchAddress("evtTimeBoard",&evtTimeBoard);
    tree->AddFriend("data","TOFPET/R_2017_06_30__13_54_21_c_trigger.root");



    for (int i=0, N=tree->GetEntries(); i<N; i++)
    {
	tree->GetEntry(i);
	
	if (i==0)
	    shift = evtTime[1];
	evtTime[1] -= shift;
	if (i<10)
	    cout << evtTime[1] << endl;
//	cout << evtTimeBoard[1] << endl;
	
    }
}

