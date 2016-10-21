#ifndef ESConstants_h
#define ESConstants_h 1

#include <globals.hh>

const G4double kSourceZPos = -3000.*mm;
const G4double kSourceToWindow = 2850.*mm;
const G4double kSourceToColliAir = 2900.*mm;
const G4double kSourceToColliVac = 2500.*mm;

// changing collimator setting
enum class ColliState
{
   No,
   InAir,
   InVac,
};

#endif
