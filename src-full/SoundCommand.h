#ifndef SOUNDCOMMAND_H
#define SOUNDCOMMAND_H

struct GlyphRect;

// Base class for sound/render commands
// Objects derived from this are queued in ZBufferManager
// vtable at 0x431050 (base), 0x431528 (CommandType3), etc.
// Note: No virtual destructor - Execute is at vtable[0]
// Non-virtual destructor sets vtable back to base class before delete
struct SoundCommand {
    /* Function start: 0x401540 */ /* DEMO ONLY - no full game match */
    virtual void Execute(GlyphRect* rect) {}
    /* Function start: 0x401550 */ /* DEMO ONLY - no full game match */
    virtual void Release(int flag) {}
    ~SoundCommand() {}
};

#endif // SOUNDCOMMAND_H
