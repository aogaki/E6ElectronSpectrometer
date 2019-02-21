#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "g4root.hh"

#include "ESRunAction.hpp"

ESRunAction::ESRunAction() : G4UserRunAction()
{
  G4AnalysisManager *anaMan = G4AnalysisManager::Instance();
  anaMan->SetVerboseLevel(1);
  G4cout << "Using " << anaMan->GetType() << " analysis manager" << G4endl;
  G4String fileName = "result";
  anaMan->SetFileName(fileName);

  // Ntuple
  anaMan->CreateNtuple("ES", "ESM study");
  anaMan->CreateNtupleIColumn("EventID");
  anaMan->CreateNtupleIColumn("TrackID");
  anaMan->CreateNtupleIColumn("PDGCode");
  anaMan->CreateNtupleSColumn("VolumeName");

  anaMan->CreateNtupleDColumn("KineticEnergy");

  anaMan->CreateNtupleDColumn("x");
  anaMan->CreateNtupleDColumn("y");
  anaMan->CreateNtupleDColumn("z");

  anaMan->CreateNtupleDColumn("vx");
  anaMan->CreateNtupleDColumn("vy");
  anaMan->CreateNtupleDColumn("vz");

  anaMan->CreateNtupleSColumn("VertexName");

  anaMan->CreateNtupleDColumn("DepositEnergy");

  anaMan->CreateNtupleDColumn("IncidentEnergy");

  anaMan->CreateNtupleIColumn("IsLast");

  anaMan->CreateNtupleDColumn("Time");
  anaMan->FinishNtuple();

  // Init parameters
  anaMan->CreateNtuple("InitPar", "Initial Parameters");
  anaMan->CreateNtupleIColumn("EventID");
  anaMan->CreateNtupleIColumn("PDGCode");
  anaMan->CreateNtupleDColumn("KineticEnergy");
  anaMan->CreateNtupleDColumn("vx");
  anaMan->CreateNtupleDColumn("vy");
  anaMan->CreateNtupleDColumn("vz");
  anaMan->FinishNtuple();
}

ESRunAction::~ESRunAction() { delete G4AnalysisManager::Instance(); }

void ESRunAction::BeginOfRunAction(const G4Run *)
{
  G4AnalysisManager *anaMan = G4AnalysisManager::Instance();
  anaMan->OpenFile();
}

void ESRunAction::EndOfRunAction(const G4Run *)
{
  G4AnalysisManager *anaMan = G4AnalysisManager::Instance();
  anaMan->Write();
  anaMan->CloseFile();
}
