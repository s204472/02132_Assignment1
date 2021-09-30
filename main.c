#include <stdlib.h>
#include <stdio.h> 
#include "cbmp.h"
#include <time.h>
#define THRESHOLD 95
#define FRAMESIZE 9
#define BMP_SIZE 950

int cells = 0;
int coord[1000][2];

unsigned char input_image[BMP_SIZE][BMP_SIZE][BMP_CHANNELS];
unsigned char output_image[BMP_SIZE][BMP_SIZE][BMP_CHANNELS];
unsigned char image1[BMP_SIZE][BMP_SIZE];
unsigned char image2[BMP_SIZE][BMP_SIZE];

unsigned char (*p1)[BMP_SIZE] = image1;
unsigned char (*p2)[BMP_SIZE] = image2;

// Erode matrices for the different erode steps. The erodeMatrix erodes only neighbouring squares, 
// while bigErodeMatrix erodes based on amount of white around cells.
int erodeMatrix[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
int bigErodeMatrix[20][2] = {
    {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, 
    {-1, -2}, {-1, -1}, {-1, 1}, {-1, 2},
    {0, -2},  {0, 2}, 
    {1, -2}, {1, -1}, {1, 1}, {1, 2},
    {2, -2}, {2, -1}, {2, 0}, {2, 1}, {2, 2}
};

// Convert color image to black and white image
void convertToBW(unsigned char input_image[BMP_SIZE][BMP_SIZE][BMP_CHANNELS], unsigned char (*p1)[BMP_SIZE]){
    for (int x = 0; x < BMP_SIZE; x++){
        for (int y = 0; y < BMP_SIZE; y++){
            int sum = 0;
            for (int c = 0; c < BMP_CHANNELS; c++){
                sum += input_image[x][y][c];
            }
            p1[x][y] = sum / 3 > THRESHOLD ? 255 : 0;
        }
    }
}

// Prints the eroded images during computation
void convertToImage(unsigned char input_image[BMP_SIZE][BMP_SIZE][BMP_CHANNELS], unsigned char (*p1)[BMP_SIZE]){
    for (int x = 0; x < BMP_SIZE; x++){
        for (int y = 0; y < BMP_SIZE; y++){
            input_image[x][y][0] = p1[x][y];
            input_image[x][y][1] = p1[x][y];
            input_image[x][y][2] = p1[x][y];
        }
    }
}
int inBounds(int x, int y){
    if (x >= 0 && x < BMP_SIZE && y >= 0 && y < BMP_SIZE) {
        return 1;
    }
    return 0;
}
void erode(unsigned char (*p1)[BMP_SIZE], unsigned char (*p2)[BMP_SIZE], int erodeMatrix[4][2], int bigErodeMatrix[20][2]) {
    // For all cells
    for (int x = 0; x < BMP_SIZE; x++){    
        for (int y = 0; y < BMP_SIZE; y++){
            int countBlack = 0;
            int found = 0;
            // For each of the 4 neighbouring squares. Check if black neighbour and color accordingly
            for (int i = 0; i < 4; i++){
                if(inBounds(x + erodeMatrix[i][0], y + erodeMatrix[i][1]) && p1[x + erodeMatrix[i][0]][y + erodeMatrix[i][1]] == 0){
                    found = 1;
                    countBlack++;
                    p2[x][y] = 0;
                }
            } 
            // If no black neighbour is found, keep color
            if (!found) {
                p2[x][y] = p1[x][y];
            }
            // Check area around, and count black cells
            for (int i = 0; i < 20; i++){
                if (inBounds(x + bigErodeMatrix[i][0], y + bigErodeMatrix[i][1]) && p1[x + bigErodeMatrix[i][0]][y + bigErodeMatrix[i][1]] == 0){
                    countBlack++;
                }
            }
            // If more than x black cells is found, use erode for all 8 neighbouring and diagonal cells.
            if (countBlack <= 11 && p1[x][y] == 0) {
                for (int i = -1; i <= 1; i++){
                    for (int j = -1; j <= 1; j++){
                        if (inBounds(x + bigErodeMatrix[i][0], y + bigErodeMatrix[i][1])){
                            p2[x + bigErodeMatrix[i][0]][y + bigErodeMatrix[i][1]] = 0;
                        }
                    }   
                }
            }
        }
    }
}

int frameCheck(unsigned char (*p1)[BMP_SIZE], int x, int y)   {
    for (int i = 1 - FRAMESIZE; i <= FRAMESIZE; i++) {
        if(i == 1 - FRAMESIZE || i == FRAMESIZE) {
            for (int j = 1 - FRAMESIZE; j <= FRAMESIZE; j++) {
                if(inBounds(i + x, j + y) && p1[i + x][j + y]) {
                    return 1;
                }
            }
        } else if(i + x >= 0 && i + x < BMP_SIZE && p1[x + i][y - FRAMESIZE + 1] || p1[x + i][y + FRAMESIZE]) {
            return 1;
        }
    }
    return 0;
}

// Sets all pixels within cell detection frame to black.
void setAllBlack(unsigned char (*p1)[BMP_SIZE], int x, int y){
    for (int i = 2 - FRAMESIZE; i <= FRAMESIZE - 1; i++) {
        for (int j = 2 - FRAMESIZE; j <= FRAMESIZE - 1; j++){
            p1[x + i][y + j] = 0;
        }
    }
}

void detectCells(unsigned char (*p1)[BMP_SIZE]) {
    for (int x = 0; x < BMP_SIZE; x++){    
        for (int y = 0; y < BMP_SIZE; y++){
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

// Checks whether any white exists on the image
int checkWhite(unsigned char (*p1)[BMP_SIZE]) {
    for(int i = 0; i < BMP_SIZE; i++) {
        for(int j = 0; j < BMP_SIZE; j++) {
            if(p1[i][j]) {
                return 1;
            }
        }
    }
    return 0;
}

void setRed(unsigned char pixel[BMP_CHANNELS]){
    pixel[0] = 255;
    pixel[1] = 0;
    pixel[2] = 0;    
}

void drawCross(unsigned char input_image[BMP_SIZE][BMP_SIZE][BMP_CHANNELS], int coords[1000][2]) {
    for (int i = 0; i < cells; i++) {
        for (int j = -7; j <= 8; j++) {
            if (coords[i][0] + j >= 0 && coords[i][0] + j < BMP_SIZE){
                setRed(input_image[coords[i][0] + j][coords[i][1]]);
            } 
            if (coords[i][1] + j >= 0 && coords[i][1] + j < BMP_SIZE){
                setRed(input_image[coords[i][0]][coords[i][1] + j]);
            }
            if (coords[i][0] + j >= 0 && coords[i][0] + j < BMP_SIZE && coords[i][1] + 1 < BMP_SIZE){
                setRed(input_image[coords[i][0] + j][coords[i][1] + 1]);
            }
            if (coords[i][1] + j >= 0 && coords[i][1] + j < BMP_SIZE && coords[i][0] + 1 < BMP_SIZE){
                setRed(input_image[coords[i][0] + 1][coords[i][1] + j]);
            }   
        }
    }
}

// Swap arrays using pointers
void swap(unsigned char (**p1)[BMP_SIZE], unsigned char (**p2)[BMP_SIZE]) {
    unsigned char (*p3)[BMP_SIZE] = *p1;
    *p1 = *p2;
    *p2 = p3;
}

// Main method
int main(int argc, char** argv){
    if (argc != 3){
        fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
        exit(1);
    }
    // Start timer
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    // Initial computations
    read_bitmap(argv[1], input_image);
    convertToBW(input_image, p1);
    detectCells(p1);
    
    // Loop until all cells are found.
    int i = 0;
    while(checkWhite(p1)) {
        erode(p1, p2, erodeMatrix, bigErodeMatrix);
        swap(&p1, &p2);
        detectCells(p1);
        
        
        // Snippet used to save erode steps
        /*
        convertToImage(output_image, p1);
        char string[100];
        snprintf(string, 100, "./pictures/picture%d.bmp", i++);
        write_bitmap(output_image, string); 
        */
    }

    // Save image to file
    drawCross(input_image, coord);
    write_bitmap(input_image, argv[2]);

    // Print results
    printf("Done! Found %d cells\n", cells);
    for (int i = 0; i < cells; i++){
        printf("(%d, %d)\t ", coord[i][0], coord[i][1]);
    }
    printf("\n");
    
    // Stop timer
    end = clock();
    cpu_time_used = end - start;
    printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
    return 0;
}

