#ifndef ESEventAction_h
#define ESEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "ESExitHit.hpp"

class ESEventAction : public G4UserEventAction
{
public:
   ESEventAction();
   virtual ~ESEventAction();

   virtual void BeginOfEventAction(const G4Event *);
   virtual void EndOfEventAction(const G4Event *);

private:
   ESExitHitsCollection *GetHitsCollection(G4int hcID, const G4Event *event) const;
   
   G4int fHitsCollectionID;
};

#endif
