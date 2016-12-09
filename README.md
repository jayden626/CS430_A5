# CS430
## Project 5 - ezview

This program uses OpenGL ES2, GLFW and a custom PPM reader to display a P3 or P6 image in a window.
The keyboard can be used to apply transformations to the image such as scale, translate, rotate and shear.

### Usage
Correct usage of this program is ProgramName FileName.ppm

### Keyboard Shortcuts

| Key        			| Effect           			|
| --------------------- | ------------------------- |
| UP arrow     			| Translate image up 		|
| DOWN arrow    		| Translate image down		|
| LEFT arrow     		| Translate image left		|
| RIGHT arrow     		| Translate image right 	|
| SHIFT + UP arrow     	| Shear image up 			|
| SHIFT + DOWN arrow    | Shear image down 			|
| SHIFT + LEFT arrow    | Shear image left 			|
| SHIFT + RIGHT arrow   | Shear image right 		|
| COMMA 				| Decrease scale 			|
| PERIOD 				| Increase scale 			|
| SHIFT + COMMA 		| Rotate left 10 degrees	|
| SHIFT + PERIOD 		| Rotate Right 10 degrees	|


### Notes
Currently does not work with P6 images. There is a problem reading the image into the pixel map and I do not know what it is.

The provided TestFile.ppm is in P3 format.
The provided recurse2.ppm is an identical image in P6 format.

Author: Jayden Urch
Date: 12/08/2016
Student No: 5388406 / jsu22
