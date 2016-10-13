#include "ESActionInitialization.hpp"
#include "ESPrimaryGeneratorAction.hpp"
#include "ESRunAction.hpp"
#include "ESEventAction.hpp"


ESActionInitialization::ESActionInitialization()
   : G4VUserActionInitialization()
{}

ESActionInitialization::~ESActionInitialization()
{}

void ESActionInitialization::BuildForMaster() const
{
   SetUserAction(new ESRunAction());
}

void ESActionInitialization::Build() const
{
   SetUserAction(new ESPrimaryGeneratorAction());
   SetUserAction(new ESRunAction());
   SetUserAction(new ESEventAction());
}
