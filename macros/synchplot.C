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
    
    int tpet_events, nino_events;
    long long tpetshift = 0;
    unsigned long long ninoshift =0;
    int nino_counter = 0;
    bool valid = true;
    double scaledninotime;
    int board;
    int nDigiSamples = 18430;
    double time_dif;
    double prev_tpettime = 0;
    double prev_ninotime = 0;
    double tpet_cycles;
    double nino_cycles;


    //Declaring variables we get from the root files
    long long tpettime;
    float step1, step2;
    unsigned short chID; //For ASIC 1 the channel is 768.  ASIC 2 is channel 896.
    double scaledtpettime;
    unsigned long long ninotime[2];
    unsigned int timeboard[2];
    unsigned int digiChannel[nDigiSamples];
    unsigned int digiGroup[nDigiSamples];
    float digiSampleValue[nDigiSamples];
    float x;
    float y;
    float energy;
    

    //initializing the TGraph we'll use to plot the two times
    TGraph *g1 = new TGraph();
    TGraph *g2 = new TGraph();
//    TGraph *g3 = new TGraph();

//########################Loading the TOFPET file into ROOT############################################

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

//###############################################################################################



//######Making TChain of the H4DAQ files once the spills have been combined for each run##########

    TChain *ninotree = new TChain("H4tree");
    ninotree->Add("TOFPET_data/Run_8076.root");
    ninotree->Add("TOFPET_data/Run_8077.root");
    ninotree->Add("TOFPET_data/Run_8078.root");

//#############################################################################################

    TTree* tpettree = (TTree*) f_tpet->Get("data");

    tpettree->SetBranchAddress("step1",&step1);
    tpettree->SetBranchAddress("step2",&step2);
    tpettree->SetBranchAddress("x",&x);
    tpettree->SetBranchAddress("y",&y);
    tpettree->SetBranchAddress("energy",&energy);

//    tpettree->SetBranchAddress("time2",&tpettime);
//    tpettree->SetBranchAddress("channelID2",&chID);

    tpettree->SetBranchAddress("time",&tpettime);
    tpettree->SetBranchAddress("channelID",&chID);

    ninotree->SetBranchAddress("evtTime",&ninotime);
    ninotree->SetBranchAddress("evtTimeBoard",&timeboard);
    ninotree->SetBranchAddress("digiChannel",&digiChannel);
    ninotree->SetBranchAddress("digiGroup",&digiGroup);
    ninotree->SetBranchAddress("digiSampleValue",&digiSampleValue);


    tpet_events = tpettree->GetEntries();
    nino_events = ninotree->GetEntries();
    cout << "tpet_events = " << tpet_events << endl;

    //Looping through the events in the TOFPET root file.  
    for (int i=0; i<tpet_events; i++)
    {
	if (valid == false) //This breaks the loop if we run out of NINO events
	    break;
	tpettree->GetEntry(i);
	if (i%10000 == 0)
	    cout << "At event " << i << endl;

	if (step1 == 4)  //Choosing only events from TOFPET having 4V threshold
	{
//	    cout << "step1 == 4" << endl;
//	    if (step2 == 32)
//	    {
		if (chID == 896)  //Choosing only triggers coming from ASIC 2
		{
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
		    time_dif = abs(scaledtpettime - scaledninotime);

//#######################################Time cuts###############################################

//This if statement only accepts events within a certain time range, but it's still a work in progress so it's disabled for now
	//	    if (abs(time_dif)<1e5)
	//	    {
			g1->SetPoint(nino_counter,scaledninotime,scaledtpettime);
			g2->SetPoint(nino_counter,scaledtpettime,time_dif);
	//	    }
	
	//	    else 
	//	    {
	//		nino_counter++;
	//		i -= 1;
	//		continue;
	//	    }
		    
		    //g2->SetPoint(nino_counter,(double)nino_counter,time_dif);
//		    g3->SetPoint(nino_counter,x,energy);
//		    if (scaledtpettime > 5e13)
//			break; 
//####################################################################################################
/*
##################Reading Digi Channels################################################################
		    for (int k=0; k<nDigiSamples; k++)
		    {
			if (digiGroup[k] == 0)
			{
			    if (digiChannel[k] == 7)
			    {
				g2->SetPoint(nino_counter, scaledtpettime, digiSampleValue[k]);
				
			    }
			}
		    }
#######################################################################################################
*/
		    nino_counter++;
		    if (nino_counter+1 == nino_events) //This checks that we don't try to read events that aren't there.
		    {
			valid = false;
			cout << "Total number of events read = " << nino_counter << endl;
		    }
		    if (i+1 == tpet_events)
			cout << "Ran out of TOFPET events.  Number of events = " << nino_counter << endl;
		    nino_cycles = (scaledninotime - prev_ninotime)/5e3;
		    tpet_cycles = (scaledtpettime - prev_tpettime)/5e3;
		    prev_tpettime = scaledtpettime;
		    prev_ninotime = scaledninotime;

		}
//	    }
	}
	if (i+1 == tpet_events)
	    cout << nino_counter << endl;


    }
    TCanvas *c1 = new TCanvas("c1","c1",500,500);
    g1->SetTitle("H4DAQ time vs TOFPET trigger time; H4DAQ time; TOFPET time");
    g1->Draw("Ap");
//    c1->SaveAs("H4DAQvsTOFPETtimes_allRuns_no_trigger_zoom_beginning.png");
    TCanvas *c2 = new TCanvas("c2","c2",500,500);
    g2->SetTitle("TOFPET trigger time vs Time difference; TOFPET time (ps); Time difference(ps)");
    g2->Draw("A*");
    c2->SaveAs("TimeDifferenceASIC1.png");
//    TCanvas *c3 = new TCanvas("c3","c3",500,500);
//    g3->Draw("Ap");
    

}



