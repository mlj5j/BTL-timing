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

void synchplot()
{
    
    int tpet_events, nino_events, num_events;
    long long tpetshift = 0;
    unsigned long long ninoshift =0;
    int nino_counter = 0;
    bool valid = true;
    double scaledninotime;

    //Declaring variables we get from the root files
    long long tpettime;
    float step1, step2;
    unsigned short chID; //For ASIC 1 the channel is 768.  ASIC is channel 896.
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

    //Looping through the events in the TOFPET root file.  
    for (int i=0; i<tpet_events; i++)
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
		    ninotree->GetEntry(nino_counter);  //Getting event from NINO corresponding to ASIC 2 trigger
		    if (nino_counter == 0)  //Here we synchronize the first events on each board to be at time = 0.
		    {
			tpetshift = tpettime;  
			ninoshift = ninotime[1];
		    }

		    tpettime -= tpetshift;  //Applying the shift needed to zero the first event time
		    ninotime[1] -= ninoshift;  //Applying the shift needed to zero the first event time
		    scaledtpettime = (double)tpettime/1e3;  //Converting the topfet time from ps to ns
		    scaledninotime = (double)ninotime[1]*5;  //Converting from UNIX time to ns
		    g1->SetPoint(nino_counter,scaledninotime,scaledtpettime);


		    //----------------For debugging----------------------------- 
		    /*
		    if (nino_counter < 610)
			cout << scaledninotime << endl;
//		    cout << nino_events << endl;
//		    ------------------------------------------------------------
		    */

		    nino_counter++;
		    if (nino_counter == nino_events) //This checks that we don't try to read events that aren't there.
		    {
			valid = false;
			cout << nino_counter << endl;
		    }
		}
//	    }
	}


    }
    TCanvas *c1 = new TCanvas("c1","c1",500,500);
    g1->Draw("Ap");
}



