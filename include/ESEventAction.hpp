#ifndef ESEventAction_h
#define ESEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"


class ESEventAction : public G4UserEventAction
{
public:
   ESEventAction();
   virtual ~ESEventAction();

   virtual void BeginOfEventAction(const G4Event *);
   virtual void EndOfEventAction(const G4Event *);

private:
   G4int fHitsCollectionID;
};

#endif
