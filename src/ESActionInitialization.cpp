#include "ESActionInitialization.hpp"
#include "ESPrimaryGeneratorAction.hpp"
#include "ESRunAction.hpp"
#include "ESEventAction.hpp"


ESActionInitialization::ESActionInitialization(G4bool useMonoEne, G4double beamEne)
   : G4VUserActionInitialization(),
     fUseMonoEne(useMonoEne),
     fBeamEne(beamEne)
{}

ESActionInitialization::~ESActionInitialization()
{}

void ESActionInitialization::BuildForMaster() const
{
   SetUserAction(new ESRunAction());
}

void ESActionInitialization::Build() const
{
   SetUserAction(new ESPrimaryGeneratorAction(fUseMonoEne, fBeamEne));
   SetUserAction(new ESRunAction());
   SetUserAction(new ESEventAction());
}
