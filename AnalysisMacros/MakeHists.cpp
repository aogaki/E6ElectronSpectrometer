#include <fstream>
#include <iostream>

#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLegend.h>

TH1D *HisTheta[3];
TH1D *HisCosTheta[3];
TH1D *HisPhi[3];
TH1D *HisEne[3];
TH2D *HisPos[3];
TH2D *HisVec[3];


void InitHists()
{
   TString title[3]{"Before window", "After window", "After air"};
   for(Int_t i = 0; i < 3; i++){
      HisTheta[i] = new TH1D(Form("HisTheta%d", i), title[i], 1000, 0, 10.);
      HisTheta[i]->SetXTitle("#it{#theta} [mrad]");
      HisTheta[i]->SetYTitle("No. particles");

      HisCosTheta[i] = new TH1D(Form("HisCosTheta%d", i), title[i], 2000, 0.9999, 1.0001);
      HisCosTheta[i]->SetXTitle("#it{Cos#theta}");
      HisCosTheta[i]->SetYTitle("No. particles");

      HisPhi[i] = new TH1D(Form("HisPhi%d", i), title[i], 1000, 0, 0);
      HisPhi[i]->SetXTitle("#it{#phi} [rad]");
      HisPhi[i]->SetYTitle("No. particles");

      HisEne[i] = new TH1D(Form("HisEne%d", i), title[i], 150000, 0, 15);
      HisEne[i]->SetXTitle("Kinetic energy [Gev]");
      HisEne[i]->SetYTitle("No. particles");

      HisPos[i] = new TH2D(Form("HisPos%d", i), title[i], 1000, -5., 5., 1000, -5., 5.);
      HisPos[i]->SetXTitle("Position [mm]");
      HisPos[i]->SetYTitle("Position [mm]");
      HisPos[i]->SetZTitle("No. particles");

      HisVec[i] = new TH2D(Form("HisVec%d", i), title[i], 1000, -0.005, 0.005, 1000, -0.005, 0.005);
      HisVec[i]->SetXTitle("");
      HisVec[i]->SetYTitle("");
      HisVec[i]->SetZTitle("No. particles");
   }
}

void FillHists(TFile *file)
{
   TTree *tree;
   tree = (TTree*)file->Get("ES");
   tree->SetBranchStatus("*", 0);

   Int_t trackID;
   tree->SetBranchStatus("TrackID", 1);
   tree->SetBranchAddress("TrackID", &trackID);
   
   Double_t ene;
   tree->SetBranchStatus("KineticEnergy", 1);
   tree->SetBranchAddress("KineticEnergy", &ene);

   TVector3 pos;
   tree->SetBranchStatus("x", 1);
   tree->SetBranchStatus("y", 1);
   tree->SetBranchStatus("z", 1);
   tree->SetBranchAddress("x", &pos[0]);
   tree->SetBranchAddress("y", &pos[1]);
   tree->SetBranchAddress("z", &pos[2]);

   TVector3 vec;
   tree->SetBranchStatus("vx", 1);
   tree->SetBranchStatus("vy", 1);
   tree->SetBranchStatus("vz", 1);
   tree->SetBranchAddress("vx", &vec[0]);
   tree->SetBranchAddress("vy", &vec[1]);
   tree->SetBranchAddress("vz", &vec[2]);
   
   Char_t volumeName[256]; // Array size is enough?
   tree->SetBranchStatus("VolumeName", 1);
   tree->SetBranchAddress("VolumeName", volumeName);

   const Int_t kNoEvents = tree->GetEntries();
   for(Int_t i = 0; i < kNoEvents; i++){
      tree->GetEntry(i);
      if(trackID == 1){ // primary electron only
         TString name = TString(volumeName); // think name and logic
         Int_t volIndex = -1;
         if(name == "Vacuum") volIndex = 0;
         else if(name == "Window") volIndex = 1;
         else if(name == "Air") volIndex = 2;
         else{ cout << "What is this? " << name << endl; exit(0);}

         HisTheta[volIndex]->Fill(vec.Theta() * 1000.);
         HisCosTheta[volIndex]->Fill(vec.CosTheta());
         HisPhi[volIndex]->Fill(vec.Phi());
         HisEne[volIndex]->Fill(ene / 1000.);
         HisPos[volIndex]->Fill(pos.x(), pos.y());
         HisVec[volIndex]->Fill(vec.x() / vec.Mag(), vec.y() / vec.Mag());

      }
   }
   
   delete tree;
}

void FitHists()
{

}

void SaveHists(TString fileName)
{
   TString pdfName = "8GeV_" + fileName + ".pdf";
   TCanvas *recCan = new TCanvas("recCan", "");                                                         
   TCanvas *squCan = new TCanvas("squCan", "", 600, 600);                                               

   recCan->cd();                                                                                        
   recCan->Print(pdfName + "[", "pdf");
   for(Int_t i = 0; i < 3; i++){
      HisTheta[i]->Draw();
      recCan->Print(pdfName, "pdf");
   }
   /*
   for(Int_t i = 0; i < 3; i++){
      HisCosTheta[i]->Draw();
      recCan->Print(pdfName, "pdf");
   }
   for(Int_t i = 0; i < 3; i++){
      HisPhi[i]->Draw();
      recCan->Print(pdfName, "pdf");
   }
   */
   for(Int_t i = 0; i < 3; i++){
      HisEne[i]->Draw();
      recCan->Print(pdfName, "pdf");
   }
   squCan->cd();
   for(Int_t i = 0; i < 3; i++){
      HisPos[i]->Draw("COLZ");
      squCan->Print(pdfName, "pdf");
   }
   for(Int_t i = 0; i < 3; i++){
      HisVec[i]->Draw("COLZ");
      squCan->Print(pdfName, "pdf");
   }
   recCan->cd();                                                                                        
   recCan->Print(pdfName + "]", "pdf");
   
   delete squCan;
   delete recCan;
}

void CheckEnergyLoss()
{
   gStyle->SetOptStat(0);
   
   Int_t maxBin = 80050;
   Int_t minBin = 79951;
   
   TCanvas *recCan = new TCanvas("recCan", "");
   recCan->Print("ene.pdf[", "pdf");
   
   HisEne[0]->SetLineColor(1);
   HisEne[1]->SetLineColor(2);
   HisEne[2]->SetLineColor(4);

   HisEne[2]->SetMaximum(HisEne[0]->GetMaximum());
   
   HisEne[2]->Draw();
   HisEne[1]->Draw("SAME");
   HisEne[0]->Draw("SAME");
   
   TLegend *leg1 = new TLegend(0.6, 0.6, 0.9, 0.9);
   leg1->SetTextSize(0.045);
   leg1->SetHeader("Energy distribution", "C");
   leg1->AddEntry(HisEne[0], "Before window");
   leg1->AddEntry(HisEne[1], "After window");
   leg1->AddEntry(HisEne[2], "After air");
   leg1->Draw();

   recCan->SetGridx();
   recCan->SetGridy();
   recCan->Print("ene.pdf", "pdf");

   recCan->SetLogy();
   recCan->Print("ene.pdf", "pdf");

   HisEne[2]->GetXaxis()->SetRange(minBin, maxBin);
   HisEne[2]->Draw();
   HisEne[1]->Draw("SAME");
   HisEne[0]->Draw("SAME");
   leg1->Draw();
   recCan->SetLogy(0);
   recCan->Print("ene.pdf", "pdf");

   recCan->SetLogy();
   recCan->Print("ene.pdf", "pdf");

   recCan->Print("ene.pdf]", "pdf");

   cout << "Before Window:\t" << HisEne[0]->Integral(minBin, maxBin)
        << "\nAfter Window:\t" << HisEne[1]->Integral(minBin, maxBin)
        << "\nAfter Air:\t" << HisEne[2]->Integral(minBin, maxBin) << endl;

   ofstream fout("ene.txt");
   fout << "Before Window:\t" << HisEne[0]->Integral(minBin, maxBin)
        << "\nAfter Window:\t" << HisEne[1]->Integral(minBin, maxBin)
        << "\nAfter Air:\t" << HisEne[2]->Integral(minBin, maxBin) << endl;
   fout.close();

   delete recCan;
}

void CheckScattering()
{
   gStyle->SetOptStat(0);
   
   TCanvas *recCan = new TCanvas("recCan", "");
   recCan->Print("theta.pdf[", "pdf");
   
   HisTheta[0]->SetLineColor(1);
   HisTheta[1]->SetLineColor(2);
   HisTheta[2]->SetLineColor(4);

   HisTheta[1]->Draw();
   HisTheta[2]->Draw("SAME");
   HisTheta[0]->Draw("SAME");
   
   TLegend *leg1 = new TLegend(0.6, 0.6, 0.9, 0.9);
   leg1->SetTextSize(0.045);
   leg1->SetHeader("Divergence angle", "C");
   leg1->AddEntry(HisTheta[0], "Before window");
   leg1->AddEntry(HisTheta[1], "After window");
   leg1->AddEntry(HisTheta[2], "After air");
   leg1->Draw();
   
   recCan->SetGridx();
   recCan->SetGridy();
   recCan->Print("theta.pdf", "pdf");

   HisTheta[1]->GetXaxis()->SetRange(1, 200);
   HisTheta[1]->Draw();
   HisTheta[2]->Draw("SAME");
   HisTheta[0]->Draw("SAME");
   leg1->Draw();
   recCan->Print("theta.pdf", "pdf");
   
   // Cos theta
   HisCosTheta[0]->SetLineColor(1);
   HisCosTheta[1]->SetLineColor(2);
   HisCosTheta[2]->SetLineColor(4);

   HisCosTheta[1]->Draw();
   HisCosTheta[2]->Draw("SAME");
   HisCosTheta[0]->Draw("SAME");
   
   TLegend *leg2 = new TLegend(0.6, 0.6, 0.9, 0.9);
   leg2->SetTextSize(0.045);
   leg2->SetHeader("Divergence angle", "C");
   leg2->AddEntry(HisCosTheta[0], "Before window");
   leg2->AddEntry(HisCosTheta[1], "After window");
   leg2->AddEntry(HisCosTheta[2], "After air");
   leg2->Draw();
   
   recCan->SetGridx();
   recCan->SetGridy();
   recCan->SetLogy();
   recCan->Print("theta.pdf", "pdf");

   HisCosTheta[1]->GetXaxis()->SetRange(951, 1050);
   HisCosTheta[1]->Draw();
   HisCosTheta[2]->Draw("SAME");
   HisCosTheta[0]->Draw("SAME");
   leg2->Draw();
   recCan->SetLogy();
   recCan->Print("theta.pdf", "pdf");
   
   recCan->Print("theta.pdf]", "pdf");

   delete recCan;
}

void CheckPosition()
{
   gStyle->SetOptStat(0);
   
   TCanvas *squCan = new TCanvas("squCan", "", 600, 600);
   HisPos[2]->Draw("COLZ");
   squCan->Print("pos.pdf", "pdf");
   
   TAxis *xAxis = HisPos[2]->GetXaxis();
   TAxis *yAxis = HisPos[2]->GetYaxis();
   Int_t nX = xAxis->GetNbins();
   Int_t nY = yAxis->GetNbins();
   Double_t counter = 0;
   for(Int_t x = 1; x <= nX; x++){
      for(Int_t y = 1; y <= nY; y++){
         Double_t xPos = xAxis->GetBinCenter(x);
         Double_t yPos = yAxis->GetBinCenter(y);
         Double_t distance = sqrt(xPos*xPos + yPos*yPos);
         if(distance < 2.) counter += HisPos[2]->GetBinContent(x, y);
      }
   }

   ofstream fout("pos.txt");
   cout << counter << " Electrons reach the magnet" << endl;
   fout << counter << " Electrons reach the magnet" << endl;
   fout.close();

   delete squCan;
}

void MakeHists(TString fileName = "noWindow.root")
{
   InitHists();

   TFile *file = new TFile(fileName, "OPEN");
   FillHists(file);
   //SaveHists(fileName);
   file->Close();
   delete file;

   CheckEnergyLoss();
   CheckScattering();
   CheckPosition();
}
