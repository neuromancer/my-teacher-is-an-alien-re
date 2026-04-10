#ifndef ASSETLIST_H
#define ASSETLIST_H

// AssetList class
// Note: g_PeterCharacter_00435a74/78/7c are Character*, not AssetList*
// Contains linked list of assets
class AssetList {
public:
    ~AssetList();
    void Cleanup();  // 0x404230
    
    void* field_0x00;
    void* field_0x04;
    void* field_0x08;
    void* field_0x0c;
    void* field_0x10;
    void* field_0x14;
    void* field_0x18;
    void* field_0x1c;
    void* field_0x20;
    void* field_0x24;
    void* field_0x28;
    void* field_0x2c;
    void* field_0x30;
    void* field_0x34;
    void* field_0x38;  // Asset list head
    void* field_0x3c;  // Asset list head 2
};

#endif // ASSETLIST_H
