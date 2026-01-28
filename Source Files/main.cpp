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
#include"../Header Files/Chunk.h"


const unsigned int width = 1600;
const unsigned int height = 1200;

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

std::uint32_t generateSeed() {
	std::random_device rd;
	return rd();
}

int main() {
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Scuffed minecraft", nullptr, nullptr);
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

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("../Resource Files/Shaders/default.vert", "../Resource Files/Shaders/default.frag");

	Texture blockTextures[] {
		Texture("../TextureSheet.png", "diffuse", 0, GL_RGB, GL_UNSIGNED_BYTE),
	};
	std::vector blockTex(blockTextures, blockTextures + sizeof(blockTextures) / sizeof(Texture));

	auto seed = generateSeed();

	std::vector<Chunk> chunks;
	std::vector<Mesh*> chunkMeshes;

	auto gridSize = 16;
	for (int x = -gridSize / 2; x <= gridSize / 2; x++) {
		for (int z = -gridSize / 2; z <= gridSize / 2; z++) {
			// Create chunk at world position (x, 0, z)
			// Note: Chunk position is in "chunk units", internal logic handles block coordinates
			chunks.emplace_back(glm::vec3(x, 0, z), seed);

			// Generate the mesh for the newly added chunk
			chunkMeshes.emplace_back(chunks.back().generateMesh(blockTex));
		}
	}

	//Chunk chunk(glm::ivec3(0, 0, 0), generateSeed());
	//Mesh* chunkMesh = chunk.generateMesh(blockTex);

	// Shader for light cube
	Shader lightShader("../Resource Files/Shaders/light.vert", "../Resource Files/Shaders/light.frag");
	// Store mesh data in vectors for the mesh
	std::vector lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	// Create light mesh
	Mesh light(lightVerts, lightInd, blockTex);


	auto lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	auto lightPos = glm::vec3(0.5f, 2.0f, 0.5f);
	auto lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glEnable(GL_DEPTH_TEST);

	// Original code from the tutorial
	/*Texture popCat("pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	popCat.texUnit(shaderProgram, "tex0", 0);*/

	Camera camera(width, height, glm::vec3(0.0f, 2.0f, 5.0f));

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

		// Draws different meshes
		shaderProgram.Activate();
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "useInstancing"), 0);
		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//chunkMesh->Draw(shaderProgram, camera);
		for (Mesh* mesh : chunkMeshes) {
			mesh -> Draw(shaderProgram, camera);
		}
		light.Draw(lightShader, camera);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	//delete chunkMesh;
	for (Mesh* mesh : chunkMeshes) {
		delete mesh;
	}
	chunkMeshes.clear();
	// Delete all the objects we've created
	shaderProgram.Delete();
	lightShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}