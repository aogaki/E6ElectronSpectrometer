#ifndef ESActionInitialization_h
#define ESActionInitialization_h 1

#include "globals.hh"
#include "G4VUserActionInitialization.hh"

#include "ESPrimaryGeneratorAction.hpp"

class ESActionInitialization : public G4VUserActionInitialization
{
public:
   ESActionInitialization();
   virtual ~ESActionInitialization();

   virtual void BuildForMaster() const;
   virtual void Build() const;

private:

};

#endif
