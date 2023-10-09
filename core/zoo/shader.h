#pragma once
#include <sstream>
#include <fstream>
#include "../ew/ewMath/mat4.h"

namespace zoo 
{
	std::string loadShaderSourceFromFile(const std::string& filePath);


	class Shader {
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use();
		void setInt(const std::string& name, int v) const;
		void setFloat(const std::string& name, float v) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setMat4(const std::string& name, const ew::Mat4& v) const;

	private:
		unsigned int m_id;
	};

}