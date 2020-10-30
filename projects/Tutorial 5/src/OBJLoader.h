#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "MeshBuilder.h"
#include "MeshFactory.h"

class OBJLoader
{
public:
	static VertexArrayObject::sptr FileLoader(const std::string& filename);

protected:
	OBJLoader() = default;
	~OBJLoader() = default;
};
