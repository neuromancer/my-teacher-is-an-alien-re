#ifndef SOUNDCOMMAND_H
#define SOUNDCOMMAND_H

struct GlyphRect;

// Base class for sound/render commands
// Objects derived from this are queued in ZBufferManager
// vtable at 0x461030 (full game), 0x431050 (demo)
// sdtor at 0x430980 (full game)
struct SoundCommand {
    /* Function start: 0x401540 */ /* DEMO ONLY - no full game match */
    virtual void Execute(GlyphRect* rect) {}
    /* Function start: 0x401550 */ /* DEMO ONLY - no full game match */
    virtual void Release(int flag) {}
    ~SoundCommand() {}
};

#endif // SOUNDCOMMAND_H
