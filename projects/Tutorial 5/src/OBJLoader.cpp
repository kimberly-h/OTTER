#include "OBJLoader.h"
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>

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

VertexArrayObject::sptr OBJLoader::FileLoader(const std::string& filename)
{

	// If our file fails to open, we will throw an error
	std::ifstream in_file(filename);
		
	if (!in_file) 
	{
		throw std::runtime_error("Failed to open file");
	}

	MeshBuilder<VertexPosNormTex> mesh;

	std::vector<glm::fvec3> posVert;
	std::vector<glm::fvec2> texVert;
	std::vector<glm::fvec3> normVert;

	//Face vectors
	std::vector<GLint> posInd;
	std::vector<GLint> texInd;
	std::vector<GLint> normInd;

	std::stringstream ss;
	std::string line = "";
	std::string prfx = ""; //The prefix
	glm::vec3 temp_vec3;
	glm::vec2 temp_vec2;
	GLint temp_glint = 0;

	// Load the file
	while (std::getline(in_file, line))
	{
		//Get line prefix
		ss.clear();
		ss.str(line);
		ss >> prfx;

		if (prfx == "#") //It finds the lines that starts with a # then it is a comment
		{
			// Skips the comment line
		}

		else if (prfx == "v") // Verticies Position
		{
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			posVert.push_back(temp_vec3);
		}

		else if (prfx == "vt") // Verticies Texture
		{
			ss >> temp_vec2.x >> temp_vec2.y;
			texVert.push_back(temp_vec2);
		}

		else if (prfx == "vn") // Verticies Normals
		{
			ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
			normVert.push_back(temp_vec3);
		}
		else if (prfx == "f") // Indices
		{
			int indType = 0; //Index Type

			while (ss >> temp_glint)
			{
				//Push indices
				if (indType == 0) 
					posInd.push_back(temp_glint);
				else if (indType == 1)
					texInd.push_back(temp_glint);
				else if (indType == 2)
					normInd.push_back(temp_glint);

				//Check if next is special character or space. Ignore if it is. 
				if (ss.peek() == '/')
				{
					++indType;
					ss.ignore(1, '/'); //it looks for the "/" to move on to the next one
				}
				else if (ss.peek() == ' ')
				{
					++indType;
					ss.ignore(1, ' ');
				}

				//Reset counterr
				if (indType > 2) //if index type is larger than 2, makes it into 0
					indType = 0;
			}
		}

		//Vertices to mesh
		for (size_t i = 0; i < posInd.size(); ++i)
		{
			//For loop that runs for the amount of indicies there are and adding them into the mesh from the vertex which includes the position, normal and texture values
			VertexPosNormTex v = VertexPosNormTex(posVert[posInd  [i] - 1], normVert[normInd[i] - 1], texVert[texInd  [i] - 1]);
			
			mesh.AddIndex(mesh.AddVertex(v));
		}

	}

	return mesh.Bake(); //baking this into a VAO
}

