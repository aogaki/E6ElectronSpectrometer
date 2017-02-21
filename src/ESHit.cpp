#include <iomanip>

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "ESHit.hpp"


G4ThreadLocal G4Allocator<ESHit> *ESHitAllocator = 0;


ESHit::ESHit()
   : G4VHit(),
     fTrackID(0),
     fPDGCode(0),
     fKineticEnergy(0),
     fIncidentEnergy(0),
     fDepositEnergy(0),
     fPosition(0),
     fMomentum(0),
     fIsLastStep(0),
     fVolumeName(""),
     fVertexName("")
{}

ESHit::~ESHit()
{}

ESHit::ESHit(const ESHit & /*right*/)
   : G4VHit()
{}

const ESHit &
ESHit::operator=(const ESHit & /*right*/)
{
   return *this;
}

int ESHit::operator==(const ESHit & /*right*/) const
{
   return 0;
}
