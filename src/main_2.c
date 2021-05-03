#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <SDL2/SDL.h>

#include "gl.h"
#include <SDL2/SDL_opengl.h>

int main( int argc, char * argv[] )
{
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

	glClearColor(0.5, 0.0, 0.0, 0.0);
	glViewport(0, 0, width, height);

	float vertices[] = {
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f,  0.8f, 0.0f
	};


	GLuint vertex_buffer_handle;
	glGenBuffers(1, &vertex_buffer_handle);


	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
	glBufferData(
		GL_ARRAY_BUFFER, 9 * sizeof(float), vertices,
		GL_STATIC_DRAW
	);

	GLuint vertex_array_handle;

	glGenVertexArrays(1, &vertex_array_handle);
	glBindVertexArray(vertex_array_handle);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
	glVertexAttribPointer(
		0, 3,
		GL_FLOAT, GL_FALSE,
		0, NULL
	);


	SDL_Event event;

	for (;;) {
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vertex_array_handle);
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
