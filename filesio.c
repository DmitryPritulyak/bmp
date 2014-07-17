#include <sys/stat.h>

signed long IsFileExist(char* fileName)
{
	struct stat buf;
	if(stat(fileName, &buf))
		return -1;
	return buf.st_size;
}

BYTE *FileOpen(char* fileName)
{
	long length;
	BYTE *bytes;
	
	if ((length = IsFileExist(fileName))<0)
	{
		printf("Error loading file '%s'!\n", fileName);
		exit(1);
	}
	bytes = (BYTE*)MM_GetPtr(length, CAN_FREE);
	if(!bytes)
	{
		puts("FileOpen: Out of memory!");
		exit(1);
	}
	FILE *f = fopen(fileName, "rb");
	fread(bytes, length, 1, f);
	fclose(f);
	return bytes;
}

void FileSave(char* fileName, BYTE *bytes)
{
	long length;
	FILE *f;
	struct memblock *temp;
	
	if(*bytes == NULL)
	{
		puts("FileSave: Out of memory!");
		exit(1);
	}
	
	temp = (struct memblock*)MM_FindPtr(bytes);
	length = temp->length;
	
	if (!(f = fopen(fileName, "wb")))
	{
		printf("Error loading file '%s'!\n", fileName);
		exit(1);
	}
	fwrite(bytes, length, 1, f);
	fclose(f);
}