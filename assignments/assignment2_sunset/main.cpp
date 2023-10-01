#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <zoo/shader.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

struct Vertex {
	float x, y, z;
	float u, v;
};

unsigned int createShader(GLenum shaderType, const char* sourceCode);
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

//float vertices[18] = {
//	//x    y    z
//	//Triangle 1 
//	-0.5, -0.5, 0.0,
//	-0.5,  0.5, 0.0,
//	 0.5,  0.5, 0.0,
//	//Triangle 2
//	0.5,  0.5, 0.0,
//	 0.5, -0.5, 0.0,
//	-0.5, -0.5, 0.0
//};

//float vertices[12] = {
//	//x    y    z
//	-0.5, -0.5, 0.0, //Bottom left
//	-0.5,  0.5, 0.0, //Bottom right
//	0.5,  0.5, 0.0, //Top right
//	0.5, -0.5, 0.0, //Top left
//};


Vertex vertices[4] = {
	//x    y    z    u    v
   { -1, -1, 0.0, 0, 0 }, //Bottom left
   { 1, -1, 0.0, 1, 0 }, //Bottom right
   { 1, 1, 0.0, 1, 1 },  //Top right
   { -1, 1, 0.0, 0, 1 }  //Top left
};


unsigned int indices[6] = {
	0, 2, 3, //Triangle 1
	0, 1, 2,  //Triangle 2
};




float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float skyColor[3] = { 0.0f, 4.0f, 4.0f };
float triangleBrightness = 1.0f;
bool showImGUIDemoWindow = true;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	zoo::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();

	//unsigned int shader = createShaderProgram(vertexShaderSource, fragmentShaderSource);
	unsigned int vao = createVAO(vertices, 12, indices, 6);

	std::string vertexShaderSource = zoo::loadShaderSourceFromFile("assets/vertexShader.vert");
	std::string fragmentShaderSource = zoo::loadShaderSourceFromFile("assets/fragmentShader.frag");
	//unsigned int shader = createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());


	//glUseProgram(shader);
	glBindVertexArray(vao);

	//wireframe
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/


	while (!glfwWindowShouldClose(window)) {

		float time = (float)glfwGetTime();

		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		//glUniform3f(glGetUniformLocation(shader, "_Color"), triangleColor[0], triangleColor[1], triangleColor[2]);
		shader.setVec3("_Color", triangleColor[0], triangleColor[1], triangleColor[2]);
		//glUniform1f(glGetUniformLocation(shader,"_Brightness"), triangleBrightness);
		shader.setFloat("_Brightness", triangleBrightness);
		shader.setVec3("iResolution", SCREEN_WIDTH, SCREEN_HEIGHT, 0.0);
		shader.setFloat("iTime", time);
		shader.setVec3("skyColor", skyColor[0], skyColor[1], skyColor[2]);

		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Color", triangleColor);
			ImGui::ColorEdit3("skyColor", skyColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);
			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}


unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 5, vertexData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(0);

	//UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
	glEnableVertexAttribArray(1);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	return vbo;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

