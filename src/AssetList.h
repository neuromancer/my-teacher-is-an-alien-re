#ifndef ASSETLIST_H
#define ASSETLIST_H

// AssetList class
// Note: DAT_00435a74/78/7c are Character*, not AssetList*
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
