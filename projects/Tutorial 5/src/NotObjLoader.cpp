#include "NotObjLoader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>

// Borrowed from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
#pragma region String Trimming 
//string trimming is not built into c++ so its taken from stackoverflow to cut out in the beginning and end of the line


// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

#pragma endregion 

VertexArrayObject::sptr NotObjLoader::LoadFromFile(const std::string& filename)
{
	// Open our file in binary mode
	std::ifstream file; //opens a file and make sure it exists
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	if (!file) {
		throw std::runtime_error("Failed to open file");
	}

	MeshBuilder<VertexPosNormTexCol> mesh;
	std::string line;

	// TODO: Load from file

	while (std::getline(file, line)) {
		trim(line);
		if (line.substr(0, 1) == "#") //it finds the lines that starts with a # then it is a comment
		{
			// Comment, no-op
		}

		else if (line.substr(0, 5) == "cube ") // We can do equality check this way since the left side is a string and not a char*
		{ //looks at the cube section 
			std::istringstream ss = std::istringstream(line.substr(5));
			glm::vec3 pos;
			ss >> pos.x >> pos.y >> pos.z;

			glm::vec3 scale;
			ss >> scale.x >> scale.y >> scale.z;

			glm::vec3 eulerDeg;
			ss >> eulerDeg.x >> eulerDeg.y >> eulerDeg.z;

			glm::vec4 color = glm::vec4(1.0f); //see if there are colours avaible/more data
			if (ss.rdbuf()->in_avail() > 0) {
				ss >> color.r >> color.g >> color.b;
			}
			if (ss.rdbuf()->in_avail() > 0) {
				ss >> color.a;
			}
			MeshFactory::AddCube(mesh, pos, scale, eulerDeg, color);
		}

		else if (line.substr(0, 6) == "plane ")
		{
			std::istringstream ss = std::istringstream(line.substr(6));
			glm::vec3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			glm::vec3 normal;
			ss >> normal.x >> normal.y >> normal.z;
			glm::vec3 tangent;
			ss >> tangent.x >> tangent.y >> tangent.z;
			glm::vec2 size;
			ss >> size.x >> size.y;
			glm::vec4 color = glm::vec4(1.0f);
			if (ss.rdbuf()->in_avail() > 0) {
				ss >> color.r >> color.g >> color.b;
			}
			if (ss.rdbuf()->in_avail() > 0) {
				ss >> color.a;
			}
			MeshFactory::AddPlane(mesh, pos, normal, tangent, size, color);

			// Note: with actual OBJ files you're going to run into the issue where faces are composited of different indices
			// You'll need to keep track of these and create vertex entries for each vertex in the face
			// If you want to get fancy, you can track which vertices you've already added
		}

		else if (line.substr(0, 7) == "sphere ")
		{
			std::istringstream ss = std::istringstream(line.substr(7));
			std::string mode;
			ss >> mode;
			int tesselation = 0;
			ss >> tesselation;
			glm::vec3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			glm::vec3 radii;
			ss >> radii.x >> radii.y >> radii.z;
			glm::vec4 color = glm::vec4(1.0f);
			if (ss.rdbuf()->in_avail() > 0) {
				ss >> color.r >> color.g >> color.b;
			}
			if (ss.rdbuf()->in_avail() > 0) {
				ss >> color.a;
			}
			// checks if it is an icosphere
			if (mode == "ico") {
				MeshFactory::AddIcoSphere(mesh, pos, radii, tesselation, color);
			}
			// if not then it goes to the uv 
			else if (mode == "uv") {
				MeshFactory::AddUvSphere(mesh, pos, radii, tesselation, color);
			}
		}

		return mesh.Bake(); //baking this into a VAO
	}
}