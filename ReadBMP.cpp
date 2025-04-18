#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>


unsigned short Read2(FILE* f)
{
    unsigned char buf[2];
    unsigned short result = 0;
    int i;

    fread(buf, 1, 2, f);
    for (i = 1; i >= 0; i--) {
        result = (result << 8) | (unsigned short)buf[i];
    }

    return result;
}


unsigned int Read4(FILE* f)
{
    unsigned char buf[4];
    unsigned int result = 0;
    int i;

    fread(buf, 1, 4, f);
    for (i = 3; i >= 0; i--) {
        result = (result << 8) | (unsigned int)buf[i];
    }

    return result;
}

struct bmp_file_header
{
    char type[3]; // 2 + null char 
    unsigned int size; //4
    unsigned short reserved1; //2
    unsigned short reserved2; //2
    unsigned long offBits; //4 

} ;

struct bmp_info_header
{
    unsigned int size; //2
    long width; //4
    long height; //4
    unsigned short planes; //2
    unsigned short bitCount; //2
    unsigned int compression; //4
    unsigned int sizeImage; //4
    long ypixPerMeter; //4
    long xpixPerMeter; //4
    unsigned long clrUsed; //4
    unsigned long clrImportant; //4
};

union BMP {
    struct bmp_file_header fh;
    struct bmp_info_header ih;
    unsigned char *pixeldata;
};

static unsigned char* read_bmp(const char* fname, int* _w, int* _h)
{

    union BMP bmp;

    FILE* f = fopen(fname, "rb");

    fread(&bmp.fh.type, 1, 2, f);
    printf("Type = %1.2s \n", bmp.fh.type);
    bmp.fh.size = (unsigned int)Read4(f);
    printf("Size = %d \n", bmp.fh.size);
    fread(&bmp.fh.reserved1, 2, 1, f);
    fread(&bmp.fh.reserved2, 2, 1, f);
    bmp.fh.offBits = (unsigned long)Read4(f);
    printf("Offset = %d \n", bmp.fh.offBits);

    bmp.ih.size = (unsigned int)Read4(f);
    printf("Size = %ld \n", bmp.ih.size);
    bmp.ih.width = (int)Read4(f);
    printf("Width = %ld \n", bmp.ih.width);
    bmp.ih.height = (int)Read4(f);
    printf("Height = %d \n", bmp.ih.height);
    bmp.ih.planes = (unsigned short)Read2(f);
    printf("Planes = %d \n", bmp.ih.planes);
    bmp.ih.bitCount = (unsigned short)Read2(f);
    printf("Bit count = %d \n", bmp.ih.bitCount);
    bmp.ih.compression = (unsigned int)Read4(f);
    printf("Compression = %d \n", bmp.ih.compression);
    bmp.ih.sizeImage = (unsigned int)Read4(f);
    printf("Image size = %d \n", bmp.ih.sizeImage);
    bmp.ih.xpixPerMeter = (int)Read4(f);
    //printf("X pixels pm = %ld \n", bmp.ih.xpixPerMeter);
    bmp.ih.ypixPerMeter = (int)Read4(f);
    //printf("Y pixels PM = %ld \n", bmp.ih.ypixPerMeter);
    bmp.ih.clrUsed = (unsigned int)Read4(f);
    //printf("Used colors = %d \n", bmp.ih.clrUsed);
    bmp.ih.clrImportant = (unsigned int)Read4(f);

    int w = bmp.ih.width;
    int h = bmp.ih.height;
    
    int lineSize = (w / 8 + (w / 8) % 4); //Размер одной линии рисунка
    int fileSize = lineSize * h; //Размер всего рисунка

    unsigned char* img = (unsigned char*)malloc(w * h); //Выделяем память для рисунка
    unsigned char* data = (unsigned char*)malloc(fileSize); //Для считывания данных каждого пикселя
    
    fseek(f, 54, SEEK_SET);
    fseek(f, 8, SEEK_CUR); //Пропускаем заголовок и цвета
    fread(data, 1, fileSize, f); //Считываем данные PixelStorage

    bmp.pixeldata = data;
    int i, j, k, l;
    for (j = 0, l = h - 1; j < h; j++, l--) {
        for (i = 0; i < w / 8; i++) {
            int fpos = j * lineSize + i;
            int pos = l * w + i * 8;
            for (k = 0; k < 8; k++)
                img[pos + (7 - k)] = (data[fpos] >> k) & 1; //Декодируем значения байтов пикселей и передаём их в img
        }
    }
    *_w = w; *_h = h; //Возвращаем ширину и длину
    return img;
}

int main()
{
    int w, h;
    unsigned char* img = read_bmp("F:\\cat.bmp", &w, &h);

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
            printf("%c ", img[j * w + i] ? ' ' : '*');

        printf("\n");
    }

    return 0;
}
