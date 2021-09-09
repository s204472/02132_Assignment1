//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h> 
#include "cbmp.h"

#define THRESHOLD 90

int cells = 0;
void *memcpy(void *dest, const void * src, size_t n);



//Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char bw_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
int hasWhite = 1;

void setPixelColor(unsigned char arr[BMP_CHANNELS], int value) {
    arr[0] = value;
    arr[1] = value;
    arr[2] = value;
}


void convertToGray(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGTH; y++){
            int sum = 0;
            for (int c = 0; c < BMP_CHANNELS; c++){
                sum += input_image[x][y][c];
            }
            setPixelColor(output_image[x][y], sum / 3);
        }
    }
}

void convertToBW(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGTH; y++){
            if (input_image[x][y][0] > THRESHOLD){
                setPixelColor(output_image[x][y], 255);
            } else {
                setPixelColor(output_image[x][y], 0);
            }
        }
    }
}

void erode(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; x++){    
        for (int y = 0; y < BMP_HEIGTH; y++){
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

int frameCheck(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], x, y)   {
    /* (i=-6; )

    input_image[]*/
    
}

void detectCells(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
    for (int x = 0; x < BMP_WIDTH; x++){    
        for (int y = 0; y < BMP_HEIGTH; y++){
            if(input_image[x][y][0]) {
                if(frameCheck(input_image, x, y)) {
                    setAllePixelsToBlack
                } else {
                    //move on to the next pixel
                }
            }
        }
    }
}


// Main function
int main(int argc, char** argv){
    if (argc != 3){
      fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
      exit(1);
    }

    printf("It works\n");

    //Load image from file
    read_bitmap(argv[1], input_image);


    convertToGray(input_image, gray_image);
    convertToBW(gray_image, bw_image);
    detectCells(bw_image);
    
    while(hasWhite) {
        erode(bw_image, eroded_image);
        detectCells(eroded_image);
        memcpy(&bw_image, &eroded_image, sizeof(eroded_image));
           
    }
    


    
    

    //Save image to file
    write_bitmap(eroded_image, argv[2]);

    printf("Done!\n");
    return 0;
}

