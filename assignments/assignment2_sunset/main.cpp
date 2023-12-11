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



const float radius = 1.0f;  // Adjust the radius as needed

Vertex vertices[4] = {
    // x    y    z    u    v
    { -radius, -radius, 0.0, 0, 0 }, // Bottom left
    { radius, -radius, 0.0, 1, 0 },  // Bottom right
    { radius, radius, 0.0, 1, 1 },   // Top right
    { -radius, radius, 0.0, 0, 1 }   // Top left
};


unsigned int indices[6] = {
	0, 2, 3, //Triangle 1
	0, 1, 2,  //Triangle 2
};




float timeMultiplier = 5.0f;
bool showImGUIDemoWindow = false;
int manualPortal = 0;
float portalColor[4] = { 1.0, 1.0, 1.0, 1.0 };

int main() {
    printf("Initializing...");
    if (!glfwInit()) {
        printf("GLFW failed to init!");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Swirling Portal", NULL, NULL);
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

    // Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    zoo::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag"); // Update fragment shader file path
    shader.use();

    unsigned int vao = createVAO(vertices, 4, indices, 6); // Adjust vertex count

    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {

        float time = (float)glfwGetTime();

        glfwPollEvents();
        glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set uniforms
        shader.setVec2("u_resolution", SCREEN_WIDTH, SCREEN_HEIGHT);
        shader.setFloat("u_time", time);
        shader.setFloat("timeMultiplier", timeMultiplier);
        shader.setInt("setColor", manualPortal);
        shader.setVec4("portalColor", portalColor[0], portalColor[1], portalColor[2], portalColor[3]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Render UI
        {
            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");
            ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
            ImGui::SliderInt("Manual Portal Color", &manualPortal, 0, 3);
            ImGui::SliderFloat("Swirl Speed", &timeMultiplier, 0.0f, 50.0f);
            ImGui::ColorEdit4("Portal Color", portalColor);
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
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

