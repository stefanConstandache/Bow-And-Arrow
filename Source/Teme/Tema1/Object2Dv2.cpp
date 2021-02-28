#include "Object2Dv2.h"

#include <Core/Engine.h>
#include <cmath>

Mesh* Object2Dv2::CreateShuriken(std::string name, float cathetus)
{
	glm::vec3 color = glm::vec3(0, 0, 0);

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(0, 0, 0), color),
		VertexFormat(glm::vec3(0, -cathetus, 0), color),
		VertexFormat(glm::vec3(cathetus, -cathetus, 0), color),
		VertexFormat(glm::vec3(cathetus, 0, 0), color),
		VertexFormat(glm::vec3(cathetus, cathetus, 0), color),
		VertexFormat(glm::vec3(0, cathetus, 0), color),
		VertexFormat(glm::vec3(-cathetus, cathetus, 0), color),
		VertexFormat(glm::vec3(-cathetus, 0, 0), color),
		VertexFormat(glm::vec3(-cathetus, -cathetus, 0), color),
	};

	std::vector<unsigned short> indices =
	{
		0, 1, 2,
		0, 3, 4,
		0, 5, 6,
		0, 7, 8
	};

	Mesh* shuriken = new Mesh(name);
	shuriken->InitFromData(vertices, indices);
	return shuriken;

}

Mesh* Object2Dv2::CreateBow(std::string name, float rX, float rY)
{
	glm::vec3 color = glm::vec3(0.25, 0, 0);

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	// Creaza partea de jos a arcului.
	for (int i = -25; i <= 0; i++) {
		float point = M_PI / 2 * i / 25;
		vertices.emplace_back(glm::vec3(rX * cos(point), rY * sin(point), 0), color);
		indices.push_back(i + 25);
	}

	// Creaza partea de sus a arcului.
	for (int i = 1; i <= 25; i++) {
		float point = M_PI / 2 * i / 25;
		vertices.emplace_back(glm::vec3(rX * cos(point), rY * sin(point), 0), color);
		indices.push_back(i + 25);
	}

	indices.push_back(50);
	// Am creat punctele ca mai sus pentru a avea centrul "cercului imaginar" in originea ecranului la crearea acestuia. 

	Mesh* bow = new Mesh(name);
	bow->SetDrawMode(GL_LINE_LOOP);
	bow->InitFromData(vertices, indices);
	return bow;

}

Mesh* Object2Dv2::CreateBaloon(std::string balloon_name, glm::vec3 center, float rX, float rY, bool colorRed, Mesh* balloon_triangle, Mesh* balloon_line)
{
	glm::vec3 color, colorTriangle;

	// Daca colorRed este "true" atunci face culoarea balonului rosie altfel o face galbena
	if (colorRed) {
		color = glm::vec3(1, 0, 0);
		colorTriangle = glm::vec3(0.75, 0, 0);
	}
	else {
		color = glm::vec3(1, 1, 0);
		colorTriangle = glm::vec3(0.75, 0.75, 0);
	}

	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;

	// 
	vertices.emplace_back(center, color);

	for (int i = 0; i < 100; i++) {
		float point = M_PI * 2 * i / 100;
		vertices.emplace_back(glm::vec3(rX * cos(point), rY * sin(point), 0), color);
		indices.push_back(i);
	}
	indices.push_back(100);
	indices.push_back(1);

	std::vector<VertexFormat> vertices_balloon_triangle =
	{
		VertexFormat(glm::vec3(0, -5 - 50, 0), colorTriangle),
		VertexFormat(glm::vec3(5, -5 - 50, 0), colorTriangle),
		VertexFormat(glm::vec3(0, 5 - 50, 0), colorTriangle),
		VertexFormat(glm::vec3(-5, -5 - 50, 0), colorTriangle),
	};

	std::vector<unsigned short> indices_balloon_triangle =
	{
		0, 1, 2,
		0, 2, 3
	};

	std::vector<VertexFormat> vertices_line =
	{
		VertexFormat(glm::vec3(0, -50, 0), glm::vec3(1, 1, 1)),
		VertexFormat(glm::vec3(-5, -60, 0), glm::vec3(1, 1, 1)),
		VertexFormat(glm::vec3(5, -70, 0), glm::vec3(1, 1, 1)),
		VertexFormat(glm::vec3(-5, -80, 0), glm::vec3(1, 1, 1)),
		VertexFormat(glm::vec3(10, -90, 0), glm::vec3(1, 1, 1)),

	};

	std::vector<unsigned short> indices_line =
	{
		0, 1, 2, 3, 4
	};

	// Balonul este facut cu triangle_fan pentru ca centrul centrul balonului sa fie creat in originea ecranului
	Mesh* balloon = new Mesh(balloon_name);
	balloon->SetDrawMode(GL_TRIANGLE_FAN);
	balloon->InitFromData(vertices, indices);

	// Linia este strasa simplu
	balloon_line->SetDrawMode(GL_LINE_STRIP);
	balloon_line->InitFromData(vertices_line, indices_line);

	balloon_triangle->InitFromData(vertices_balloon_triangle, indices_balloon_triangle);

	return balloon;
}

Mesh* Object2Dv2::CreateArrow(std::string rectangle_name, bool powerBar, float length, float triangle_high, float rectangle_high, Mesh* triangle)
{
	glm::vec3 color_triangle = glm::vec3(1, 1, 1);
	glm::vec3 color_rectangle;

	if (!powerBar) {
		color_rectangle = glm::vec3(0.25, 0, 0);
	}
	else {
		color_rectangle = glm::vec3(0, 0, 0);
	}

	std::vector<VertexFormat> vertices_rectangle =
	{
		VertexFormat(glm::vec3(-length / 2, -rectangle_high / 2, 0), color_rectangle),
		VertexFormat(glm::vec3(length / 2, -rectangle_high / 2, 0), color_rectangle),
		VertexFormat(glm::vec3(length / 2, rectangle_high / 2, 0), color_rectangle),
		VertexFormat(glm::vec3(-length / 2, rectangle_high / 2, 0), color_rectangle)
	};


	std::vector<unsigned short> indices_rectangle =
	{
		0, 1, 2,
		0, 2, 3
	};

	if (!powerBar) {

		std::vector<VertexFormat> vertices_triangle =
		{
			VertexFormat(glm::vec3(length / 2, 0, 0), color_triangle),
			VertexFormat(glm::vec3(length / 2, -triangle_high / 2, 0), color_triangle),
			VertexFormat(glm::vec3(length / 2 + triangle_high, 0, 0), color_triangle),
			VertexFormat(glm::vec3(length / 2, triangle_high / 2, 0), color_triangle)
		};

		std::vector<unsigned short> indices_triangle =
		{
			0, 1, 2,
			0, 2, 3
		};

		triangle->InitFromData(vertices_triangle, indices_triangle);
	}

	Mesh* rectangle = new Mesh(rectangle_name);
	rectangle->InitFromData(vertices_rectangle, indices_rectangle);

	return rectangle;
}

Mesh* Object2Dv2::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}