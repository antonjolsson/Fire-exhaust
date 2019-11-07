
#include <GL/glew.h>

// STB_IMAGE for loading images of many filetypes
#include <stb_image.h>

#include <cstdlib>

#include <labhelper.h>

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

// The window we'll be rendering to
SDL_Window* g_window = nullptr;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "lab2_main.h"
using namespace glm;

int mag = 1;
int mini = 5;
float anisotropy = 16.0f;
float camera_pan = 0.f;
bool showUI = false;

// The shaderProgram holds the vertexShader and fragmentShader
GLuint shaderProgram;

// The vertexArrayObject here will hold the pointers to
// the vertex data (in positionBuffer) and color data per vertex (in colorBuffer)
GLuint roadPosBuffer, explPosBuffer, colorBuffer, indexBuffer, roadTexBuffer, explTexBuffer, roadVAO, explosionVAO;

void initGL()
{
	///////////////////////////////////////////////////////////////////////////
	// Create the vertex array object
	///////////////////////////////////////////////////////////////////////////
	// Create a handle for the vertex array object
	glGenVertexArrays(1, &roadVAO);
	// Set it as current, i.e., related calls will affect this object
	glBindVertexArray(roadVAO);

	///////////////////////////////////////////////////////////////////////////
	// Create the positions buffer object
	///////////////////////////////////////////////////////////////////////////
	const float roadPositions[] = {
		// X      Y       Z
		-10.0f, -4.2f,  -10.0f,  // v0
		-10.0f, 20.0f, -330.0f, // v1
		10.0f,  20.0f, -330.0f, // v2
		10.0f,  -4.2f,  -10.0f   // v3
	};
	// Create a handle for the vertex position buffer
	glGenBuffers(1, &roadPosBuffer);
	// Set the newly created buffer as the current one
	glBindBuffer(GL_ARRAY_BUFFER, roadPosBuffer);
	// Send the vetex position data to the current buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadPositions), roadPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false /*normalized*/, 0 /*stride*/, 0 /*offset*/);
	// Enable the attribute
	glEnableVertexAttribArray(0);

	///////////////////////////////////////////////////////////////////////////
	// >>> @task 1 : Create the texture coordinates.
	//				 Create the texture coordinates' buffer object.
	//				 Set up the attrib pointer.
	//				 Enable the vertex attrib array.
	///////////////////////////////////////////////////////////////////////////

	float roadTexCoords[] = {
	0.0f, 0.0f,    // (u,v) for v0
    0.0f, 15.0f,   // (u,v) for v1
    1.0f, 15.0f,   // (u,v) for v2
    1.0f, 0.0f     // (u,v) for v3
	};

	glGenBuffers(1, &roadTexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, roadTexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(roadTexCoords), roadTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, false/*normalized*/, 0/*stride*/, 0/*offset*/);
	glEnableVertexAttribArray(2);

	///////////////////////////////////////////////////////////////////////////
	// Create the element array buffer object
	///////////////////////////////////////////////////////////////////////////
	const int indices[] = {
		0, 1, 3, // Triangle 1
		1, 2, 3  // Triangle 2
	};
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// The loadShaderProgram and linkShaderProgam functions are defined in glutil.cpp and
	// do exactly what we did in lab1 but are hidden for convenience
	shaderProgram = labhelper::loadShaderProgram("../lab2-textures/simple.vert",
	                                             "../lab2-textures/simple.frag");

	//**********************************************

	//************************************
	//			Load Texture
	//************************************
	// >>> @task 2
	// >>> @task 2.1
	int w, h, comp;
	unsigned char* roadImage = stbi_load("../scenes/asphalt.jpg", &w, &h, &comp, STBI_rgb_alpha);
	glGenTextures(1, &roadTexBuffer);
	glBindTexture(GL_TEXTURE_2D, roadTexBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, roadImage);
	free(roadImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);
	// Sets the type of filtering to be used on magnifying and
	// minifying the active texture. These are the nicest available options.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);

	//************************************
	//			Init explosion!
	//************************************

	glGenVertexArrays(1, &explosionVAO);
	glBindVertexArray(explosionVAO);

	const float explPositions[] = {
		// X      Y       Z
		-40.0f, -20.0f,  -170.0,  // v0
		-40.0f, 60.0f, -170.0f, // v1
		40.0f,  60.0f, -170.0f, // v2
		40.0f,  -20.0f, -170.0f   // v3
	};

	glGenBuffers(1, &explPosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, explPosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, explPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(explPositions), explPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	float explTexCoords[] = {
	0.0f, 0.0f, // (u,v) for v0 
	0.0f, 1.0f, // (u,v) for v1
	1.0f, 1.0f, // (u,v) for v2
	1.0f, 0.0f // (u,v) for v3
	};

	glGenBuffers(1, &explTexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, explTexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(explTexCoords), explTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(2);

	unsigned char* explImage = stbi_load("../scenes/explosion.png", &w, &h, &comp, STBI_rgb_alpha);
	glGenTextures(1, &explTexBuffer);
	glBindTexture(GL_TEXTURE_2D, explTexBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, explImage);
	free(explImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void display(void)
{
	// The viewport determines how many pixels we are rasterizing to
	int w, h;
	SDL_GetWindowSize(g_window, &w, &h);
	// Set viewport
	glViewport(0, 0, w, h);

	// Set clear color
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	// Clears the color buffer and the z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// We disable backface culling for this tutorial, otherwise care must be taken with the winding order
	// of the vertices. It is however a lot faster to enable culling when drawing large scenes.
	glDisable(GL_CULL_FACE);
	// Disable depth testing
	glDisable(GL_DEPTH_TEST);
	// Set the shader program to use for this draw call
	glUseProgram(shaderProgram);

	// Set up a projection matrix
	float fovy = radians(45.0f);
	float aspectRatio = float(w) / float(h);
	float nearPlane = 0.01f;
	float farPlane = 300.0f;
	mat4 projectionMatrix = perspective(fovy, aspectRatio, nearPlane, farPlane);
	// Send it to the vertex shader
	int loc = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(loc, 1, false, &projectionMatrix[0].x);

	loc = glGetUniformLocation(shaderProgram, "cameraPosition");
	glUniform3f(loc, camera_pan, 0, 0);

	glBindVertexArray(roadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// >>> @task 3.1
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, roadTexBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST + mag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mini + (mini < 2 ? GL_NEAREST : GL_NEAREST_MIPMAP_NEAREST - 2));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

	glBindVertexArray(roadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, explTexBuffer);

	glBindVertexArray(explosionVAO);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0); // "unsets" the current shader program. Not really necessary.
}

void gui()
{
	// Inform imgui of new frame
	ImGui_ImplSdlGL3_NewFrame(g_window);

	// ----------------- Set variables --------------------------
	ImGui::PushID("mag");
	ImGui::Text("Magnification");
	ImGui::RadioButton("GL_NEAREST", &mag, 0);
	ImGui::RadioButton("GL_LINEAR", &mag, 1);
	ImGui::PopID();

	ImGui::PushID("mini");
	ImGui::Text("Minification");
	ImGui::RadioButton("GL_NEAREST", &mini, 0);
	ImGui::RadioButton("GL_LINEAR", &mini, 1);
	ImGui::RadioButton("GL_NEAREST_MIPMAP_NEAREST", &mini, 2);
	ImGui::RadioButton("GL_NEAREST_MIPMAP_LINEAR", &mini, 3);
	ImGui::RadioButton("GL_LINEAR_MIPMAP_NEAREST", &mini, 4);
	ImGui::RadioButton("GL_LINEAR_MIPMAP_LINEAR", &mini, 5);
	ImGui::PopID();

	ImGui::SliderFloat("Anisotropic filtering", &anisotropy, 1.0, 16.0, "Number of samples: %.0f");
	ImGui::Dummy({ 0, 20 });
	ImGui::SliderFloat("Camera Panning", &camera_pan, -1.0, 1.0);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
	            ImGui::GetIO().Framerate);
	// ----------------------------------------------------------

	// Render the GUI.
	ImGui::Render();
}

int main(int argc, char* argv[])
{
	g_window = labhelper::init_window_SDL("OpenGL Lab 2");

	initGL();

	// render-loop
	bool stopRendering = false;
	while(!stopRendering)
	{
		// render to window
		display();

		// Render overlay GUI.
		if (showUI)
		{
			gui();
		}

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);

		// check events (keyboard among other)
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			// Allow ImGui to capture events.
			ImGui_ImplSdlGL3_ProcessEvent(&event);

			if(event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
			{
				stopRendering = true;
			}
			if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
			{
				showUI = !showUI;
			}
		}
	}

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);
	return 0;
}
