#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "g4root.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4VProcess.hh"

#include "ESExitSD.hpp"
#include "ESExitHit.hpp"


ESExitSD::ESExitSD(const G4String &name,
                   const G4String &hitsCollectionName)
   : G4VSensitiveDetector(name)
{
   collectionName.insert(hitsCollectionName);
}

ESExitSD::~ESExitSD()
{}

void ESExitSD::Initialize(G4HCofThisEvent *hce)
{
   fHitsCollection
      = new ESExitHitsCollection(SensitiveDetectorName, collectionName[0]);

   G4int hcID
      = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
   hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool ESExitSD::ProcessHits(G4Step *step, G4TouchableHistory */*history*/)
{
   if(!step->IsLastStepInVolume()) return false;
   G4Track *track = step->GetTrack();   
   G4int trackID = track->GetTrackID();
   //if(trackID != 1) return false;
   
   ESExitHit *newHit = new ESExitHit();

   G4ParticleDefinition *particle = track->GetDefinition();
   G4int pdgCode = particle->GetPDGEncoding();
   newHit->SetPDGCode(pdgCode);

   newHit->SetTrackID(trackID);
   
   G4StepPoint *preStepPoint = step->GetPreStepPoint();
   G4String volumeName = preStepPoint->GetPhysicalVolume()->GetName();
   newHit->SetVolumeName(volumeName);

   G4StepPoint *postStepPoint = step->GetPostStepPoint();
   G4ThreeVector position =  postStepPoint->GetPosition();
   newHit->SetPosition(position);
   
   G4double kineticEnergy = postStepPoint->GetKineticEnergy();
   newHit->SetKineticEnergy(kineticEnergy);

   G4ThreeVector momentum =  postStepPoint->GetMomentum();
   newHit->SetMomentum(momentum);

   fHitsCollection->insert(newHit);
   return true;
}
