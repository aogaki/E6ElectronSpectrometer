#ifndef ESActionInitialization_h
#define ESActionInitialization_h 1

#include "globals.hh"
#include "G4VUserActionInitialization.hh"

#include "ESConstants.hpp"
#include "ESPrimaryGeneratorAction.hpp"

class ESActionInitialization : public G4VUserActionInitialization
{
public:
   ESActionInitialization(BeamState beam, G4double beamEne);
   virtual ~ESActionInitialization();

   virtual void BuildForMaster() const;
   virtual void Build() const;

private:
   BeamState fBeamState;
   G4double fBeamEne;
   
};

#endif
