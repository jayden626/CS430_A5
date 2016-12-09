/**
 * Author: Jayden Urch
 * Date: 09/16/2016
 * Student No: 5388406 / jsu22
 *
 * This program can be used to read a P3 or P6 PPM file into memory
 * and write the memory to a file in either P3 or P6 format.
 * It's proper usage should be "ppmrw [3|6] inputFile.extension outputFile.extension"
 *
 * NOTE: If height or width is less than 1, program will throw an error.
 * NOTE: Will not throw an error if the width*height is less than or greater than the number of input pixels
 *       i.e. a 3*3 file having 4*4 input pixels or a 5*5 file having 4*4 input pixels
 */

#include "ppmrw.h"

//Main function.
/*int main(int argc, char* argv[]) {
	RGBpixel* pixmap;
	FILE* input;
	FILE* output;
	int convertToNumber; //Magic number to write out to
	char magicNumber[255];
	
	int height;
	int width;
	int scale;

	//Reading in arguements and checking for errors
	if(argc != 4){
		fprintf(stderr, "Error: Wrong amount of arguements. Correct format is ppmrw [3|6] input output.");
		return 1;
	}
	convertToNumber = (int) strtol(argv[1],(char **)NULL,10); //strtol is used throughout this program to convert a string to an integer.	
	if(convertToNumber != 3 && convertToNumber != 6){
		fprintf(stderr, "Error: Not a correct conversion number. Correct formats are '3' or '6'.");
		return 1;
	}
	input = fopen(argv[2], "r");
	if(!input){
		fprintf(stderr, "Error: Cannot open input file.");
		return 1;
	}
	output = fopen(argv[3], "w");
	if(!input){
		fprintf(stderr, "Error: Cannot open output file.");
		return 1;
	}

	readHeader(input, magicNumber, &width, &height, &scale);

	//Initialize pixmap
	pixmap = malloc(sizeof(RGBpixel)*height*width*3);
	
	//Check magicNumber and use appropriate read function
	if(strcmp(magicNumber,"P3") == 0){
		readP3(input, pixmap, width, height, scale);
	}
	else if(strcmp(magicNumber,"P6") == 0){
		readP6(input, pixmap, width, height);
	}

	//Check conversion arguement and use appropriate output function
	if(convertToNumber == 3){
		writeP3(output, pixmap, width, height, scale);
	}
	else if(convertToNumber == 6){
		writeP6(output, pixmap, width, height, scale);
	}

	int inputClose = fclose(input);
	int outputClose = fclose(output);
	return 0;
}*/

int readHeader(FILE* file, char* thisMagicNumber, int* thisWidth, int* thisHeight, int* thisScale){

	unsigned char tempInput[255]; //Buffer that temporarily holds input values before storing them
	//Read magic number
	fscanf(file,"%s",thisMagicNumber);
	if(strcmp(thisMagicNumber, "P3") != 0 && strcmp(thisMagicNumber, "P6") != 0){
		fprintf(stderr, "Error: Magic number is incorrect. Expected \"P3\" or \"P6\", got %s.", *thisMagicNumber);
		exit(1);
	}

	//Read in all comment lines. The last read value will contain the width. Works with zero or many comments
	int commentLine; //boolean used to terminate while loop.
	do{
		fscanf(file,"%s",tempInput);
		if(tempInput[0] == '#'){
			commentLine = 1;
			char str[1000];
			fgets (str, 1000, file); //ignore rest of the line. please no lines over 1000 char
		}
		else{
			*thisWidth = (int) strtol(tempInput,(char **)NULL,10);
			commentLine = 0;
		}
	}
	while(commentLine);

	//Scanning and storing height and scale
	do{
		fscanf(file,"%s",tempInput);
		if(tempInput[0] == '#'){
			commentLine = 1;
			char str[1000];
			fgets (str, 1000, file);
		}
		else{
			*thisHeight = (int) strtol(tempInput,(char **)NULL,10);
			commentLine = 0;
		}
	}
	while(commentLine);

	do{
		fscanf(file,"%s",tempInput);
		if(tempInput[0] == '#'){
			commentLine = 1;
			char str[1000];
			fgets (str, 1000, file);
		}
		else{
			*thisScale = (int) strtol(tempInput,(char **)NULL,10);
			commentLine = 0;
		}
	}
	while(commentLine);
	fgetc(file); //Skipping the newline character after the scale
	
	//Checking that each channel is a single byte
	if(*thisScale < 0 || *thisScale > 255 || *thisWidth < 1 || *thisHeight < 1){
		fprintf(stderr, "Error: width, height and/or scale have incorrect values");
		exit(1);
	}
}
/*
 * This funtion prints the given array of RGBpixels to stdout
 * Preconditions: width, height and scale are greater than 0
 *                scale is no greater than 255
 *                map contains either ASCII or binary pixel data
 * Postconditions: writes the RGBpixel array to the stdout
 */
void printPixmap(RGBpixel* map, char* magicNumber, int width, int height, int scale){
	int maxPrintWidth = 3;
	int j;

	if(width < 0 || height < 0 || scale < 0 || scale > 255){
		fprintf(stderr, "Error writing P3: width, height and/or scale have incorrect values");
		exit(1);
	}
	
	//Print header, then loop through pixmap printing each value to stdout
	printf("%s\n%u %u\n%u\n", magicNumber, width, height, scale);
	for (j=0; j<height*width*3; j++) {
		if(map[j] > scale){
			fprintf(stderr, "Error printing to stdout: value to be written exceeds the scale");
			exit(1);
		}
		printf("%*u ", maxPrintWidth, map[j]);
		printf("\n");
	}
}

/*
 * This funtion writes the given array of RGBpixels to the given output file in P3 format
 * Preconditions: width, height and scale are greater than 0
 *                scale is no greater than 255
 *                map contains either ASCII or binary pixel data
 * Postconditions: writes the RGBpixel array to the passed output file in P3 format
 */
void writeP3(FILE* output, RGBpixel* map, int width, int height, int scale){
	int j;
	
	if(width < 1 || height < 1 || scale < 0 || scale > 255){
		fprintf(stderr, "Error writing P3: width, height and/or scale have incorrect values");
		exit(1);
	}
	
	//Print header, then loop through pixmap printing each value to output file
	fprintf(output, "P3\n%u %u\n%u\n", width, height, scale);
	for (j=0; j<height*width*3; j++) {
		if(map[j] > scale){
			fprintf(stderr, "Error writing P3: value to be written exceeds the scale");
			exit(1);
		}
		fprintf(output, "%u", map[j]);
		fprintf(output, "\n");
	}
}

/*
 * This funtion writes the given array of RGBpixels to the given output file in P6 format
 * Preconditions: width, height and scale are greater than 0
 *                scale is no greater than 255
 *                map contains either ASCII or binary pixel data
 * Postconditions: writes the RGBpixel array to the passed output file in P6 format
 */
void writeP6(FILE* output, RGBpixel* map, int width, int height, int scale){

	if(width < 1 || height < 1 || scale < 0 || scale > 255){
		fprintf(stderr, "Error writing P6: width, height and/or scale have incorrect values");
		exit(1);
	}
	
	//Print header, write the pixmap to the output file.
	fprintf(output, "P6\n%u %u\n%u\n", width, height, scale);
	fwrite(map, sizeof(RGBpixel), height*width*3, output);
}

/*
 * This funtion reads the given P3 input file and stores it in the buffer
 * Preconditions: width, height and scale are greater than 0
 *                scale is no greater than 255
 *                input file is in p3 format and has been read past the header (i.e. next read value will be a pixel value
 * Postconditions: stores the input pixel data to the passed RGBpixel array.
 */
void readP3(FILE* input, RGBpixel* map, int width, int height, int scale){
	unsigned char tempInput[8];
	int tempInt;
	int j = 0;
	for (j=0; j<height*width*3; j++) {
		fscanf(input,"%d",&tempInt);
		if(tempInt < 0 || tempInt > 255 || tempInt > scale){
			fprintf(stderr, "Error: String size out of bounds (less than 0, greater than 255 or greater than scale)");
			exit(1);
		}
		map[j] = (unsigned char)tempInt;
	}
}

/*
 * This funtion reads the given P6 input file and stored it in the buffer
 * Preconditions: width and heightare greater than 0
 *                input file is in p6 format and has been read past the header (i.e. next read value will be a pixel value
 * Postconditions: stores the input pixel data to the passed RGBpixel array.
 */
void readP6(FILE* input, RGBpixel* map, int width, int height){
	fread(map, sizeof(RGBpixel), height*width*3, input);
}


