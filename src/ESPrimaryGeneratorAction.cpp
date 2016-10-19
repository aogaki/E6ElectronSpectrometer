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


static G4int nEveInPGA = 0; // Global variable change to local? 
G4Mutex mutexInPGA = G4MUTEX_INITIALIZER;

ESPrimaryGeneratorAction::ESPrimaryGeneratorAction(G4bool useMonoEne, G4double beamEne)
   : G4VUserPrimaryGeneratorAction(),
     fParticleGun(nullptr),
     fUseMonoEne(useMonoEne),
     fBeamEne(beamEne*GeV),
     fMessenger(nullptr)
{
   G4AutoLock lock(&mutexInPGA);
   
   G4int nPar = 1;
   fParticleGun = new G4ParticleGun(nPar);

   fZPosition = -2703.*mm;
   fThetaMax = 1.5*mrad;
   fCosMax = cos(fThetaMax);
   
   G4ParticleTable *parTable = G4ParticleTable::GetParticleTable();
   G4ParticleDefinition *electron = parTable->FindParticle("e-");
   fParticleGun->SetParticleDefinition(electron);
   fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., fZPosition));
   fParVec = G4ThreeVector(0., 0., 1.);
   fParticleGun->SetParticleMomentumDirection(fParVec);
   fParticleGun->SetParticleEnergy(fBeamEne);

   if(fUseMonoEne) GunFuncPointer = &ESPrimaryGeneratorAction::MonoEneGun;
   else GunFuncPointer = &ESPrimaryGeneratorAction::UniformGun;

   DefineCommands();
}

ESPrimaryGeneratorAction::~ESPrimaryGeneratorAction()
{
   delete fParticleGun;
   delete fMessenger;
}

void ESPrimaryGeneratorAction::MonoEneGun()
{
   // Do nothing
   // Using the parameters set in constructor
}

void ESPrimaryGeneratorAction::UniformGun()
{
   G4double theta = acos(1. - G4UniformRand() * (1. - fCosMax));
   G4double phi = G4UniformRand() * 2. * CLHEP::pi;
   
   fParVec.setRThetaPhi(1., theta, phi);
   fBeamEne = (G4UniformRand() + 8)*GeV;
}

void ESPrimaryGeneratorAction::GeneratePrimaries(G4Event *event)
{
   (this->*GunFuncPointer)(); 
   
   fParticleGun->SetParticleMomentumDirection(fParVec);
   fParticleGun->SetParticleEnergy(fBeamEne);
   fParticleGun->GeneratePrimaryVertex(event);

   G4AnalysisManager *anaMan = G4AnalysisManager::Instance();
   anaMan->FillNtupleIColumn(1, 0, event->GetEventID());
   anaMan->FillNtupleIColumn(1, 1, 11); // electron PDG code
   anaMan->FillNtupleDColumn(1, 2, fBeamEne);
   anaMan->FillNtupleDColumn(1, 3, fParVec.x());
   anaMan->FillNtupleDColumn(1, 4, fParVec.y());
   anaMan->FillNtupleDColumn(1, 5, fParVec.z());
   anaMan->AddNtupleRow(1);

   G4AutoLock lock(&mutexInPGA);
   if (nEveInPGA++ % 10000 == 0)
      G4cout << nEveInPGA - 1 << " events done" << G4endl;

}

void ESPrimaryGeneratorAction::DefineCommands()
{
   fMessenger = new G4GenericMessenger(this, "/ES/Primary/", 
                                       "Beam control");

   // z position
   G4GenericMessenger::Command &sourceZCmd
      = fMessenger->DeclareMethodWithUnit("Z", "mm",
                                          &ESPrimaryGeneratorAction::SetSourceZ, 
                                          "Set the length of source.");

   sourceZCmd.SetParameterName("z", true);
   sourceZCmd.SetRange("z>=-4000. && z<0.");
   sourceZCmd.SetDefaultValue("-2703");
}

void ESPrimaryGeneratorAction::SetSourceZ(G4double z)
{
   fZPosition = z;
   fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., fZPosition));
}
