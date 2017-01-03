#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4ThreeVector.hh"
#include "g4root.hh"

#include "ESEventAction.hpp"


ESEventAction::ESEventAction()
   : G4UserEventAction(),
     fHitsCollectionID(-1)
{
   // set printing per each event
   //G4RunManager::GetRunManager()->SetPrintProgress(1);
}

ESEventAction::~ESEventAction()
{}

ESExitHitsCollection *ESEventAction::GetHitsCollection(G4int hcID, const G4Event *event)
const
{
   ESExitHitsCollection *hitsCollection 
      = static_cast<ESExitHitsCollection *>(
         event->GetHCofThisEvent()->GetHC(hcID));
  
   if ( ! hitsCollection ) {
      G4ExceptionDescription msg;
      msg << "Cannot access hitsCollection ID " << hcID; 
      G4Exception("B4cEventAction::GetHitsCollection()",
                  "MyCode0003", FatalException, msg);
   }         

   return hitsCollection;
}

void ESEventAction::BeginOfEventAction(const G4Event *)
{}

void ESEventAction::EndOfEventAction(const G4Event *event)
{
   if (fHitsCollectionID == -1)
      fHitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID("ExitHC");
   
   ESExitHitsCollection *hc = GetHitsCollection(fHitsCollectionID, event);
   
   G4int eventID = event->GetEventID();

   G4AnalysisManager *anaMan = G4AnalysisManager::Instance();

   const G4int kHit = hc->entries();
   for (G4int iHit = 0; iHit < kHit; iHit++) {
      ESExitHit *newHit = (*hc)[iHit];

      anaMan->FillNtupleIColumn(0, 0, eventID); // EventID

      G4int trackID = newHit->GetTrackID();
      anaMan->FillNtupleIColumn(0, 1, trackID);

      G4int pdgCode = newHit->GetPDGCode();
      anaMan->FillNtupleIColumn(0, 2, pdgCode);

      G4String volumeName = newHit->GetVolumeName();
      anaMan->FillNtupleSColumn(0, 3, volumeName);

      G4double kineticEnergy = newHit->GetKineticEnergy();
      anaMan->FillNtupleDColumn(0, 4, kineticEnergy);

      G4ThreeVector position = newHit->GetPosition();
      anaMan->FillNtupleDColumn(0, 5, position.x());
      anaMan->FillNtupleDColumn(0, 6, position.y());
      anaMan->FillNtupleDColumn(0, 7, position.z());

      G4ThreeVector momentum = newHit->GetMomentum();
      anaMan->FillNtupleDColumn(0, 8, momentum.x());
      anaMan->FillNtupleDColumn(0, 9, momentum.y());
      anaMan->FillNtupleDColumn(0, 10, momentum.z());

      G4String vertexName = newHit->GetVertexName();
      anaMan->FillNtupleSColumn(0, 11, vertexName);
      
      anaMan->AddNtupleRow(0);
   }

}

