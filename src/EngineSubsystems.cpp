#include "EngineSubsystems.h"
#include "globals.h"

// ============================================================================
// Target implementation (Stubs for linking)
// ============================================================================

Target::Target() : Parser() {}
Target::~Target() {}
void Target::Init(char* line) {}

// ============================================================================
// Viewport implementation (Stubs for linking)
// ============================================================================

void Viewport::SetDimensions(int w, int h) {}
void Viewport::SetAnchor(int x, int y) {}
void Viewport::SetDimensions2(int w, int h) {}
void Viewport::SetCenter() {}
void Viewport::FUN_00412ce0(int x) {}
void Viewport::FUN_00412d00(int y) {}
