/*
----------------------------------Written by Matt Joyce-------------------------------------
|  This macro looks to see if the time the trigger from the TOFPET2 board (ASIC 2) matches |
|  with the time the trigger is received by the NINO boards.  This is only checking        |
|  the first spill for the first run of the NINOs coinciding with the beginning of         |
|  the TOFPET run.                                                                         |
--------------------------------------------------------------------------------------------
*/

#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#include "TAxis.h"
#include "TChain.h"

void synchplot()
{

//    int k = 1;  //counter for debugging
//    int l = 1;  //counter for debugging
    
    int tpet_events, nino_events;
    long long tpetshift = 0;
    unsigned long long ninoshift =0;
    int nino_counter = 0;
    bool valid = true;
    double scaledninotime;
    int board;

    //Declaring variables we get from the root files
    long long tpettime;
    float step1, step2;
    unsigned short chID; //For ASIC 1 the channel is 768.  ASIC 2 is channel 896.
    double scaledtpettime;
    unsigned long long ninotime[2];
    unsigned int timeboard[2];
    

    //initializing the TGraph we'll use to plot the two times
    TGraph *g1 = new TGraph();
//    TGraph *g2 = new TGraph();

//    TFile *f_tpet = new TFile("TOFPET/R_2017_06_30__13_54_21_c.root");
    TFile *f_tpet = new TFile("TOFPET/R_2017_06_30__13_54_21_s.root");
//    TFile *f_tpet = new TFile("TOFPET/R_2017_06_30__13_54_21_c_trigger.root"); //loading the tofpet root file
    if (f_tpet == 0)
    {
	cout << "Error:  Cannot find TOFPET file!" << endl;
	return;
    }
    else
	cout << "File found and loaded!" << endl;


/*######################Commented to test TChain##########################################################
    TFile *f_nino = new TFile("/afs/cern.ch/user/j/jihan/work/public/nino_8076.root");
//  TFile *f_nino = new TFile("/afs/cern.ch/work/p/pbarria/public/BTL_Utility/NINO/8076/NINO_8076.root");
//    TFile *f_nino = new TFile("NINO/8076/1.root"); //loading the h4daq root file
    if (f_nino == 0)
    {
	cout << "Error:  Cannot find NINO file!" << endl;
	return;
    }
    else
	cout << "File found and loaded!" << endl;
*///######################################################################################################

    TChain *ninotree = new TChain("H4tree");
    ninotree->Add("TOFPET_data/Run_8076.root");
    ninotree->Add("TOFPET_data/Run_8077.root");
    ninotree->Add("TOFPET_data/Run_8078.root");


    TTree* tpettree = (TTree*) f_tpet->Get("data");

    tpettree->SetBranchAddress("step1",&step1);
    tpettree->SetBranchAddress("step2",&step2);

//    tpettree->SetBranchAddress("time2",&tpettime);
//    tpettree->SetBranchAddress("channelID2",&chID);

    tpettree->SetBranchAddress("time",&tpettime);
    tpettree->SetBranchAddress("channelID",&chID);

    ninotree->SetBranchAddress("evtTime",&ninotime);
    ninotree->SetBranchAddress("evtTimeBoard",&timeboard);


    tpet_events = tpettree->GetEntries();
    nino_events = ninotree->GetEntries();
    cout << "tpet_events = " << tpet_events << endl;


    int counter =0;
    //Looping through the events in the TOFPET root file.  
    for (int i=0; i<1e5 /*tpet_events*/; i++)
    {
	if (valid == false) //This breaks the loop if we run out of NINO events
	    break;
	tpettree->GetEntry(i);

	if (step1 == 4)  //Choosing only events from TOFPET having 4V threshold
	{
//	    if (step2 == 32)
//	    {
		if (chID == 896)  //Choosing only triggers coming from ASIC 2
		{
//		    g2->SetPoint(i,(double)i,(double)step1);
//		    counter++;
//		    cout << "event " << counter << endl;
		    ninotree->GetEntry(nino_counter);  //Getting event from NINO corresponding to ASIC 2 trigger
		    
		    if (timeboard[0] == 16842753)
			board = 0;
		    else 
			board = 1;

		    if (nino_counter == 0)  //Here we synchronize the first events on each board to be at time = 0.
		    {
			tpetshift = tpettime;  
			ninoshift = ninotime[board];
		    }

		    tpettime -= tpetshift;  //Applying the shift needed to zero the first event time
		    ninotime[board] -= ninoshift;  //Applying the shift needed to zero the first event time
		    scaledtpettime = (double)tpettime;  //Converting the topfet time from ps to ns
		    scaledninotime = (double)ninotime[board]*1e6;  //Converting from UNIX time (ms) to ps
//		    cout << "nino_counter = " << nino_counter << endl;
		    g1->SetPoint(nino_counter,scaledninotime,scaledtpettime); 
/*
//----------------------For debugging---------------------------------------------
		    if (scaledtpettime > 4e10 && scaledtpettime < 8e10)
		    {
			cout << "time step 1 = " << scaledtpettime << endl;
//			cout << "number of step events = " << k << endl;
			k++;
		    }
		    if (scaledtpettime > 8e10 && scaledtpettime < 1e11)
		    {
			cout << "time step 2 = " << scaledtpettime << endl;
//			cout << "number of second step events = " << l << endl;
			l++;
		    }
//-------------------------------------------------------------------------------
*/
		    //----------------For debugging----------------------------- 
		    /*
		    if (nino_counter < 610)
			cout << scaledninotime << endl;
//		    cout << nino_events << endl;
//		    ------------------------------------------------------------
		    */

		    nino_counter++;
		    if (nino_counter+1 == nino_events) //This checks that we don't try to read events that aren't there.
		    {
			valid = false;
			cout << "Total number of events read = " << nino_counter << endl;
		    }
		    if (i+1 == tpet_events)
			cout << "Ran out of TOFPET events.  Number of events = " << nino_counter << endl;

		}
//	    }
	}
	if (i+1 == tpet_events)
	    cout << nino_counter << endl;


    }
    TCanvas *c1 = new TCanvas("c1","c1",500,500);
    g1->SetTitle("H4DAQ time vs TOFPET trigger time; H4DAQ time; TOFPET time");
    g1->SetMarkerStyle(7);
    g1->Draw("Ap");
//    c1->SaveAs("H4DAQvsTOFPETtimes_allRuns_no_trigger_full_test.png");
//    TCanvas *c2 = new TCanvas("c2","c2",500,500);
//    g2->Draw("A*");
}



