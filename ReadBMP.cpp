#include <stdio.h>
#include <string.h>
#include <malloc.h>

static unsigned char* read_bmp(const char* fname, int* _w, int* _h)
{
    unsigned char head[54]; //������ ��������� BMP-�����

    FILE* f = fopen(fname, "rb");

    fread(head, 1, 54, f); //��������� ������ �� ���������

    int w = head[18] + (((int)head[19]) << 8) + (((int)head[20]) << 16) + (((int)head[21]) << 24); //�������� ������ �������
    int h = head[22] + (((int)head[23]) << 8) + (((int)head[24]) << 16) + (((int)head[25]) << 24); //������
    printf("width = %d; height = %d \n", w, h);

    int lineSize = (w / 8 + (w / 8) % 4); //������ ����� ����� �������
    int fileSize = lineSize * h; //������ ����� �������

    unsigned char* img = (unsigned char*)malloc(w * h); //�������� ������ ��� �������
    unsigned char* data = (unsigned char*)malloc(fileSize); //��� ���������� ������ ������� �������

    fseek(f, 54, SEEK_SET); 
    fseek(f, 8, SEEK_CUR); //���������� ��������� � �����
    fread(data, 1, fileSize, f); //��������� ������ PixelStorage

    int i, j, k, l;
    for (j = 0, l = h - 1; j < h; j++, l--) {
        for (i = 0; i < w / 8; i++) {
            int fpos = j * lineSize + i;
            int pos = l * w + i * 8;
            for (k = 0; k < 8; k++)
                img[pos + (7 - k)] = (data[fpos] >> k) & 1; //���������� �������� ������ �������� � ������� �� � img
        }
    }

    free(data);
    *_w = w; *_h = h; //���������� ������ � �����
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