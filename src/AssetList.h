#ifndef ASSETLIST_H
#define ASSETLIST_H

// AssetList class  
// Used for DAT_00435a74, DAT_00435a78, DAT_00435a7c
// Contains linked list of assets
class AssetList {
public:
    ~AssetList();
    void Cleanup();  // 0x404230
    
    void* field_0x00;
    void* field_0x04;
    void* field_0x08;
    // ... more fields
    void* field_0x38;  // Asset list head
    void* field_0x3c;  // Asset list head 2
};

#endif // ASSETLIST_H
