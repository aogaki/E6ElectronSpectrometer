#ifndef ESExitSD_h
#define ESExitSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"

#include "ESExitHit.hpp"


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class ESExitSD: public G4VSensitiveDetector
{
public:
   ESExitSD(const G4String &name,
            const G4String &hitsCollectionName);
   virtual ~ESExitSD();

   virtual void Initialize(G4HCofThisEvent *hce);
   virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

private:
   ESExitHitsCollection *fHitsCollection;
};

#endif
