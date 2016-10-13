#include <iomanip>

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "ESExitHit.hpp"


G4ThreadLocal G4Allocator<ESExitHit> *ESExitHitAllocator = 0;


ESExitHit::ESExitHit()
   : G4VHit(),
     fTrackID(0),
     fPDGCode(0),
     fKineticEnergy(0),
     fPosition(0),
     fMomentum(0),
     fIsLastStep(0),
     fVolumeName("")
{}

ESExitHit::~ESExitHit()
{}

ESExitHit::ESExitHit(const ESExitHit & /*right*/)
   : G4VHit()
{}

const ESExitHit &
ESExitHit::operator=(const ESExitHit & /*right*/)
{
   return *this;
}

int ESExitHit::operator==(const ESExitHit & /*right*/) const
{
   return 0;
}
