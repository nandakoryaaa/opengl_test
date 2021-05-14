#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <SDL2/SDL.h>

#include "gl.h"
#include <SDL2/SDL_opengl.h>

typedef struct {
	float x, y, tx, ty;
} Vertex;

char string_buffer[1024];
char log_buffer[1024];

GLuint load_shader(
	int type, const char *filename,
	char *string_buffer, char *log_buffer
) {
	FILE *file = fopen(filename, "r");
	int size = fread(string_buffer, 1, 1023, file);
	fclose(file);
	string_buffer[size] = 0;

	GLuint shader = glCreateShader(type);
	const GLchar* parts[] = {(const GLchar *) string_buffer};
	glShaderSource(shader, 1, parts, NULL);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		GLint log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		if (log_length > 0) {
			GLsizei written;
    		glGetShaderInfoLog(shader, 1023, &written, log_buffer);
			log_buffer[log_length] = 0;
		}
		return 0;
	}

	return shader;
}

void convert_surface_rgb(SDL_Surface *surf) {
	unsigned char *pixels = (unsigned char *) surf->pixels;
	unsigned char *end = pixels + surf->h * surf->pitch;
	while (pixels < end) {
		unsigned char tmp = pixels[0];
		pixels[0] = pixels[2];
		pixels[2] = tmp;
		pixels += surf->format->BytesPerPixel;
	}
}

int main(int argc, char * argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE);


	int width = 800;
	int height = 600;

	SDL_Window * window = SDL_CreateWindow(
		"OpenGL App",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_OPENGL
	);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, "shaders/vertex_shader.glsl", string_buffer, log_buffer);
	if (!vertex_shader) {
		printf("Vertex shader error log: %s\n", log_buffer);
		return 1;
	}

	GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, "shaders/fragment_shader.glsl", string_buffer, log_buffer);
	if (!fragment_shader) {
		printf("Fragment shader error log: %s\n", log_buffer);
		return 1;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		printf("Failed to link shader program\n");
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			GLsizei written;
			glGetProgramInfoLog(program, sizeof(log_buffer) - 1, &written, log_buffer);
			printf("Program log: \n%s", log_buffer);
		}
		return 1;
	}

	glUseProgram(program);

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glViewport(0, 0, width, height);

	Vertex vertices[] = {
		{ -0.8f, -0.8f, 0.0f, 1.0f },
		{ 0.8f, -0.8f, 1.0f, 1.0f },
		{ 0.0f,  0.8f, 0.5f, 0.0f }
	};

	SDL_Surface *bmpSurface = SDL_LoadBMP("textures/mona-256.bmp");
	if (!bmpSurface) {
		printf("Error: %s\n", SDL_GetError());
		return 1;
	}

	convert_surface_rgb(bmpSurface);

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		bmpSurface->w, bmpSurface->h, 0,
		GL_RGB, GL_UNSIGNED_BYTE,
		bmpSurface->pixels
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLuint vertex_buffer_handle;
	glGenBuffers(1, &vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);

	glBufferData(
		GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		GL_STATIC_DRAW
	);

	GLuint vertex_array_handle;

	glGenVertexArrays(1, &vertex_array_handle);
	glBindVertexArray(vertex_array_handle);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
	glVertexAttribPointer(
		0, 4, GL_FLOAT, GL_FALSE,
		0, NULL
	);

	SDL_Event event;

	glBindVertexArray(vertex_array_handle);

	GLint progress_loc =
		glGetUniformLocation(program, "progress");

	float progress = 0.0;
	for (;;) {
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform1f(progress_loc, progress);

		progress += 0.01;
		if (progress >= 1) {
			progress = 0;
		}

		glDrawArrays(GL_TRIANGLES, 0, 3);

		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}

		SDL_GL_SwapWindow(window);
		SDL_Delay(1);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
