#include <fstream>
#include <math.h>
#include <time.h>

#include <TFile.h>
#include <TH2.h>
#include <TF1.h>
#include <TRandom3.h>

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "Randomize.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"
#include "g4root.hh"
#include "G4GenericMessenger.hh"

#include "ESPrimaryGeneratorAction.hpp"


G4int nEveInPGA = 0; // Global variable change to local? 
G4Mutex mutexInPGA = G4MUTEX_INITIALIZER;

ESPrimaryGeneratorAction::ESPrimaryGeneratorAction()
   : G4VUserPrimaryGeneratorAction(),
     fParticleGun(nullptr)
{
   G4AutoLock lock(&mutexInPGA);
   
   G4int nPar = 1;
   fParticleGun = new G4ParticleGun(nPar);

   fZPosition = -2500.*mm;
   fThetaMax = 1.5*mrad;
   fCosMax = cos(fThetaMax);
   
   G4ParticleTable *parTable = G4ParticleTable::GetParticleTable();
   G4ParticleDefinition *electron = parTable->FindParticle("e-");
   fParticleGun->SetParticleDefinition(electron);
   fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., fZPosition));
   fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
   fParticleGun->SetParticleEnergy(9.*GeV);

}

ESPrimaryGeneratorAction::~ESPrimaryGeneratorAction()
{
   if(fParticleGun != nullptr) {delete fParticleGun; fParticleGun = nullptr;}
}

void ESPrimaryGeneratorAction::GeneratePrimaries(G4Event *event)
{
   G4double ene = 8.*GeV;
   G4ThreeVector parVec = G4ThreeVector(0., 0., 1.);
/*
   G4double theta = acos(1. - G4UniformRand() * (1. - fCosMax));
   G4double phi = G4UniformRand() * 2. * CLHEP::pi;
   
   parVec.setRThetaPhi(1., theta, phi);
   ene = (G4UniformRand() + 8)*GeV;
*/
   fParticleGun->SetParticleMomentumDirection(parVec);
   fParticleGun->SetParticleEnergy(ene);
   fParticleGun->GeneratePrimaryVertex(event);

   G4AnalysisManager *anaMan = G4AnalysisManager::Instance();
   anaMan->FillNtupleIColumn(1, 0, event->GetEventID());
   anaMan->FillNtupleIColumn(1, 1, 11); // electron PDG code
   anaMan->FillNtupleDColumn(1, 2, ene);
   anaMan->FillNtupleDColumn(1, 3, parVec.x());
   anaMan->FillNtupleDColumn(1, 4, parVec.y());
   anaMan->FillNtupleDColumn(1, 5, parVec.z());
   anaMan->AddNtupleRow(1);

   G4AutoLock lock(&mutexInPGA);
   if (nEveInPGA++ % 10000 == 0)
      G4cout << nEveInPGA - 1 << " events done" << G4endl;

}
