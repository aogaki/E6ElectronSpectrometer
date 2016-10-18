#ifndef ESActionInitialization_h
#define ESActionInitialization_h 1

#include "globals.hh"
#include "G4VUserActionInitialization.hh"

#include "ESPrimaryGeneratorAction.hpp"

class ESActionInitialization : public G4VUserActionInitialization
{
public:
   ESActionInitialization(G4bool useMonoEne, G4double beamEne);
   virtual ~ESActionInitialization();

   virtual void BuildForMaster() const;
   virtual void Build() const;

private:
   G4bool fUseMonoEne;
   G4double fBeamEne;
   
};

#endif
