#include "filesio.c"

signed long IsFileExist(char* fileName);
BYTE *FileOpen(char* fileName);
void FileSave(char* fileName, BYTE *bytes);
