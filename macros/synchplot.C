#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"

void synchplot()
{
    
    int tpet_events, nino_events, num_events;
    int tpetshift = 0;
    int ninoshift =0;
    int nino_counter = 0;
    bool valid = true;
    double scaledninotime;

    long long tpettime;
    float step1, step2;
    unsigned short chID;
    double scaledtpettime;
    unsigned long long ninotime[2];
    unsigned int timeboard[2];
    

    //initializing the TGraph we'll use to plot the two times
    TGraph *g1 = new TGraph();

    TFile *f_tpet = new TFile("TOFPET/R_2017_06_30__13_54_21_c_trigger.root"); //loading the tofpet root file
    if (f_tpet == 0)
    {
	cout << "Error:  Cannot find TOFPET file!" << endl;
	return;
    }
    else
	cout << "File found and loaded!" << endl;

    TFile *f_nino = new TFile("NINO/8076/1.root"); //loading the nino root file
    if (f_nino == 0)
    {
	cout << "Error:  Cannot find NINO file!" << endl;
	return;
    }
    else
	cout << "File found and loaded!" << endl;

    TTree* tpettree = (TTree*) f_tpet->Get("data");
    TTree* ninotree = (TTree*) f_nino->Get("H4tree");

    tpettree->SetBranchAddress("time2",&tpettime);
    tpettree->SetBranchAddress("step1",&step1);
    tpettree->SetBranchAddress("step2",&step2);
    tpettree->SetBranchAddress("channelID2",&chID);

    ninotree->SetBranchAddress("evtTime",&ninotime);
    ninotree->SetBranchAddress("evtTimeBoard",&timeboard);

    //Need to determine the number of events in each file so we know how many points we'll be plotting
    //I think there is a bug here because the tofpet has an event for each asic (I think?) so it might have twice
    //the number of events than triggers sent to nino.  Need to check this.

    tpet_events = tpettree->GetEntries();
    nino_events = ninotree->GetEntries();
    if (tpet_events > nino_events)
	num_events = nino_events;
    else
	num_events = tpet_events;

    for (int i=0; i<tpet_events; i++)
    {
	if (valid == false)
	    break;
	tpettree->GetEntry(i);

	if (step1 == 4)
	{
//	    if (step2 == 32)
//	    {
		if (chID == 896)
		{
		    ninotree->GetEntry(nino_counter);
		    if (nino_counter == 0)
		    {
			tpetshift = tpettime;
			ninoshift = ninotime[1];
		    }

		    tpettime -= tpetshift;
		    ninotime[1] -= ninoshift;
		    scaledtpettime = (double)tpettime/1e3;
		    scaledninotime = (double)ninotime[1]/5;
		    g1->SetPoint(nino_counter,scaledninotime,scaledtpettime);
		    nino_counter++;
//		    cout << nino_counter << endl;
		    if (nino_counter > nino_events)
			valid == false;
		}
//	    }
	}


    }
    TCanvas *c1 = new TCanvas("c1","c1",500,500);
    g1->Draw("Ap");
}



