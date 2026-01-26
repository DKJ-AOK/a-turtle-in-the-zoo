//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------

#include<iostream>
#include<glad/gl.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<stb_image.h>
#include"../Header Files/Mesh.h"


const unsigned int width = 800;
const unsigned int height = 800;


// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES                            /              NORMALS                /               COLORS                  /       TEXTURE COORDINATES       //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex blockVertices[] =
{
    // COORDINATES (vec3)           / NORMALS (vec3)      / COLORS (vec3)       / UVs (vec2)

    // --- TOP FACE (Green: U from 0.0 to 0.33) ---
    Vertex{glm::vec3(-0.2f,  0.2f,  0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.000f, 0.0f)},
    Vertex{glm::vec3(-0.2f,  0.2f, -0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.000f, 1.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f, -0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 1.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f,  0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 0.0f)},

    // --- BOTTOM FACE (Brown: U from 0.66 to 1.0) ---
    Vertex{glm::vec3(-0.2f, -0.2f, -0.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.67f, 0.0f)},
    Vertex{glm::vec3(-0.2f, -0.2f,  0.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.67f, 1.0f)},
    Vertex{glm::vec3( 0.2f, -0.2f,  0.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.000f, 1.0f)},
    Vertex{glm::vec3( 0.2f, -0.2f, -0.2f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.000f, 0.0f)},

    // --- SIDE FACES (Grass/Dirt Mix: U from 0.33 to 0.66) ---

    // FRONT
    Vertex{glm::vec3(-0.2f, -0.2f,  0.2f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 0.0f)},
    Vertex{glm::vec3( 0.2f, -0.2f,  0.2f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 0.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f,  0.2f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 1.0f)},
    Vertex{glm::vec3(-0.2f,  0.2f,  0.2f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 1.0f)},

    // BACK
    Vertex{glm::vec3( 0.2f, -0.2f, -0.2f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 0.0f)},
    Vertex{glm::vec3(-0.2f, -0.2f, -0.2f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 0.0f)},
    Vertex{glm::vec3(-0.2f,  0.2f, -0.2f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 1.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f, -0.2f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 1.0f)},

    // LEFT
    Vertex{glm::vec3(-0.2f, -0.2f, -0.2f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 0.0f)},
    Vertex{glm::vec3(-0.2f, -0.2f,  0.2f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 0.0f)},
    Vertex{glm::vec3(-0.2f,  0.2f,  0.2f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 1.0f)},
    Vertex{glm::vec3(-0.2f,  0.2f, -0.2f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 1.0f)},

    // RIGHT
    Vertex{glm::vec3( 0.2f, -0.2f,  0.2f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 0.0f)},
    Vertex{glm::vec3( 0.2f, -0.2f, -0.2f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 0.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f, -0.2f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.66f, 1.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f,  0.2f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.34f, 1.0f)}
};

GLuint blockIndices[] =
{
	// --- TOP FACE ---
	0, 1, 2,     // First Triangle
	0, 2, 3,     // Second Triangle

	// --- BOTTOM FACE ---
	4, 5, 6,     // First Triangle
	4, 6, 7,     // Second Triangle

	// --- FRONT FACE ---
	8, 9, 10,    // First Triangle
	8, 10, 11,   // Second Triangle

	// --- BACK FACE ---
	12, 13, 14,  // First Triangle
	12, 14, 15,  // Second Triangle

	// --- LEFT FACE ---
	16, 17, 18,  // First Triangle
	16, 18, 19,  // Second Triangle

	// --- RIGHT FACE ---
	20, 21, 22,  // First Triangle
	20, 22, 23   // Second Triangle
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", nullptr, nullptr);
	// Error check if the window fails to create
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	if (!gladLoadGL(glfwGetProcAddress)) {
		// Handle error: Failed to initialize GLAD
		return -1;
	}
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	// Texture data
	Texture textures[]
	{
		Texture("../planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("../planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("../Resource Files/Shaders/default.vert", "../Resource Files/Shaders/default.frag");
	// Store mesh data in vectors for the mesh
	std::vector verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector tex(textures, textures + sizeof(textures) / sizeof(Texture));
	// Create floor mesh
	Mesh floor(verts, ind, tex);

	Texture blockTextures[] {
		Texture("../DirtBlock.png", "diffuse", 0, GL_RGB, GL_UNSIGNED_BYTE),
	};

	std::vector blockVerts(blockVertices, blockVertices + sizeof(blockVertices) / sizeof(Vertex));
	std::vector blockInds(blockIndices, blockIndices + sizeof(blockIndices) / sizeof(GLuint));
	std::vector blockTex(blockTextures, blockTextures + sizeof(blockTextures) / sizeof(Texture));
	Mesh dirtBlock(blockVerts, blockInds, blockTex);

	// Shader for light cube
	Shader lightShader("../Resource Files/Shaders/light.vert", "../Resource Files/Shaders/light.frag");
	// Store mesh data in vectors for the mesh
	std::vector lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	// Create light mesh
	Mesh light(lightVerts, lightInd, tex);




	auto lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	auto lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	auto lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	auto pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	auto pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);

	auto blockPos = glm::vec3(-0.5f, 0.3f, -0.5f);
	auto blockModel = glm::mat4(1.0f);
	blockModel = glm::translate(blockModel, blockPos);

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST);

	// Original code from the tutorial
	/*Texture popCat("pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	popCat.texUnit(shaderProgram, "tex0", 0);*/

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

		// --- DRAW THE FLOOR ---
		shaderProgram.Activate();
		// Re-send floor model matrix in case it changed (or just to be safe)
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
		floor.Draw(shaderProgram, camera);

		// --- DRAW THE DIRT BLOCK ---
		// 1. We stay in shaderProgram.Activate()
		// 2. Update the "model" uniform to the block's specific matrix
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(blockModel));
		// 3. Trigger the draw call
		dirtBlock.Draw(shaderProgram, camera);

		// Draws different meshes
		//floor.Draw(shaderProgram, camera);
		light.Draw(lightShader, camera);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	shaderProgram.Delete();
	lightShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}