//------- Ignore this ----------
#include<filesystem>
#include "../Header Files/PlayerController.h"
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
#include"../Header Files/World.h"
#include"../Header Files/InputManager.h"


const unsigned int width = 1200;
const unsigned int height = 800;

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

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Scuffed minecraft", nullptr, nullptr);
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
	glViewport(0, 0, screenWidth, screenHeight);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("../Resource Files/Shaders/default.vert", "../Resource Files/Shaders/default.frag");

	Texture blockTextures[] {
		Texture("../TextureSheet.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
	};
	std::vector blockTex(blockTextures, blockTextures + sizeof(blockTextures) / sizeof(Texture));

	auto seed = generateSeed();
	World world(seed, blockTex);

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

	InputManager inputManager(screenWidth, screenHeight);
	PlayerController playerController(inputManager, screenWidth, screenHeight);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Updates State of Keybindings
		inputManager.update(window);
		playerController.update(deltaTime);

		if (inputManager.isActionJustPressed(Action::EXIT_GAME)) {
			std::cout << "Exit Game" << std::endl;
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Updates and exports the camera matrix to the Vertex Shader
		world.updateChunks(playerController.camera.Position);

		// Draws different meshes
		shaderProgram.Activate();

		world.draw(shaderProgram, playerController.camera);
		light.Draw(lightShader, playerController.camera);

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