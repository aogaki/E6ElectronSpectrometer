#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "g4root.hh"

#include "ESRunAction.hpp"


ESRunAction::ESRunAction()
   : G4UserRunAction()
{}

ESRunAction::~ESRunAction()
{}

void ESRunAction::BeginOfRunAction(const G4Run *)
{
   G4AnalysisManager *anaMan = G4AnalysisManager::Instance();
   anaMan->SetVerboseLevel(1);
   G4cout << "Using " << anaMan->GetType()
          << " analysis manager" << G4endl;
   G4String fileName = "result";
   anaMan->OpenFile(fileName);

   // Ntuple
   anaMan->CreateNtuple("ES", "ESM study");
   anaMan->CreateNtupleIColumn(0, "EventID");
   anaMan->CreateNtupleIColumn(0, "TrackID");
   anaMan->CreateNtupleIColumn(0, "PDGCode");
   anaMan->CreateNtupleSColumn(0, "VolumeName");
   
   anaMan->CreateNtupleDColumn(0, "KineticEnergy");
   
   anaMan->CreateNtupleDColumn(0, "x");
   anaMan->CreateNtupleDColumn(0, "y");
   anaMan->CreateNtupleDColumn(0, "z");

   anaMan->CreateNtupleDColumn(0, "vx");
   anaMan->CreateNtupleDColumn(0, "vy");
   anaMan->CreateNtupleDColumn(0, "vz");

   anaMan->CreateNtupleSColumn(0, "VertexName");

   // Init parameters
   anaMan->CreateNtuple("InitPar", "Initial Parameters");
   anaMan->CreateNtupleIColumn(1, "EventID");
   anaMan->CreateNtupleIColumn(1, "PDGCode");
   anaMan->CreateNtupleDColumn(1, "KineticEnergy");
   anaMan->CreateNtupleDColumn(1, "vx");
   anaMan->CreateNtupleDColumn(1, "vy");
   anaMan->CreateNtupleDColumn(1, "vz");

   anaMan->FinishNtuple();
}

void ESRunAction::EndOfRunAction(const G4Run *)
{
   G4AnalysisManager *anaMan = G4AnalysisManager::Instance();
   anaMan->Write();
   anaMan->CloseFile();
}

