#include "GameOutcome.h"

// GameOutcome is a trivial data class in the full game.
// The demo had PlayOutcomeVideo() playing rat1\lose.smk / rat1\win.smk,
// but the full game uses a different outcome system (no matching strings).
//
// Constructor (outcome = 0) and destructor (empty) are trivially inlined
// by the compiler and don't appear as separate functions in the full game binary.
