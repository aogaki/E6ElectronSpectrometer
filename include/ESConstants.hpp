#ifndef ESConstants_h
#define ESConstants_h 1


#include <G4SystemOfUnits.hh>
#include <globals.hh>

const G4double kSourceZPos = -3000.*mm;
const G4double kSourceToWindow = 2850.*mm;
const G4double kSourceToColliAir = 2900.*mm;
const G4double kSourceToColliVac = 2500.*mm;
const G4double kMagnetGap = 20.*mm;

// changing beam
enum class BeamState
{
   Normal,
   Wide,
   Mono,
   MonoZero,
   MonoWide,
   Reference,
   ZeroAng,
};

// changing collimator position
enum class ColliState
{
   No,
   InAir,
   InVac,
};

// changing mirror position
enum class MirrorState
{
   No,
   OnBeam,
   OffBeam,
};

// changing detector setting
enum class DetState
{
   Real,
   Horizontal,
   Vertical,
   NoFront,
};

#endif
