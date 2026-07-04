#ifndef AILSAMPLE_H
#define AILSAMPLE_H

// Minimal view of the internal MSS 3.x SAMPLE record. The game only ever
// inspects the sample data length at offset 0x0C (compared against
// Sample::m_size to verify the loaded sample still belongs to the object).
typedef struct AILSampleData {
    int field_0;   // 0x00
    int field_4;   // 0x04
    int field_8;   // 0x08
    int len;       // 0x0C - sample data length
} AILSampleData;

#endif // AILSAMPLE_H
