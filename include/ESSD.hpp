#ifndef ESSD_h
#define ESSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"

#include "ESHit.hpp"


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class ESSD: public G4VSensitiveDetector
{
public:
   ESSD(const G4String &name,
            const G4String &hitsCollectionName);
   virtual ~ESSD();

   virtual void Initialize(G4HCofThisEvent *hce);
   virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

private:
   ESHitsCollection *fHitsCollection;
};

#endif
