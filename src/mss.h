#ifndef MSS_H
#define MSS_H

//
// General type definitions for portability
//

#ifndef C8
#define C8 char
#endif

#ifndef U8
#define U8 unsigned char
#endif

#ifndef S8
#define S8 signed char
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef S16
#define S16 signed short
#endif

#ifndef U32
#define U32 unsigned long
#endif

#ifndef S32
#define S32 signed long
#endif

#define FAR

//
// Handle to sample and driver being managed by pipeline filter
//

typedef S32 HSAMPLESTATE;
typedef S32 HDRIVERSTATE;

//
// Type definitions
//

struct _DIG_DRIVER;

struct _MDI_DRIVER;

typedef struct _DIG_DRIVER FAR * HDIGDRIVER;    // Handle to digital driver

typedef struct _MDI_DRIVER FAR * HMDIDRIVER;    // Handle to XMIDI driver

typedef struct _SAMPLE FAR * HSAMPLE;           // Handle to sample

typedef struct _SEQUENCE FAR * HSEQUENCE;       // Handle to sequence

typedef S32 HTIMER;                             // Handle to timer

#define SMP_DONE          0x0002    // Sample has finished playing, or has
                                    // never been started

#endif // MSS_H
