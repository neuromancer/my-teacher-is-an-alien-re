#ifndef ZBUFFERMANAGER_H
#define ZBUFFERMANAGER_H

// ZBufferManager class 
// Size: 0xAC (172 bytes)
// Global: DAT_0043698c
// Manages ZBuffer rendering and object lists
// Contains Timer at offset 0x84
class ZBufferManager {
public:
    ZBufferManager* Init();  // 0x41B760
    void Cleanup();          // 0x41B8E0
    
    char data[0xAC];         // placeholder for actual fields
};

#endif // ZBUFFERMANAGER_H
