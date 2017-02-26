#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "g4root.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4VProcess.hh"

#include "ESSD.hpp"
#include "ESHit.hpp"


ESSD::ESSD(const G4String &name,
           const G4String &hitsCollectionName)
   : G4VSensitiveDetector(name)
{
   collectionName.insert(hitsCollectionName);
}

ESSD::~ESSD()
{}

void ESSD::Initialize(G4HCofThisEvent *hce)
{
   fHitsCollection
      = new ESHitsCollection(SensitiveDetectorName, collectionName[0]);

   G4int hcID
      = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
   hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool ESSD::ProcessHits(G4Step *step, G4TouchableHistory */*history*/)
{
   //if(!step->IsLastStepInVolume()) return false; // only the last step
   G4Track *track = step->GetTrack();   
   G4int trackID = track->GetTrackID();
   //if(trackID != 1) return false; // only the primal particle

   ESHit *newHit = new ESHit();

   newHit->SetTrackID(trackID);
   
   G4ParticleDefinition *particle = track->GetDefinition();
   G4int pdgCode = particle->GetPDGEncoding();
   //if(pdgCode != 22) return false;
   newHit->SetPDGCode(pdgCode);

   G4int isLast = step->IsLastStepInVolume();
   newHit->SetIsLast(isLast);
      
   G4StepPoint *preStepPoint = step->GetPreStepPoint();
   G4String volumeName = preStepPoint->GetPhysicalVolume()->GetName();
   newHit->SetVolumeName(volumeName);

   if(preStepPoint->GetStepStatus() == fGeomBoundary){
      G4double incidentEnergy = preStepPoint->GetKineticEnergy();
      newHit->SetIncidentEnergy(incidentEnergy);
   }
   else newHit->SetIncidentEnergy(0.);
   
   G4String vertexName = track->GetLogicalVolumeAtVertex()->GetName();
   newHit->SetVertexName(vertexName);

   G4StepPoint *postStepPoint = step->GetPostStepPoint();
   G4ThreeVector position =  postStepPoint->GetPosition();
   newHit->SetPosition(position);
   
   G4double kineticEnergy = postStepPoint->GetKineticEnergy();
   newHit->SetKineticEnergy(kineticEnergy);

   G4ThreeVector momentum =  postStepPoint->GetMomentum();
   newHit->SetMomentum(momentum);

   G4double depositEnergy = step->GetTotalEnergyDeposit();
   newHit->SetDepositEnergy(depositEnergy);

   G4double time = postStepPoint->GetGlobalTime();
   newHit->SetTime(time);

   fHitsCollection->insert(newHit);
   return true;
}
