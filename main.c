//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp
#include <stdlib.h>
#include <stdio.h> 
#include "cbmp.h"
#include <time.h>
#define THRESHOLD 95

int cells = 0;
int coord[1000][2];

unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char image1[BMP_WIDTH][BMP_HEIGHT];
unsigned char image2[BMP_WIDTH][BMP_HEIGHT];

unsigned char (*p1)[BMP_HEIGHT] = image1;
unsigned char (*p2)[BMP_HEIGHT] = image2;

int erodeMatrix[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
int bigErodeMatrix[20][2] = {
    {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, 
    {-1, -2}, {-1, -1}, {-1, 1}, {-1, 2},
    {0, -2},  {0, 2}, 
    {1, -2}, {1, -1}, {1, 1}, {1, 2},
    {2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2}
};


void setPixelColor(unsigned char arr[BMP_CHANNELS], int value) {
    arr[0] = value;
    arr[1] = value;
    arr[2] = value;
}

void convertToBW(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char (*p1)[BMP_WIDTH]){
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGHT; y++){
            int sum = 0;
            for (int c = 0; c < BMP_CHANNELS; c++){
                sum += input_image[x][y][c];
            }
            p1[x][y] = sum / 3 > THRESHOLD ? 255 : 0;
        }
    }
}

void convertToImage(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char (*p1)[BMP_WIDTH]){
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGHT; y++){
            input_image[x][y][0] = p1[x][y];
            input_image[x][y][1] = p1[x][y];
            input_image[x][y][2] = p1[x][y];
        }
    }
}





void erode(unsigned char (*p1)[BMP_WIDTH], unsigned char (*p2)[BMP_WIDTH], int erodeMatrix[4][2], int bigErodeMatrix[20][2]) {
    for (int x = 0; x < BMP_WIDTH; x++){    
        for (int y = 0; y < BMP_HEIGHT; y++){
            int countBlack = 0;
            int found = 0;
            for (int i = 0; i < 4; i++){
                if (
                    x + erodeMatrix[i][0] >= 0 && x + erodeMatrix[i][0] < BMP_HEIGHT && 
                    y + erodeMatrix[i][1] >= 0 && y + erodeMatrix[i][1] < BMP_HEIGHT && 
                    p1[x + erodeMatrix[i][0]][y + erodeMatrix[i][1]] == 0
                ){
                    found = 1;
                    countBlack++;
                    p2[x][y] = 0;
                } 
            } 
            if (!found) {
                p2[x][y] = p1[x][y];
            }
            for (int i = 0; i < 20; i++){
                if (
                    x + bigErodeMatrix[i][0] >= 0 && x + bigErodeMatrix[i][0] < BMP_HEIGHT && 
                    y + bigErodeMatrix[i][1] >= 0 && y + bigErodeMatrix[i][1] < BMP_HEIGHT && 
                    p1[x + bigErodeMatrix[i][0]][y + bigErodeMatrix[i][1]] == 0
                ){
                    countBlack++;
                } 
            }
            if (countBlack <= 11 && p1[x][y] == 0) {
                for (int i = -1; i <= 1; i++){
                    for (int j = -1; j <= 1; j++){
                        if (
                        x + bigErodeMatrix[i][0] >= 0 && x + bigErodeMatrix[i][0] < BMP_HEIGHT && 
                        y + bigErodeMatrix[i][1] >= 0 && y + bigErodeMatrix[i][1] < BMP_HEIGHT) {
                            p2[x + bigErodeMatrix[i][0]][y + bigErodeMatrix[i][1]] = 0;
                        }
                    }   
                }
            }
        }
    }
}

int frameCheck(unsigned char (*p1)[BMP_WIDTH], int x, int y)   {
    for (int i = -8; i <= 9; i++) {
        if(i == -8 || i == 9) {
            for (int j = -8; j <= 9; j++) {
                if(i + x >= 0 && i + x < BMP_WIDTH && j + y >= 0 && j + y < BMP_WIDTH && p1[i + x][j + y]) {
                    return 1;
                }
            }
        } else if(i + x >= 0 && i + x < BMP_WIDTH && p1[x + i][y - 8] || p1[x + i][y + 9]) {
            return 1;
        }
    }
    return 0;
}

void setAllBlack(unsigned char (*p1)[BMP_WIDTH], int x, int y){
    for (int i = -7; i <= 8; i++) {
        for (int j = -7; j <= 8; j++){
            p1[x + i][y + j] = 0;
        }
    }
}

void setRed(unsigned char pixel[BMP_CHANNELS]){
    pixel[0] = 255;
    pixel[1] = 0;
    pixel[2] = 0;
    
}

void detectCells(unsigned char (*p1)[BMP_WIDTH]) {
    for (int x = 0; x < BMP_WIDTH; x++){    
        for (int y = 0; y < BMP_HEIGHT; y++){
            if(p1[x][y]) {
                if(!frameCheck(p1, x, y)) {
                    setAllBlack(p1, x, y);
                    coord[cells][0] = x;
                    coord[cells][1] = y;
                    cells++;
                }
            }
        }
    }
}

int checkWhite(unsigned char (*p1)[BMP_WIDTH]) {
    for(int i = 0; i < BMP_HEIGHT; i++) {
        for(int j = 0; j < BMP_WIDTH; j++) {
            if(p1[i][j]) {
                return 1;
            }
        }
    }
    return 0;
} 


void drawCross(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int coords[1000][2]) {
    for (int i = 0; i < cells; i++) {
        for (int j = -7; j <= 8; j++) {
            if (coords[i][0] + j >= 0 && coords[i][0] + j < BMP_HEIGHT){
                setRed(input_image[coords[i][0] + j][coords[i][1]]);
            } 
            if (coords[i][1] + j >= 0 && coords[i][1] + j < BMP_HEIGHT){
                setRed(input_image[coords[i][0]][coords[i][1] + j]);
            }
            if (coords[i][0] + j >= 0 && coords[i][0] + j < BMP_WIDTH && coords[i][1] + 1 < BMP_WIDTH){
                setRed(input_image[coords[i][0] + j][coords[i][1] + 1]);
            }
            if (coords[i][1] + j >= 0 && coords[i][1] + j < BMP_WIDTH && coords[i][0] + 1 < BMP_WIDTH){
                setRed(input_image[coords[i][0] + 1][coords[i][1] + j]);
            }   
        }
    }
}

void swap(unsigned char (**p1)[BMP_HEIGHT], unsigned char (**p2)[BMP_HEIGHT]) {
    unsigned char (*p3)[BMP_HEIGHT] = *p1;
    *p1 = *p2;
    *p2 = p3;
}

// Main function
int main(int argc, char** argv){
    if (argc != 3){
      fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
      exit(1);
    }
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    //Load image from file
    read_bitmap(argv[1], input_image);
    convertToBW(input_image, p1);
    //convertToImage(input_image, p1);
    detectCells(p1);
    
    int i=0;
    while(checkWhite(p1)) {
        erode(p1, p2, erodeMatrix, bigErodeMatrix);
        swap(&p1, &p2);
        detectCells(p1);
        convertToImage(output_image, p1);
        char string[100];
        snprintf(string, 100, "./pictures/picture%d.bmp", i);
        write_bitmap(output_image, string);
        i++;
    }
    drawCross(input_image, coord);
    
    // Save image to file
    write_bitmap(input_image, argv[2]);
    printf("Done! Found %d cells\n", cells);
    
    for (int i = 0; i < cells; i++){
        //printf("(%d, %d)\t ", coord[i][0], coord[i][1]);
    }
    
    end = clock();
    cpu_time_used = end - start;
    printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
    return 0;
}

