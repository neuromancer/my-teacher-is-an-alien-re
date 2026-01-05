#ifndef SOUNDCOMMAND_H
#define SOUNDCOMMAND_H

struct GlyphRect;

// Base class for sound/render commands
// Objects derived from this are queued in ZBufferManager
// vtable at 0x431518 and similar
struct SoundCommand {
    virtual ~SoundCommand() {}
    virtual void Execute(GlyphRect* rect) = 0;
};

#endif // SOUNDCOMMAND_H
