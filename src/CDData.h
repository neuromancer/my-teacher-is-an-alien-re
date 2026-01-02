#ifndef CDDATA_H
#define CDDATA_H

class CDData {
public:
  char data[0x80];
  char field_0x80[0x40];       // offset 0x80
  char field_0xc0[0x105];      // offset 0xc0 (drive/path storage)
  char field_0x1c5[0x20];      // offset 0x1c5

  CDData(char *param_1, char *param_2);
  int CheckFileOnDrive(int drive_letter);
  int ChangeToDriveDirectory(int drive_letter);
  int ChangeDirectory(unsigned char *path);
};

#endif
