#include "ESActionInitialization.hpp"
#include "ESPrimaryGeneratorAction.hpp"
#include "ESRunAction.hpp"
#include "ESEventAction.hpp"


ESActionInitialization::ESActionInitialization(G4bool useMonoEne, G4double beamEne, G4bool useZeroAng)
   : G4VUserActionInitialization(),
     fUseMonoEne(useMonoEne),
     fBeamEne(beamEne),
     fUseZeroAng(useZeroAng)
{}

ESActionInitialization::~ESActionInitialization()
{}

void ESActionInitialization::BuildForMaster() const
{
   SetUserAction(new ESRunAction());
}

void ESActionInitialization::Build() const
{
   SetUserAction(new ESPrimaryGeneratorAction(fUseMonoEne, fBeamEne, fUseZeroAng));
   SetUserAction(new ESRunAction());
   SetUserAction(new ESEventAction());
}
