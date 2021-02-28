#pragma once

#include <string>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Object2Dv2
{
	Mesh* CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);

	Mesh* CreateBaloon(std::string balloon_name, glm::vec3 center, float rX, float rY, bool colorRed, Mesh* balloon_triangle, Mesh* balloon_line);

	Mesh* CreateArrow(std::string rectangle_name, bool powerBar, float length, float triangle_high, float rectangle_high, Mesh* triangle);

	Mesh* CreateBow(std::string name, float rX, float rY);

	Mesh* CreateShuriken(std::string name, float length);
}

