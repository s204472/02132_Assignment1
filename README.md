(Linux/Mac) To compile and run the program on linux or mac writte the following two commands in youre terminal:
To compile (Linux/Mac):

```
gcc cbmp.c main.c -o main.out -std=c99
```

To run (Linux/Mac):

```
./main.out input_image.bmp output_image.bmp
```

(Win) To compile and run the program on windows writte the following two commands in youre terminal:

To compile (Win): 

```
gcc cbmp.c main.c -o main.exe -std=c99
```

To run (win): 

```
main.exe input_image.bmp output_image.bmp
```



In the above example you have to replace input_image.bmp, with the specific image, that you want to process (remember the path to the picture).
Whereas the output_image.bmp should be replaced by the name and path, where you want to save the image after termination of the program. 

