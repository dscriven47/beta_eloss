#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <TLegend.h>

void beta_eloss() {
	gROOT->SetBatch(true);
    std::ifstream inputFile("estarCeBr3.txt");

    if (!inputFile) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::string sdensity;
    double density;
    std:string material;
    std::vector<double> energy;
    std::vector<double> stpow;
    std::vector<double> range;

    // Skip the first 9 lines
    std::string line;
    std::cout<<"PRINTING HEADER INFORMATION"<<std::endl;
    for (int i = 0; i < 8; ++i) {
      if (std::getline(inputFile, line)) {
        std::cout << i << "   ";
        std::cout << line << "   ";
        if(i==2 && line.find("Density")<100 && line.find(" g/cm3")<100)
        {
          std::cout<<std::endl;
          sdensity=line.substr(line.find("Density")+8,line.find(" g/cm3")-7);
          density=std::stod(line.substr(line.find("Density")+8,line.find(" g/cm3")-13));
          material=line.substr(0,line.find(" (Density"));
        }
        cout<< " " << material;
        
        std::cout<<std::endl;
      } else {
        std::cerr << "File has fewer than 9 lines" << std::endl;
        return 1;
      }
    }
    std::cout << Form("GOT DENSITY OF MATERIAL (%s) ",material.c_str()) << sdensity << std::endl;


    double value1, value2, value3;
    while (inputFile >> value1 >> value2 >> value3) 
    {
        energy.push_back(value1);
        stpow.push_back(value2*density);
        range.push_back(value3/density);
    }

    inputFile.close();


    // Output the contents of the arrays
    /*std::cout << "Column 1: ";
    for (double val : energy) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "Column 2: ";
    for (double val : stpow) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    */
	


    // make the first plot
    int n = energy.size();
    TCanvas *c1 = new TCanvas("stpow","stpow",700,700);
    TGraph *gstpow = new TGraph(n,&energy[0],&stpow[0]);
    TSpline5 *sstpow = new TSpline5("gstpow",gstpow); 
    sstpow->SetLineColor(kRed);
    gstpow->GetXaxis()->SetRangeUser(1.0e-2,5e1);
    gstpow->GetYaxis()->SetRangeUser(1,100);
    
    gstpow->SetTitle(Form(" Stopping power of electrons in %s ; Energy [MeV] ; dE/dx [MeV/cm]",material.c_str()));
    gstpow->SetMarkerStyle(8);
    gstpow->Draw("AP");
    sstpow->Draw("same");
    gPad->SetLogy();
    gPad->SetLogx();
    




    // make the second plot
    TCanvas *c2 = new TCanvas("range","range",700,700);
    TGraph *grange = new TGraph(n,&energy[0],&range[0]);
    TGraph *grange2 = new TGraph(n,&range[0],&energy[0]); //inverted graph
    TSpline5 *srange = new TSpline5("grange",grange); 
    TSpline5 *srange2 = new TSpline5("grange2",grange2); //inverted spline
    srange->SetLineColor(kRed);
    gPad->SetLogx();
    grange->GetXaxis()->SetRangeUser(0,50);
    grange->GetYaxis()->SetRangeUser(0,5);
    grange->SetTitle(Form(" Range of electrons in %s ; Energy [MeV] ; Range [cm]",material.c_str()));
    grange->SetMarkerStyle(8);
    grange->Draw("AP");
    srange->Draw("same");


    // make the 3rd plot
    int nsamplestry=10000;
    int nmin=0;
    int nmax=12;
    double samplesize=(double)(nmax-nmin)/nsamplestry;
    //double Estart[] = {1.0,2.5,5.0,10.0,15.0,20.0,25.0,50.0};
    double Estart[] = {50,25,20,15,10,5,2.5,1.0};
    int ngraphs = sizeof(Estart)/sizeof(*Estart);
    int nsamplesreal[ngraphs];
    vector<double> dist[ngraphs];
    vector<double> LET[ngraphs];

    for(int i=0; i<ngraphs; i++)
    {
      nsamplesreal[i]=0;
      
      for(int j=0; j<nsamplestry; j++)
      {
        //cout<<"Start"<<endl;
        double R1=grange->Eval(Estart[i]);
        double R2=R1-samplesize*j;
        double Er=grange2->Eval(R2);
        //if(1.0*samplesize*j<=grange->Eval(Estart[i]))cout<<"samplesize*j " << 1.0*samplesize*j << "    maxrange " << grange->Eval(Estart[i]) <<endl;
        if(1.0*samplesize*j<=grange->Eval(Estart[i])){

          dist[i].push_back(samplesize*j);
          LET[i].push_back(samplesize*gstpow->Eval(Er));
          nsamplesreal[i]+=1;
        }
        //cout<<"stop"<<endl;
      } 
      
    }
    TGraph *gLET[ngraphs];
    TMultiGraph *mg = new TMultiGraph();
    for(int i=0; i<ngraphs; i++)
    {
      gLET[i] = new TGraph(nsamplesreal[i],&dist[i][0],&LET[i][0]);
      gLET[i]->SetTitle(Form("%.2f MeV ; Distance [cm] ; dE/dx [MeV/cm]",Estart[i]));
      gLET[i]->SetMarkerStyle(21+i);
      gLET[i]->SetLineColor(i);
      gLET[i]->SetMarkerSize(1);
      gLET[i]->GetXaxis()->SetRangeUser(0,4.5);
      gLET[i]->GetYaxis()->SetRangeUser(0,0.4);
      mg->Add(gLET[i],"ALP");
      
    }


    
    TCanvas *c3 = new TCanvas("LET","LET",700,700);
    mg->Draw("A pmc plc");
    mg->SetTitle(Form("Stopping Power of Electrons in %s ; Distance [cm] ; dE/dx [MeV/cm] ",material.c_str()));
    c3->BuildLegend(0.7,0.5,0.88,0.88,"","PL");
    mg->GetXaxis()->SetRangeUser(0,4.5);
    mg->GetYaxis()->SetRangeUser(0,35);
    gPad->Modified();
    gPad->Update();





    // make a fourth plot
    //loop over the LET parameter
    vector<double> sumEloss[ngraphs];

    for(int i=0; i<ngraphs; i++){
	double sumE=0;
        for(int j=0; j<LET[i].size(); j++){
	    sumE+=LET[i][j];
            sumEloss[i].push_back(sumE);
        }
    }

TGraph *gSumEloss[ngraphs];
    TMultiGraph *mg2 = new TMultiGraph();
    for(int i=0; i<ngraphs; i++)
    {
      gSumEloss[i] = new TGraph(nsamplesreal[i],&dist[i][0],&sumEloss[i][0]);
      gSumEloss[i]->SetTitle(Form("%.2f MeV ; Distance [cm] ; E Deposited [MeV]",Estart[i]));
      gSumEloss[i]->SetMarkerStyle(21+i);
      gSumEloss[i]->SetLineColor(i);
      gSumEloss[i]->SetMarkerSize(1);
      gSumEloss[i]->GetXaxis()->SetRangeUser(0,4.5);
      //gSumEloss[i]->GetYaxis()->SetRangeUser(5,35);
      mg2->Add(gSumEloss[i],"ALP");
      
    }
TCanvas *c4 = new TCanvas("SumELoss","SumELoss",700,700);
    mg2->Draw("A pmc plc");
    mg2->SetTitle(Form("LET of Electrons in %s ; Distance [cm] ; E Deposited [MeV] ",material.c_str()));
    c4->BuildLegend(0.7,0.5,0.88,0.88,"","PL");
    mg2->GetXaxis()->SetRangeUser(0,5);
    mg2->GetYaxis()->SetRangeUser(0,55);
    gPad->Modified();
    gPad->Update();



    // save the 3 canvases
    //c1->SaveAs(Form("%s_StoppingPower.pdf",material.c_str()));
    //c2->SaveAs(Form("%s_Range.pdf",material.c_str()));
    //sc3->SaveAs(Form("%s_LET.pdf",material.c_str()));



	gROOT->SetBatch(false);
    // plot all on single canvas
    TCanvas *call = new TCanvas("call","call",1200,800);
    call->Divide(2,2);
    call->cd(1);
    sstpow->SetLineColor(kRed);
    gstpow->GetXaxis()->SetRangeUser(1.0e-2,5e1);
    gstpow->GetYaxis()->SetRangeUser(1,100);
    gstpow->SetTitle(Form(" Stopping power of electrons in %s ; Energy [MeV] ; dE/dx [MeV/cm]",material.c_str()));
    gstpow->SetMarkerStyle(8);
    gstpow->Draw("AP");
    sstpow->Draw("same");
    gPad->SetLogx();
    gPad->SetLogy();

    call->cd(2);
    srange->SetLineColor(kRed);
    gPad->SetLogx();
    grange->GetXaxis()->SetRangeUser(0,50);
    grange->GetYaxis()->SetRangeUser(0,5);
    grange->SetTitle(Form(" Range of electrons in %s ; Energy [MeV] ; Range [cm]",material.c_str()));
    grange->SetMarkerStyle(8);
    grange->Draw("AP");
    srange->Draw("same");

    call->cd(3);
    mg->Draw("A pmc plc");
    mg->SetTitle(Form("Stopping Power of Electrons in %s ; Distance [cm] ; dE/dx [MeV/cm] ",material.c_str()));
    gPad->BuildLegend(0.7,0.5,0.88,0.88,"","PL");
    mg->GetXaxis()->SetRangeUser(0,4);
    mg->GetYaxis()->SetRangeUser(0,0.075);
    gPad->Modified();
    gPad->Update();

    call->cd(4);
    mg2->Draw("A pmc plc");
    mg2->SetTitle(Form("LET of Electrons in %s ; Distance [cm] ; E Deposited [MeV] ",material.c_str()));
    gPad->BuildLegend(0.7,0.5,0.88,0.88,"","PL");
    //mg->GetXaxis()->SetRangeUser(0,);
    //mg->GetYaxis()->SetRangeUser(0,0.4);
    gPad->Modified();
    gPad->Update();


call->SaveAs(Form("%s_all.pdf",material.c_str()));

}
