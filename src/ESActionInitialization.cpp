#include "ESActionInitialization.hpp"
#include "ESPrimaryGeneratorAction.hpp"
#include "ESRunAction.hpp"
#include "ESEventAction.hpp"


ESActionInitialization::ESActionInitialization(BeamState beam, G4double beamEne)
   : G4VUserActionInitialization(),
     fBeamState(beam),
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
   SetUserAction(new ESPrimaryGeneratorAction(fBeamState, fBeamEne));
   SetUserAction(new ESRunAction());
   SetUserAction(new ESEventAction());
}
