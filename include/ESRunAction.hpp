#ifndef ESRunAction_h
#define ESRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"


class G4Run;

class ESRunAction: public G4UserRunAction
{
public:
   ESRunAction();
   virtual ~ESRunAction();

   virtual void BeginOfRunAction(const G4Run *);
   virtual void EndOfRunAction(const G4Run *);

private:

};

#endif
