#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <zoo/camera.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

zoo::Camera camera;
zoo::CameraControls cameraControls;

zoo::Camera defaultCamera;
zoo::CameraControls defaultCameraControls;

double previousTime = glfwGetTime();

void moveCamera(GLFWwindow* window, zoo::Camera* camera, zoo::CameraControls* controls)
{
	// If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		// Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}

	// GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// If we just started right-clicking, set prevMouse values to the current position.
	// This prevents a bug where the camera moves as soon as we click.
	if (controls->firstMouse) {
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	// Get mouse position delta for this frame
	double deltaX = mouseX - controls->prevMouseX;
	double deltaY = mouseY - controls->prevMouseY;

	// Add to yaw and pitch
	controls->yaw += deltaX * controls->mouseSensitivity;
	controls->pitch += deltaY * controls->mouseSensitivity;

	// Clamp pitch between -89 and 89 degrees
	if (controls->pitch > 89.0) {
		controls->pitch = 89.0;
	}
	else if (controls->pitch < -89.0) {
		controls->pitch = -89.0;
	}

	// Remember the previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	float yawRadians = ew::Radians(controls->yaw);
	float pitchRadians = ew::Radians(controls->pitch);

	// Calculate the forward vector
	ew::Vec3 forward;
	forward.x = cos(yawRadians) * cos(pitchRadians);
	forward.y = sin(pitchRadians);
	forward.z = sin(yawRadians) * cos(pitchRadians);

	// Normalize the forward vector (optional, depending on your camera implementation)
	forward = ew::Normalize(forward);

	// Set the camera's target to a point in front of the camera along its forward direction
	camera->target = camera->position + forward;

	// Construct camera right and up vectors using the Graham-Schmidt process
	ew::Vec3 worldUp = ew::Vec3(0.0f, 1.0f, 0.0f);
	ew::Vec3 cameraFront = ew::Normalize(camera->target - camera->position);
	ew::Vec3 right = ew::Normalize(ew::Cross(cameraFront, worldUp));
	ew::Vec3 up = ew::Normalize(ew::Cross(right, cameraFront));

	double currentTime = glfwGetTime();
	double deltaTime = currentTime - previousTime;
	previousTime = currentTime;


	// Keyboard controls for moving along forward, back, right, left, up, and down
	// Assuming the key mappings are: W for forward, S for backward, A for left, D for right, Space for up, and Left Shift for down
	float moveSpeed = controls->moveSpeed * deltaTime; // Scale by deltaTime for frame rate independence

	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera->position += cameraFront * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->position -= cameraFront * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->position -= right * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->position += right * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		camera->position += up * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		camera->position -= up * moveSpeed;
	}

	// Update the camera's target after changing the position
	camera->target = camera->position + cameraFront;

}


int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	//Camera values
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0, 0, 0);
	camera.fov = 60;
	camera.aspectRatio = (SCREEN_WIDTH / SCREEN_HEIGHT);
	camera.orthoSize = 6;
	camera.nearPlane = 0.1;
	camera.farPlane = 100;
	camera.orthographic = false;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		moveCamera(window, &camera, &cameraControls);
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		shader.use();
		shader.setMat4("_View", camera.ViewMatrix());
		shader.setMat4("_Projection", camera.ProjectionMatrix());

		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}
		

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");
			ImGui::DragFloat3("Position", &camera.position.x, 0.05f);
			ImGui::DragFloat3("Target", &camera.target.x, 0.05f);
			ImGui::Checkbox("Orthographic", &camera.orthographic);
			ImGui::DragFloat("Ortho Height", &camera.orthoSize, 0.05f);
			ImGui::DragFloat("FOV", &camera.fov, 0.05f);
			ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.05f);
			ImGui::DragFloat("Far Plane", &camera.farPlane, 0.05f);
			if (ImGui::Button("Reset Camera")) {
				camera = defaultCamera;
				cameraControls = defaultCameraControls;
				camera.position = ew::Vec3(0, 0, 5);
				camera.target = ew::Vec3(0, 0, 0);
				camera.fov = 60;
				camera.aspectRatio = (SCREEN_WIDTH / SCREEN_HEIGHT);
				camera.orthoSize = 6;
				camera.nearPlane = 0.1;
				camera.farPlane = 100;
				camera.orthographic = false;
			}
			ImGui::End();

			
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}



