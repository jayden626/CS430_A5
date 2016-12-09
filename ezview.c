/*
 * Author: Jayden Urch
 * Date: 12/08/2016
 * Student No: 5388406 / jsu22
 */
#define GLFW_DLL 1
#define GL_GLEXT_PROTOTYPES
#define GL_FRAGMENT_PRECISION_HIGH 1
#define M_PI acos(-1.0)

#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "linmath.h"
#include "ppmrw.h"

int image_width, image_height;
RGBpixel* pixmap;

GLFWwindow* window;
GLuint vertex_buffer, vertex_shader, fragment_shader, program;
GLint mvp_location, vpos_location, vcol_location, texcoord_location, tex_location;

double scale, rotation, translate_x, translate_y, shear_x, shear_y;

typedef struct {
  float Position[2];
  float TexCoord[2];
} Vertex;

Vertex vertexes[] = {
			{{1, -1}, {0.99999, 0.99999}},
            {{1, 1}, {0.99999, 0.00000}},
            {{-1, 1}, {0.00000, 0.00000}},
            {{-1, 1}, {0.00000, 0.00000}},
            {{-1,-1}, {0.00000, 0.99999}},
            {{1, -1}, {0.99999, 0.99999}}
};


static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec2 TexCoordIn;\n"
"attribute vec2 vPos;\n"
"varying vec2 TexCoordOut;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    TexCoordOut = TexCoordIn;\n"
"}\n";

static const char* fragment_shader_text =
"varying lowp vec2 TexCoordOut;\n"
"uniform sampler2D Texture;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(Texture, TexCoordOut);\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

	//Move image up, or shear up if shift is held
	if (key == GLFW_KEY_UP && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS){
        shear_y += 0.1;
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
        translate_y += 0.1;
    }

	//Move image down, or shear down if shift is held
    if (key == GLFW_KEY_DOWN && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS){
        shear_y -= 0.1;
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        translate_y -= 0.1;
    }

	//Move image left, or shear left if shift is held
    if (key == GLFW_KEY_LEFT && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS){
        shear_x -= 0.1;
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
       translate_x -= 0.1;
    }

	//Move image right, or shear right if shift is held
    if (key == GLFW_KEY_RIGHT && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS){
        shear_x += 0.1;
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        translate_x += 0.1;
    }

	//Decrease scale, rotate left if shift is held
	if (key == GLFW_KEY_COMMA && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS){
        rotation += 10*M_PI/180;
    }
    else if (key == GLFW_KEY_COMMA && action == GLFW_PRESS){
        scale /= 1.1;
    }

	//Increase scale, rotate right if shift is held
	if (key == GLFW_KEY_PERIOD && mods == GLFW_MOD_SHIFT && action == GLFW_PRESS){
        rotation -= 10*M_PI/180;
    }
    else if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS){
        scale *= 1.1;
    }
}

void glCompileShaderOrDie(GLuint shader) {
	GLint compiled;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		char* info = malloc(infoLen+1);
		GLint done;
		glGetShaderInfoLog(shader, infoLen, &done, info);
		printf("Unable to compile shader: %s\n", info);
		exit(1);
	}
}

int initialize_window(){

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1000, 1000, "EzView", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted due to poor programming

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShaderOrDie(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShaderOrDie(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    assert(mvp_location != -1);

    vpos_location = glGetAttribLocation(program, "vPos");
    assert(vpos_location != -1);

    texcoord_location = glGetAttribLocation(program, "TexCoordIn");
    assert(texcoord_location != -1);

    tex_location = glGetUniformLocation(program, "Texture");
    assert(tex_location != -1);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (sizeof(float) * 2));

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixmap);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniform1i(tex_location, 0);
    
	return 1;
}

int main(int argc, char* argv[])
{

	int color_scale;
	char magicNumber[255];
	
	scale = 1;
	rotation = 0;
	translate_x = 0;
	translate_y = 0;
	shear_x = 0;
	shear_y = 0;

	if(argc != 2){
		fprintf(stderr, "Error: Incorrect number of arguments. Proper usage is programName filename.ppm\n");
		exit(1);
	}

	FILE* input = fopen(argv[1], "r");
	if(!input){
		fprintf(stderr, "Error: Cannot open input file.");
		return 1;
	}

	//read ppm file information and store in local variables
	readHeader(input, magicNumber, &image_width, &image_height, &color_scale);
	pixmap = malloc(sizeof(RGBpixel)*image_height*image_width*3);
	
	//Check magicNumber and use appropriate read function
	if(strcmp(magicNumber,"P3") == 0){
		readP3(input, pixmap, image_width, image_height, color_scale);
	}
	else if(strcmp(magicNumber,"P6") == 0){
		readP6(input, pixmap, image_width, image_height);
		fprintf(stderr, "Program does not work with P6 images, displays black screen. Could not figure out why");
	}
	else{
		fprintf(stderr, "Error: magic number is incorrect\n");
		exit(1);
	}

	//Initialize the GLFW window
    initialize_window();

	//Program loop
	while (!glfwWindowShouldClose(window)) {

		int width, height;
		mat4x4 shear_matrix, rotation_matrix, scale_matrix, translation_matrix;
		mat4x4 transformation_matrix; //the final matrix with all transformations applied
		mat4x4 temp1, temp2;

		//Initialize matricies to the identity matrix
		mat4x4_identity(shear_matrix);	
		mat4x4_identity(rotation_matrix);
		mat4x4_identity(scale_matrix);
		mat4x4_identity(translation_matrix);
		//mat4x4_identity(transformation_matrix); //redundant
						
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		shear_matrix[1][0] = shear_x; //Shear in x axis
		shear_matrix[0][1] = shear_y; //Shear in y axis

		mat4x4_rotate_Z(rotation_matrix, rotation_matrix, rotation); //Rotate the image

		scale_matrix[0][0] *= scale; //Scale in x axis
		scale_matrix[1][1] *= scale; //Scale in y axis

		mat4x4_translate(translation_matrix,translate_x, translate_y, 0); //Translate the image

		//multiply all matricies together
		mat4x4_mul(transformation_matrix, scale_matrix, shear_matrix);
		mat4x4_mul(transformation_matrix, transformation_matrix, rotation_matrix);
		mat4x4_mul(transformation_matrix, transformation_matrix, translation_matrix);

		//update program and swap buffers
		glUseProgram(program);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) transformation_matrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

