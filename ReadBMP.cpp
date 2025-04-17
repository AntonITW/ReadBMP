#include <stdio.h>
#include <string.h>
#include <malloc.h>

static unsigned char* read_bmp(const char* fname, int* _w, int* _h)
{
    unsigned char head[54]; //Размер заголовка BMP-файла

    FILE* f = fopen(fname, "rb");

    fread(head, 1, 54, f); //Считываем данные из заголовка

    int w = head[18] + (((int)head[19]) << 8) + (((int)head[20]) << 16) + (((int)head[21]) << 24); //Получаем ширину рисунка
    int h = head[22] + (((int)head[23]) << 8) + (((int)head[24]) << 16) + (((int)head[25]) << 24); //Высоту
    printf("width = %d; height = %d \n", w, h);

    int lineSize = (w / 8 + (w / 8) % 4); //Размер одной линии рисунка
    int fileSize = lineSize * h; //Размер всего рисунка

    unsigned char* img = (unsigned char*)malloc(w * h); //Выделяем память для рисунка
    unsigned char* data = (unsigned char*)malloc(fileSize); //Для считывания данных каждого пикселя

    fseek(f, 54, SEEK_SET); 
    fseek(f, 8, SEEK_CUR); //Пропускаем заголовок и цвета
    fread(data, 1, fileSize, f); //Считываем данные PixelStorage

    int i, j, k, l;
    for (j = 0, l = h - 1; j < h; j++, l--) {
        for (i = 0; i < w / 8; i++) {
            int fpos = j * lineSize + i;
            int pos = l * w + i * 8;
            for (k = 0; k < 8; k++)
                img[pos + (7 - k)] = (data[fpos] >> k) & 1; //Декодируем значения байтов пикселей и передаём их в img
        }
    }

    free(data);
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