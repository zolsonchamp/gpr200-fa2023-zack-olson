#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"
#include "transformations.h"

namespace zoo {

	//Camera aiming related variables
	struct CameraControls {
		double prevMouseX, prevMouseY; //Mouse position from previous frame
		float yaw = 0, pitch = 0; //Degrees
		float mouseSensitivity = 0.1f; //How fast to turn with mouse
		bool firstMouse = true; //Flag to store initial mouse position
		float moveSpeed = 5.0f; //How fast to move with arrow keys (M/S)
	};

	struct Camera {
		ew::Vec3 position; //Camera body position
		ew::Vec3 target; //Position to look at
		float fov; //Vertical field of view in degrees
		float aspectRatio; //Screen width / Screen height
		float nearPlane; //Near plane distance (+Z)
		float farPlane; //Far plane distance (+Z)
		bool orthographic; //Perspective or orthographic?
		float orthoSize; //Height of orthographic frustum
		ew::Mat4 ViewMatrix()
		{
			return zoo::LookAt(position, target, ew::Vec3(0.0f, 1.0f, 0.0f));
		};
		ew::Mat4 ProjectionMatrix()
		{
			switch (orthographic)
			{
			case true:
				return zoo::Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
				break;
			case false:
				return zoo::Perspective(ew::Radians(fov), aspectRatio, nearPlane, farPlane);
				break;
			}
		};

	};
}
