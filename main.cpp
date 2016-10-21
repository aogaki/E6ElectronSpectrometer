#include <time.h>

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "QGSP_BERT_HP.hh"
#include "QGSP_BIC_HP.hh"
#include "Shielding.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#include "G4TrajectoryParticleFilter.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

// Remove?
#include "Randomize.hh"

#include "ESDetectorConstruction.hpp"
#include "ESActionInitialization.hpp"
#include "ESPrimaryGeneratorAction.hpp"
#include "ESPhysicsList.hpp"
#include "ESConstants.hpp"


namespace {
   void PrintUsage()
   {
      G4cerr << " Usage: " << G4endl;
      G4cerr << " ./E6ES [-m macro filename]\n"
             << " -a show all particles. default only e-\n"
             << " -e [kinetic energy in GeV] Using mono-energetic beam \n"
             << " -z zero angle beam \n"
             << " -c [0: no collimator, 1: collimator in air, 2: collimator in vacuum] \n"
             << G4endl;
   }
}

unsigned int GetRandomSeed()
{
   // Using /dev/urandom for generating random number.
   // If it is not, I have to think solution.
   unsigned int seed;
   std::ifstream file("/dev/urandom", std::ios::binary);
   if (file.is_open()) {
      char *memblock;
      int size = sizeof(int);
      memblock = new char[size];
      file.read(memblock, size);
      file.close();
      seed = *reinterpret_cast<int *>(memblock);
      delete[] memblock;
   } else {
      seed = 0;
   }

   return seed;
}

int main(int argc, char **argv)
{
   G4String macro = "";
   G4bool showAll = false;
   G4bool useMonoEne = false;
   G4bool useZeroAng = false;
   G4double beamEne = 9.;
   ColliState colliState = ColliState::No;
   
   for (G4int i = 1; i < argc; i++) {
      if (G4String(argv[i]) == "-m")
         macro = argv[++i];
      else if (G4String(argv[i]) == "-a")
         showAll = true;
      else if (G4String(argv[i]) == "-e"){
         useMonoEne = true;
         beamEne = std::stod(argv[++i]);
      }
      else if (G4String(argv[i]) == "-z")
         useZeroAng = true;
      else if (G4String(argv[i]) == "-c"){
         G4int colliFlag = atoi(argv[++i]);
         if(colliFlag == 0) colliState = ColliState::No;
         else if(colliFlag == 1) colliState = ColliState::InAir;
         else if(colliFlag == 2) colliState = ColliState::InVac;
         else {
            PrintUsage();
            return 1;
         }
      }
      else {
         PrintUsage();
         return 1;
      }
   }
   
   // Remove?
   // Choose the Random engine
   CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
   unsigned int seed = GetRandomSeed();
   if (seed == 0) seed = time(0);
   G4cout << "\nseed = " << seed << G4endl;
   CLHEP::HepRandom::setTheSeed(seed);
   G4Random::setTheSeed(seed);

   // Construct the default run manager
#ifdef G4MULTITHREADED
   G4MTRunManager *runManager = new G4MTRunManager();
   runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
   //runManager->SetNumberOfThreads(1);
#else
   G4RunManager *runManager = new G4RunManager();
#endif

   // Set mandatory initialization classes
   //
   // Detector construction
   runManager->SetUserInitialization(new ESDetectorConstruction(colliState));

   // Physics list
   //G4VModularPhysicsList *physicsList = new FTFP_BERT();
   //G4VModularPhysicsList *physicsList = new QGSP_BERT_HP();
   //G4VModularPhysicsList *physicsList = new QGSP_BIC_HP();
   //G4VModularPhysicsList *physicsList = new Shielding();
   G4VModularPhysicsList *physicsList = new ESPhysicsList();
   physicsList->SetVerboseLevel(0);
   runManager->SetUserInitialization(physicsList);

   // Primary generator action and User action intialization
   runManager->SetUserInitialization(new ESActionInitialization(useMonoEne, beamEne, useZeroAng));

   // Initialize G4 kernel
   //
   runManager->Initialize();

#ifdef G4VIS_USE
   // Initialize visualization
   G4VisManager *visManager = new G4VisExecutive();
   visManager->Initialize();

   if (!showAll) { //Show only electron
      G4TrajectoryParticleFilter *filterp = new G4TrajectoryParticleFilter();
      filterp->Add("e-");
      visManager->RegisterModel(filterp);
   }
#endif

   // Get the pointer to the User Interface manager
   G4UImanager *UImanager = G4UImanager::GetUIpointer();

   if (macro != "") {
      // batch mode
      G4String command = "/control/execute ";
      UImanager->ApplyCommand(command + macro);
   } else {
      // interactive mode : define UI session
#ifdef G4UI_USE
      G4UIExecutive *ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute init_vis.mac");
#else
      UImanager->ApplyCommand("/control/execute init.mac");
#endif
      if (ui->IsGUI()) {
         UImanager->ApplyCommand("/control/execute icons.mac");
      }
      ui->SessionStart();
      delete ui;
#endif
   }

   // Job termination
   // Free the store: user actions, physics_list and detector_description are
   // owned and deleted by the run manager, so they should not be deleted
   // in the main() program !

#ifdef G4VIS_USE
   delete visManager;
#endif

   delete runManager;

   return 0;
}
