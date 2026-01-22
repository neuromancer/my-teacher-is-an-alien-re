#ifndef SOUNDCOMMAND_H
#define SOUNDCOMMAND_H

struct GlyphRect;

// Base class for sound/render commands
// Objects derived from this are queued in ZBufferManager
// vtable at 0x431050 (base), 0x431528 (CommandType3), etc.
// Note: No virtual destructor - Execute is at vtable[0]
// Non-virtual destructor sets vtable back to base class before delete
struct SoundCommand {
    virtual void Execute(GlyphRect* rect) = 0;
    ~SoundCommand() {}
};

#endif // SOUNDCOMMAND_H
