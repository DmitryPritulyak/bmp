#include <stdio.h>
#include <windows.h>
#include "memmng.h"
#include "filesio.h"

#define GetAlign(Width, BitCount)\
(\
	(((Width*BitCount+31)/8)&~3) - (Width*BitCount+7)/8\
)

struct
{
	LONG Align;
	LONG Width;
	LONG Height;
} Sizes;

BYTE *pixels;
RGBQUAD *pallete;
BITMAPFILEHEADER *fileHeader;
BITMAPINFOHEADER *infoHeader;
BYTE *input;
BYTE *output;

void YCbCr()
{
	
}

void GetData()
{
	BYTE* l;
	BYTE *temp;
	short shift;
	short subBitC;
	LONG row, column;
	int oldAlign;
	LONG i = 0,
		j = 0;
	unsigned int RMask = 0, GMask = 0, BMask = 0;
	RGBTRIPLE rgb;
	fileHeader = input;
	infoHeader = input+14;
	if(fileHeader->bfType!=19778)
	{
		puts("Error: Not BMP file");
		exit(1);
	}
	if(fileHeader->bfSize>0x40000000)
	{
		puts("Error: File size is too large");
		exit(1);
	}
	Sizes.Width = infoHeader->biWidth;
	Sizes.Height = infoHeader->biHeight;
	Sizes.Align = GetAlign(infoHeader->biWidth, 24);
	switch(infoHeader->biBitCount)
	{
		case 1:
			pallete = (RGBQUAD*)MM_GetPtr(2*4, CAN_FREE);
			pallete = input+54;
			break;
		case 4:
			pallete = (RGBQUAD*)MM_GetPtr(16*4, CAN_FREE);
			pallete = input+54;
			break;
		case 8:
			pallete = (RGBQUAD*)MM_GetPtr(256*4, CAN_FREE);
			pallete = input+54;
			break;
		case 15:
		case 16:
			pallete = NULL;
			RMask = 0x7C00;
			GMask = 0x03E0;
			BMask = 0x001F;
			break;
		case 24:
			pallete = NULL;
			break;
		case 32:
			pallete = NULL;
			RMask = 0x00FF0000;
			GMask = 0x0000FF00;
			BMask = 0x000000FF;
			break;
	}
	BITMAPFILEHEADER *fh;
	BITMAPINFOHEADER *ih;
	pixels = (BYTE*)MM_GetPtr((Sizes.Width*3 + Sizes.Align) * Sizes.Height, CAN_FREE);
	output = (BYTE*)MM_GetPtr(54+(Sizes.Width*3 + Sizes.Align) * Sizes.Height, DONT_FREE);
	fh = output;
	ih = output + 14;
	fh->bfType = fileHeader->bfType;
	fh->bfSize = 54+(Sizes.Width*3 + Sizes.Align) * Sizes.Height;
	fh->bfOffBits = 54;
	ih->biSize = 40;
	ih->biWidth = Sizes.Width;
	ih->biHeight = Sizes.Height;
	ih->biBitCount = 24;
	ih->biPlanes = 1;
	temp = input + fileHeader->bfOffBits;
	pixels = output + 54;
	oldAlign = GetAlign(Sizes.Width, infoHeader->biBitCount);
	if(pallete)
	{
		switch(infoHeader->biBitCount)
		{
			case 1:
				for(i=0; i<Sizes.Height; i++)
				{
					for(j=0; j<Sizes.Width; j++)
					{
						int color = temp[i*((Sizes.Width+7)/8 + oldAlign) + (j+7)/8]>>(7-j%8)&1;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3] = pallete[color].rgbBlue;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3 + 1] = pallete[color].rgbGreen;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3 + 2] = pallete[color].rgbRed;
					}
				}
				break;
			case 4:
				for(i=0; i<Sizes.Height; i++)
				{
					for(j=0; j<Sizes.Width; j++)
					{
						int color = temp[i*((Sizes.Width+1)/2 + oldAlign) + j/2]>>(4*(1-j%2))&0xF;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3] = pallete[color].rgbBlue;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3 + 1] = pallete[color].rgbGreen;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3 + 2] = pallete[color].rgbRed;
					}
				}
				
				break;
			case 8:
				for(i=0;i<Sizes.Height;i++)
				{
					for(j=0; j<Sizes.Width; j++)
					{
						int color = temp[i*(Sizes.Width + oldAlign) + j];
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3] = pallete[color].rgbBlue;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3 + 1] = pallete[color].rgbGreen;
						pixels[i*(Sizes.Width*3+Sizes.Align)+j*3 + 2] = pallete[color].rgbRed;
					}
				}
				break;
		}
		
	}
	else
	{
		switch(infoHeader->biBitCount)
		{
			case 15:
			case 16:
				for(i=0; i<Sizes.Height; i++)
				{
					for(j=0; j<Sizes.Width; j++)
					{
						WORD color = (temp[i*(2*Sizes.Width + oldAlign) + 2*j]<<8) + (temp[i*(2*Sizes.Width + oldAlign) + 2*j + 1]);
						pixels[i*(3*Sizes.Width + Sizes.Align) + 3*j] = 8*(color & BMask)>>10;
						pixels[i*(3*Sizes.Width + Sizes.Align) + 3*j + 1] = 8*(color & GMask)>>5;
						pixels[i*(3*Sizes.Width + Sizes.Align) + 3*j + 2] = 8*(color & RMask);
					}
				}
				break;
			case 24:
				for(i=0; i<(Sizes.Width*3 + Sizes.Align) * Sizes.Height; i++)
					pixels[i] = temp[i];
				break;
		}
	}
	MM_SortMem();
}

void main(int argc, char *argv[])
{
	input = FileOpen(argv[1]);
	GetData();
	FileSave("test.bmp", output);
}