#ifndef CDDATA_H
#define CDDATA_H

class CDData {
public:
  char baseDir[0x80];          // offset 0x00 - current working directory
  char cdFolder[0x40];         // offset 0x80 - CD folder name (e.g., "cddata")
  char pathBuffer[0x105];      // offset 0xc0 - path buffer: [0]=drive, [5]=srcPath, [0x85]=destPath
  char dataFolder[0x20];       // offset 0x1c5 - data subfolder name (e.g., "DATA")

  CDData(char *param_1, char *param_2);
  CDData(char *baseDir, const char *cdId, const char *errorMsg);
  int CheckFileOnDrive(int drive_letter);
  int ChangeToDriveDirectory(int drive_letter);
  int ChangeDirectory(unsigned char *path);
  int ResolvePath(char* param_1);  // 0x421F90
};

#endif
