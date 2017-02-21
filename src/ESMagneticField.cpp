#include <TFile.h>
#include <TH2.h>

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4AutoLock.hh"

#include "ESMagneticField.hpp"


G4Mutex mutexInMF = G4MUTEX_INITIALIZER;

//static G4ThreadLocal TH2D *fHisBx{nullptr};


ESMagneticField::ESMagneticField()
   : G4MagneticField(),
     fFieldManager(nullptr),
     fHisBx(nullptr)
{
   fFieldManager = new G4FieldManager();
   fFieldManager->SetDetectorField(this);    
   fFieldManager->CreateChordFinder(this);

   G4AutoLock lock(&mutexInMF);
   TFile *file = new TFile("Data/MagneticField.root", "READ");
   fHisBx = (TH2D*)file->Get("HisBx");
   fHisBx->SetDirectory(0);
   file->Close();
   delete file;

   // Convert magnetic field map from tesla to default unit in G4
   G4int xMax = fHisBx->GetXaxis()->GetNbins();
   G4int yMax = fHisBx->GetYaxis()->GetNbins();
   for(G4int x = 1; x <= xMax; x++ ){
      for(G4int y = 1; y <= yMax; y++ ){
         G4double b = fHisBx->GetBinContent(x, y) * tesla;
         fHisBx->SetBinContent(x, y, b);
      }
   }

   // Using global cordinate
   fXMin = -10.*mm;
   fXMax = 10.*mm;
   fYMin = fHisBx->GetYaxis()->GetXmin()*mm;
   //fYMax = fHisBx->GetYaxis()->GetXmax()*mm;
   fYMax = 35.*mm;
   fZMin = fHisBx->GetXaxis()->GetXmin()*mm;
   fZMax = fHisBx->GetXaxis()->GetXmax()*mm;
   //fZMin = -100.*mm;
   //fZMax = 1000.*mm;

   // cut fYMax due to iron york
   fYMax = 35.*mm;

   G4cout << fZMin <<"\t"<< fZMax << G4endl;
   //exit(0);
}

ESMagneticField::~ESMagneticField()
{
   delete fFieldManager;
   delete fHisBx;
}

void ESMagneticField::GetFieldValue(const G4double point[4], 
                                    G4double* bField) const
{
   bField[0] = bField[1] = bField[2] = 0.;

   if(fXMin < point[0] && point[0] < fXMax &&
      fYMin < point[1] && point[1] < fYMax &&
      fZMin < point[2] && point[2] < fZMax)
      bField[0] = fHisBx->Interpolate(point[2], point[1]);
}

G4FieldManager*  ESMagneticField::GetFieldManager() const
{
   return fFieldManager;
}
