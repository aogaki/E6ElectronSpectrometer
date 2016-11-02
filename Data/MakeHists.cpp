TH2D *HisBx;

const Int_t kNoPar = 1597;
TVector3 PosPar[kNoPar];
TVector3 BPar[kNoPar];

const Int_t kNoPer = 318; // Par and Per? Think variable name!
TVector3 PosPer[kNoPer];
TVector3 BPer[kNoPer];

const Double_t kMaxB = 16327.;

void ReadingData()
{
   ifstream fin("parallel.txt");
   for(Int_t i = 0; i < kNoPar; i++){
      if(!(fin >> PosPar[i][0])) break; // file check
      fin >> PosPar[i][2];
      fin >> PosPar[i][1];

      fin >> BPar[i][0]; // waste mag. of B
      fin >> BPar[i][0];
      fin >> BPar[i][2];
      fin >> BPar[i][1];
   }
   fin.close();

   fin.open("per.txt");
   for(Int_t i = 0; i < kNoPer; i++){
      if(!(fin >> PosPer[i][0])) break; // file check
      fin >> PosPer[i][2];
      fin >> PosPer[i][1];

      fin >> BPer[i][0]; // waste mag. of B
      fin >> BPer[i][0];
      fin >> BPer[i][2];
      fin >> BPer[i][1];
   }
   fin.close();
}

void FillingHis()
{
   for(Int_t x = 1; x <= kNoPar; x++){
      for(Int_t y = 1; y <= kNoPer; y++){
         Double_t b = BPar[x - 1][0] * BPer[y - 1][0] / kMaxB;
         HisBx->SetBinContent(x, kNoPer - y + 1, b / 10000.); // gaus to tesla
      }
   }
}

void MakeHists()
{
   ReadingData();

   //HisBx = new TH2D("HisBx", "Bx", kNoPar, -798.75, 799.25, kNoPer, 1.25, 319.25);
   // shift to real geometry.  The sign of Y axis is changed
   HisBx = new TH2D("HisBx", "Bx",
                    kNoPar, -798.75 + 500., 799.25 + 500.,
                    kNoPer, -(319.25 - 135.), -(1.25 - 135.));
   FillingHis();

   TFile *file = new TFile("MagneticField.root", "RECREATE");
   HisBx->Write();
   file->Close();
   delete file;
   
}
