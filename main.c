//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h> 
#include "cbmp.h"
#include <time.h>
#define THRESHOLD 90

int cells = 0;
void *memcpy(void *dest, const void * src, size_t n);

//Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char gray_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char bw_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char eroded_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];




int coord[5000][2];

void setPixelColor(unsigned char arr[BMP_CHANNELS], int value) {
    arr[0] = value;
    arr[1] = value;
    arr[2] = value;
}


void convertToGray(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]){
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGHT; y++){
            int sum = 0;
            for (int c = 0; c < BMP_CHANNELS; c++){
                sum += input_image[x][y][c];
            }
            setPixelColor(output_image[x][y], sum / 3);
        }
    }
}

void convertToBW(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]){
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGHT; y++){
            if (input_image[x][y][0] > THRESHOLD){
                setPixelColor(output_image[x][y], 255);
            } else {
                setPixelColor(output_image[x][y], 0);
            }
        }
    }
}

void erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; x++){    
        for (int y = 0; y < BMP_HEIGHT; y++){
            if (input_image[x + 1][y] && !input_image[x + 1][y][0]){
                setPixelColor(output_image[x][y], 0);
            } else if (input_image[x - 1][y] && !input_image[x - 1][y][0]){
                setPixelColor(output_image[x][y], 0);
            } else if (input_image[x][y - 1] && !input_image[x][y - 1][0]){
                setPixelColor(output_image[x][y], 0);
            } else if (input_image[x][y + 1] && !input_image[x][y + 1][0]){
                setPixelColor(output_image[x][y], 0);
            } else {
                setPixelColor(output_image[x][y], input_image[x][y][0]);
            }
        }
    }
}

int frameCheck(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int x, int y)   {
    for (int i = -6; i <= 7; i++) {
        if(i == -6 || i == 7) {
            for (int j = -6; j <= 7; j++) {
                if(input_image[i + x][j + y][0]) {
                    return 1;
                }
            }
        } else {
            if(input_image[x + i][y - 6][0] || input_image[x + i][y + 7][0]) {
                return 1;
            }
        }
    }
    return 0;
}

void setAllBlack(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int x, int y){
    for (int i = -5; i <= 6; i++) {
        for (int j = -5; j <= 6; j++){
            setPixelColor(input_image[x + i][y + j], 0);
        }
    }
}
void setRed(unsigned char pixel[BMP_CHANNELS]){
    pixel[0] = 255;
    pixel[1] = 0;
    pixel[2] = 0;
    
}

void detectCells(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; x++){    
        for (int y = 0; y < BMP_HEIGHT; y++){
            if(input_image[x][y][0]) {
                if(!frameCheck(input_image, x, y)) {
                    setAllBlack(input_image, x, y);
                    cells++;
                    coord[cells][0] = x;
                    coord[cells][1] = y;
                }
            }
        }
    }
}

int checkWhite(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
    for(int i = 0; i < BMP_HEIGHT; i++) {
        for(int j = 0; j < BMP_WIDTH; j++) {
            if(input_image[i][j][0]) {
                return 1;
            }
        }
    }
    return 0;
} 


void drawCross(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], int coords[5000][2]) {
    for (int i = 0; i < cells; i++) {
        for (int j = -7; j <= 8; j++) {
            setRed(input_image[coords[i][0] + j][coords[i][1]]);
            setRed(input_image[coords[i][0]][coords[i][1] + j]);
            setRed(input_image[coords[i][0] + j][coords[i][1] + 1]);
            setRed(input_image[coords[i][0] + 1][coords[i][1] + j]);
        }
    }
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
    //printf("It works\n");

    //Load image from file
    read_bitmap(argv[1], input_image);


    convertToGray(input_image, gray_image);
    convertToBW(gray_image, bw_image);
    detectCells(bw_image);
    
    while(checkWhite(bw_image)) {
        erode(bw_image, eroded_image);
        detectCells(eroded_image);
        memcpy(&bw_image, &eroded_image, sizeof(eroded_image));
    }

    drawCross(input_image, coord);
    
    // Save image to file
    write_bitmap(input_image, argv[2]);

    //printf("Done! Found %d cells\n", cells);
   

    
    
    
    end = clock();
    cpu_time_used = end - start;
    printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
    return 0;
}

