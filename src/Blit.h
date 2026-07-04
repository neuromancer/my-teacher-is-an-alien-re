#ifndef BLIT_H
#define BLIT_H

int __cdecl BlitBufferOpaque(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, unsigned int srcHandle, unsigned int destHandle);
int __cdecl BlitBufferTransparent(int srcX1, int srcX2, int srcY1, int srcY2, int destX, int destY, unsigned int srcHandle, unsigned int destHandle);

#endif // BLIT_H
