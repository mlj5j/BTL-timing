#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include "TCanvas.h"
#include "TH1.h"
#include "TGraph.h"

void testing()
{

    TGraph *g1 = new TGraph();
    TH1F *h1 = new TH1F("h1", "h1", 300, -5000, 15000);
    TFile *f = TFile::Open("TOFPET/R_2017_06_30__13_54_21_c_trigger.root");
    if (f == 0)
    {
	cout << "Error: cannot find file!" << endl;
	return;
    }
    else 
	cout << "File found and loaded!" << endl;

    TTree* tree = (TTree*) f->Get("data");
    long long time1, time2;
    float step1, step2;
    tree->SetBranchAddress("time1",&time1);
    tree->SetBranchAddress("time2",&time2);
    tree->SetBranchAddress("step1",&step1);
    tree->SetBranchAddress("step2",&step2);

    long long shift = 0;
//    long long time;

    long long dtime = 0;
    double scaledtime1;
    double scaledtime2;

   int w = 0; 
    
    unsigned short channelID1;
    unsigned short channelID2;
//    tree->SetBranchAddress("time",&time);
    tree->SetBranchAddress("channelID1",&channelID1);
    tree->SetBranchAddress("channelID2",&channelID2);

    for (int i=0, N = tree->GetEntries(); i < N; ++i)
    {
	tree->GetEntry(i);

	if (step1==4)
	{
	    if (step2 == 32)
	    {
		if (channelID2==896)
		{
		    if (w==0)
			shift = -time2;
		    time2 += shift;
		    scaledtime2 = (double)time2/1000;
		    if (w<100)
			cout << scaledtime2 << endl;
		    w++;
		}
	    }
	}




/*
	if (channelID2==896)
	{
	    if (w==0)
		shift = -time2;
	    time2 += shift;
	    scaledtime2 = (double)time2/5e3;
	    if (w<10)
		cout << scaledtime2 << endl;
	    w++;
	}
	if (i==0)
	{
	    shift = time1;
	}
	scaledtime1 = (double)time1/5000 ;
	scaledtime2 = (double)time2/5000 ;
	time1 = time1 - shift;
	time2 = time2 - shift;
	dtime = time2 - time1;
*/
/*	
	if (channelID1 == 896 || channelID1 == 768)
	{
	    g1->SetPoint(n,(double)time1,(double)time2);
	    if (n<10)
	    {
		cout << time1 << endl;
	    }
	    n++;
	}
*/
/*
	if (i<10)
	{
	    cout << time1 << endl;
	}
	g1->SetPoint(i,(double)time2,(double)time1);
	h1->Fill(dtime);
*/
/*
	if (i < 10)
	{
	    cout << scaledtime2 << endl;
	}  
*/
    }
/*
    TCanvas *c1 = new TCanvas("c1", "c1", 1200, 500);
    c1->Divide(2,0);
    c1->cd(1);
    g1->Draw("Ap");
    c1->cd(2);
    h1->Draw();
*/

}
