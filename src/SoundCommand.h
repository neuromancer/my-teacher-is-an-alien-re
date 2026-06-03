#ifndef SOUNDCOMMAND_H
#define SOUNDCOMMAND_H

struct GlyphRect;

// Base class for sound/render commands
// Objects derived from this are queued in ZBufferManager
// Full game vtable: 0x461030, sdtor: 0x430980.
// Keep the destructor non-virtual: dumped queue drains reset to this base vtable before scalar delete.
struct SoundCommand {
    /* Function start: 0x401CC0 */
    virtual void Execute(GlyphRect* rect) {}
    /* Function start: 0x401CD0 */
    virtual void Release(int flag) {}
    ~SoundCommand() {}
};

#endif // SOUNDCOMMAND_H
