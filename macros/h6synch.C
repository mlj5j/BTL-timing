#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#include "TAxis.h"
#include "TChain.h" 

using namespace std;

//---------set debug to 1 during testing-----------------------------------------------------------
bool debug = true;

void h6synch()
{

//-----------------Declaring variables we get from the ROOT files-----------------------------------

    long long tpettime;
    float step1, step2;
    unsigned short chID;
    unsigned long long h4time[2];
    unsigned int timeboard[2];
    unsigned int spill;
//--------------------------------------------------------------------------------------------------



//-----------------Declaring other variables--------------------------------------------------------

    int tpet_max = 80000;
    double scaledtpettime;
    double tpetshift;
    bool first_tpet = true;

    double scaledh4time;
    double h4shift;
    int h4count = 0;
    int board;

    double timediff;
    int plotcounter = 0;

    int chArray[16] = {
	7 ,9 ,14,19,
	20,23,26,31,
	32,33,34,40,
	41,43,56,63 };
//    int k = 0;




//--------------------------------------------------------------------------------------------------


//-----------------Initializing plots---------------------------------------------------------------

    TGraph *timegraph = new TGraph();
//    TGraph *tdiffgraph = new TGraph();
    TGraph *spillgraph = new TGraph();
    TCanvas *c1 = new TCanvas("c1","c1",2000,2000);
    c1->Divide(4,4);


//--------------------------------------------------------------------------------------------------


//-----------------Loading ROOT files and trees-----------------------------------------------------

    TChain *tpettree = new TChain("data");
    tpettree->Add("H6_TOFPET/9457.root");

    TChain *h4tree = new TChain("H4tree");
    h4tree->Add("H6_NINO/9457.root");

//---------------------------------------------------------------------------------------------------


//-----------------Setting up branches---------------------------------------------------------------



    tpettree->SetBranchAddress("step1",&step1);
    tpettree->SetBranchAddress("step2",&step2);
    tpettree->SetBranchAddress("time",&tpettime);
    tpettree->SetBranchAddress("channelID",&chID);

    h4tree->SetBranchAddress("evtTime",&h4time);
    h4tree->SetBranchAddress("evtTimeBoard",&timeboard);
    h4tree->SetBranchAddress("spillNumber",&spill);

    if (debug == false)
    {
	tpet_max = tpettree->GetEntries();
    }
    int h4_events = h4tree->GetEntries();

//---------------------------------------------------------------------------------------------------
//

    for (int k=0; k<16; k++)
    {
	TGraph *tdiffgraph = new TGraph();
	first_tpet = true;
	h4count = 0;
	cout << "#########################################" << endl;
	cout << "Now Processing Channel " << chArray[k] << endl;
//	int j = 0;
	

    for (int i=0; i<tpet_max; i++)
    {
	if (i%10000 == 0)
	    cout << "Now processing event number " << i << endl;

	tpettree->GetEntry(i);
	//cout << "channel id is " << chID << endl;

//	if (step1 == 4)
//	{
	    if (chID == chArray[k])
	    {
		if (first_tpet == true)
		{
		    tpetshift = tpettime;
		}

		tpettime -= tpetshift;
		scaledtpettime = (double)tpettime;

		first_tpet = false;

		for (int j = h4count; j < h4count + 10; j++)
		{
		    h4tree->GetEntry(j);

		    if (timeboard[0] == 16908289)
			board = 0;
		    else 
			board = 1;

		    if (h4count == 0)
		    {
			h4shift = h4time[board];
		    }

		    h4time[board] -= h4shift;
		    scaledh4time = (double)h4time[board]*1e6;
		    timediff = abs(scaledh4time - scaledtpettime);
		    if (timediff < 100e6)
		    {
			timegraph->SetPoint(plotcounter, scaledh4time, scaledtpettime);
			tdiffgraph->SetPoint(plotcounter, scaledtpettime, timediff);
			plotcounter++;
//			h4count++;    //test
//			j++;    //test
//			cout << "Currently at h4counter " << h4count << endl;
			h4count = j + 1;
			break;

		    }

//---------------------------Debugging stuff-------------------------------------------		    
//		    else
//		    {
//			cout << "TOFPET event " << i << " and H4 event " << j << " are out of synch" << endl;
//			cout << "TOFPET time is " << scaledtpettime << " and H4 time is " << scaledh4time << endl;
			
//		    }
//		    if (spill==2)
//			cout << "Currently in spill 2" << endl;
//-------------------------------------------------------------------------------------			
		}
		spillgraph->SetPoint(i,scaledtpettime, (double)spill);
	    }
//	}
    }
//---------------Creating canvas and drawing plots------------------------------    
/*    
    TCanvas *c1 = new TCanvas("c1","c1",1000,300);
    c1->Divide(3,1);
    c1->cd(1);
    timegraph->Draw("A*");
    c1->cd(2);
    tdiffgraph->Draw("A*");
    c1->cd(3);
    spillgraph->Draw("A*");
//------------------------------------------------------------------------------
*/


//-------------------Making plot to check each Channel ID-----------------------    
    c1->cd(k+1);
    tdiffgraph->Draw("A*");
//------------------------------------------------------------------------------

    }
    


}
