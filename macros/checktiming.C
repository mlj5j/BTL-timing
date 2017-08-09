



#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#include "TAxis.h"
#include "TChain.h"

void checktiming()
{
    //--------------------------Setting Options--------------------
    double timerange = 1e6;  //This sets the max acceptable time difference in ns
    int N = -1;  //Sets number of TOFPET events to loop over.  Set -1 for all of them.

    //-----------Initialize TGraph------------------
    TGraph *g1 = new TGraph();

    //-----------Declaring variables----------------------------------------------
    bool IsFirstTpet = true;

    long long tpetshift = 0;
    double scaledtpettime = 0;
    int evtnum = 0;

    double scaledninotime;
    unsigned long long ninoshift = 0;
    int board;
    int ninocounter = 0;
    
    //-------------------From root file------------------------------------------
    long long tpettime;
    float step1, step2;
    unsigned short chID;
    
    unsigned long long ninotime[2];
    unsigned int timeboard[2];


    //-----------------Opening the files---------------------------------------
    TFile *f_tpet = new TFile("TOFPET/R_2017_06_30__13_54_21_c_trigger.root");
    if (f_tpet == 0)
    {
	cout << "Error: Cannot find TOFPET file!" << endl;
	return;
    }
    else 
	cout << "TOFPET file loaded!" << endl;

    //-----------------------Making TChain of H4DAQ Runs----------------------------
    TChain *ninotree = new TChain("H4tree");
    ninotree->Add("TOFPET_data/Run_8076.root");
    ninotree->Add("TOFPET_data/Run_8077.root");
    ninotree->Add("TOFPET_data/Run_8078.root");


    //----------------------Getting branches and setting branch addresses------------------
    TTree* tpettree = (TTree*) f_tpet->Get("data");

    tpettree->SetBranchAddress("step1",&step1);
    tpettree->SetBranchAddress("step2",&step2);
    tpettree->SetBranchAddress("time2",&tpettime);
    tpettree->SetBranchAddress("channelID2",&chID);

    ninotree->SetBranchAddress("evtTime",&ninotime);
    ninotree->SetBranchAddress("evtTimeBoard",timeboard);

    //------Looping through TOFPET events and grabbing NINO events in coincidence-----
    if (N == -1)
	N = tpettree->GetEntries();
    for (int i=0; i < N; i++)
    {
	tpettree->GetEntry(i);

	if (step1 == 4)
	{
	    if (chID == 896)
	    {
		if (IsFirstTpet)
		    tpetshift = tpettime;
		IsFirstTpet = false;
		scaledtpettime = ((double)tpettime - (double)tpetshift)/1e3;
//		cout << "Currently at event " << i << endl;

		for (int j=ninocounter; j < ninocounter + 1000 /*ninotree->GetEntries()*/; j++)
		{
		    ninotree->GetEntry(j);
		    if (timeboard[0] == 16842753)
			board = 0;
		    else
			board = 1;
		    if (j == 0)
			ninoshift = ninotime[board];
		    scaledninotime = ((double)ninotime[board] - (double)ninoshift)*1e3;
		    if (abs(scaledninotime - scaledtpettime) < timerange)
		    {
			ninocounter = j;
//			cout << "found one at i=  " << i << endl;

			g1->SetPoint(evtnum,scaledninotime,scaledtpettime);
						
		    }
		}
		evtnum++;
	    }
	}
    }
    TCanvas *c1 = new TCanvas("c1","c1",500,500);
    g1->SetTitle("H4DAQ time vs TOFPET trigger time; H4DAQ time; TOFPET time");
    g1->Draw("Ap");

}




    


