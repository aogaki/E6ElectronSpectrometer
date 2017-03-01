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
//#include "G4BlineTracer.hh"
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
             << " -z zero divergence angle beam \n"
             << " -v only vacuum with magnetic field \n"
             << " -c [0: no collimator, 1: collimator in air, 2: collimator in vacuum] \n"
             << " -p [0: no mirror, 1: primary beam hit mirror, 2: not hit] \n"
             << " -d [0: real setup of LANEX, 1: large horizontal plane, 2: large vertical plane] \n"
             << " -r Making reference data for callibration of LANEX position\n"
             << " -w Using wide beam (3.5mrad)"
             << G4endl;
   }
}

unsigned int GetRandomSeed()
{
   // Using /dev/urandom for generating random number.
   // If it is not, I have to think solution.
   // Using std::random_device of C++11 is better
   unsigned int seed;
   std::ifstream file("/dev/urandom", std::ios::binary);
   if (file.is_open()) {
      char *memblock;
      int size = sizeof(int);
      memblock = new char[size];
      file.read(memblock, size);
      file.close();
      seed = *reinterpret_cast<unsigned int *>(memblock);
      delete[] memblock;
   } else {
      seed = 0;
   }

   return seed;
}

int main(int argc, char **argv)
{
   // Too much arguments...
   G4String macro = "";
   G4bool showAll = false;
   G4bool vacFlag = false;
   G4double beamEne = 0.;
   G4bool zeroAngFlag = false;
   G4bool wideFlag = false;
   ColliState colliState = ColliState::No;
   MirrorState mirrorState = MirrorState::No;
   DetState detState = DetState::Real;
   BeamState beamState = BeamState::Normal;
   
   for (G4int i = 1; i < argc; i++) {
      if (G4String(argv[i]) == "-m")
         macro = argv[++i];
      else if (G4String(argv[i]) == "-a")
         showAll = true;
      else if (G4String(argv[i]) == "-w"){
         beamState = BeamState::Wide;
         wideFlag = true;
      }
      else if (G4String(argv[i]) == "-e"){
         beamState = BeamState::Mono;
         beamEne = std::stod(argv[++i]);
      }
      else if (G4String(argv[i]) == "-z"){
         beamState = BeamState::ZeroAng;
         zeroAngFlag = true;
      }
      else if (G4String(argv[i]) == "-v")
         vacFlag = true;
      else if (G4String(argv[i]) == "-r"){
         vacFlag = true;
         beamState = BeamState::Reference;
      }
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
      else if (G4String(argv[i]) == "-p"){
         G4int mirrorFlag = atoi(argv[++i]);
         if(mirrorFlag == 0) mirrorState = MirrorState::No;
         else if(mirrorFlag == 1) mirrorState = MirrorState::OnBeam;
         else if(mirrorFlag == 2) mirrorState = MirrorState::OffBeam;
         else {
            PrintUsage();
            return 1;
         }
      }
      else if (G4String(argv[i]) == "-d"){
         G4int detFlag = atoi(argv[++i]);
         if(detFlag == 0) detState = DetState::Real;
         else if(detFlag == 1) detState = DetState::Horizontal;
         else if(detFlag == 2) detState = DetState::Vertical;
         else if(detFlag == 3) detState = DetState::NoFront;
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

   if(zeroAngFlag == true && beamEne > 0.) beamState = BeamState::MonoZero;
   if(wideFlag == true && beamEne > 0.) beamState = BeamState::MonoWide;
   
   // Choose the Random engine
   // Need both?
   unsigned int seed = GetRandomSeed();
   if (seed == 0) seed = time(0);
   G4cout << "\nseed = " << seed << G4endl;
   CLHEP::HepRandom::setTheEngine(new CLHEP::MTwistEngine(seed));
   G4Random::setTheEngine(new CLHEP::MTwistEngine(seed));

   // Construct the default run manager
#ifdef G4MULTITHREADED
   G4MTRunManager *runManager = new G4MTRunManager();
   if(beamState == BeamState::Reference) runManager->SetNumberOfThreads(1);
   else runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
   //runManager->SetNumberOfThreads(1);
#else
   G4RunManager *runManager = new G4RunManager();
#endif

   // Set mandatory initialization classes
   //
   // Detector construction
   runManager->SetUserInitialization(new ESDetectorConstruction(mirrorState, colliState, detState, vacFlag));

   // Physics list
   //G4VModularPhysicsList *physicsList = new FTFP_BERT();
   //G4VModularPhysicsList *physicsList = new QGSP_BERT_HP();
   //G4VModularPhysicsList *physicsList = new QGSP_BIC_HP();
   G4VModularPhysicsList *physicsList = new Shielding();
   //G4VModularPhysicsList *physicsList = new ESPhysicsList();
   physicsList->SetVerboseLevel(0);
   runManager->SetUserInitialization(physicsList);

   // Primary generator action and User action intialization
   runManager->SetUserInitialization(new ESActionInitialization(beamState, beamEne));

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
