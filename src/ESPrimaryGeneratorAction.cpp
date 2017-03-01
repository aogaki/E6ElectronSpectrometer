#include <fstream>
#include <math.h>
#include <time.h>

#include <TFile.h>
#include <TH1.h>
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

ESPrimaryGeneratorAction::ESPrimaryGeneratorAction(BeamState beam, G4double beamEne)
   : G4VUserPrimaryGeneratorAction(),
     fParticleGun(nullptr),
     fBeamState(beam),
     fBeamEne(beamEne*GeV),
     fFncNorm(nullptr),
     fHisBeam(nullptr),
     fMessenger(nullptr)
{
   G4AutoLock lock(&mutexInPGA);
   
   G4int nPar = 1;
   fParticleGun = new G4ParticleGun(nPar);

   TFile *file = new TFile("Data/NamBeam.root", "READ");
   fHisBeam = (TH1D*)file->Get("HisBeam");
   fFncNorm = (TF1*)file->Get("FncNorm");
   fHisBeam->SetDirectory(0);
   //fFncNorm->SetDirectory(0); // TF1 is not dead after file closed.  But, why?
   file->Close();
   delete file;

   // Each thread got each random generatot.  It works.
   // But, using /dev/urand is more better?
   Int_t seed = G4UniformRand() * 1000000;
   G4cout << "Seed of PGA = " << seed << G4endl;
   gRandom->SetSeed(seed);
   
   fZPosition = kSourceZPos;
   if(fBeamState == BeamState::Wide || fBeamState == BeamState::MonoWide) fThetaMax = 3.5*mrad;
   //fThetaMax = atan((kMagnetGap / 2.) / -kSourceZPos);
   else fThetaMax = 1.5*mrad; // zero angle and reference are set later
   
   fCosMax = cos(fThetaMax);

   if(fBeamState == BeamState::Reference) fBeamEne = 0.;
   
   G4ParticleTable *parTable = G4ParticleTable::GetParticleTable();
   G4ParticleDefinition *electron = parTable->FindParticle("e-");
   fParticleGun->SetParticleDefinition(electron);
   fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., fZPosition));
   fParVec = G4ThreeVector(0., 0., 1.);
   fParticleGun->SetParticleMomentumDirection(fParVec);
   fParticleGun->SetParticleEnergy(fBeamEne);

   if(fBeamState == BeamState::Mono || fBeamState == BeamState::MonoZero || fBeamState == BeamState::MonoWide)
      GunPointer = &ESPrimaryGeneratorAction::MonoEneGun;
   else if(fBeamState == BeamState::Reference) GunPointer = &ESPrimaryGeneratorAction::ReferenceGun;
   else GunPointer = &ESPrimaryGeneratorAction::UniformGun;
   //else GunPointer = &ESPrimaryGeneratorAction::NamGun;

   if(fBeamState == BeamState::ZeroAng || fBeamState == BeamState::MonoZero)
      AngGenPointer = &ESPrimaryGeneratorAction::ZeroAng;
   else AngGenPointer = &ESPrimaryGeneratorAction::UniformAng;

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
   (this->*AngGenPointer)(); 
}

void ESPrimaryGeneratorAction::ZeroAng()
{
   // Do nothing
   // Using the parameters set in constructor
   // I wish no overhead to call this function
   // Check!
}

void ESPrimaryGeneratorAction::UniformGun()
{
   //fBeamEne = (9 * G4UniformRand() + 1)*GeV;
   fBeamEne = (10. * G4UniformRand())*GeV;
   (this->*AngGenPointer)(); 
}

void ESPrimaryGeneratorAction::ReferenceGun()
{
   fBeamEne += 10.*MeV;
}

void ESPrimaryGeneratorAction::NamGun()
{
   G4AutoLock lock(&mutexInPGA);
   fBeamEne = fFncNorm->Eval(fHisBeam->GetRandom())*GeV;
   lock.unlock();
   (this->*AngGenPointer)(); 
}

void ESPrimaryGeneratorAction::UniformAng()
{
   G4double theta = acos(1. - G4UniformRand() * (1. - fCosMax));
   G4double phi = G4UniformRand() * 2. * CLHEP::pi;
   fParVec.setRThetaPhi(1., theta, phi);
}

void ESPrimaryGeneratorAction::GeneratePrimaries(G4Event *event)
{
   (this->*GunPointer)(); 
   
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
   sourceZCmd.SetDefaultValue("-3000");
}

void ESPrimaryGeneratorAction::SetSourceZ(G4double z)
{
   fZPosition = z;
   fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., fZPosition));
}
