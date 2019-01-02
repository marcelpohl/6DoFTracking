#ifndef PHONG_H
#define PHONG_H

#include "Shader.h"

class Phong : public Shader
{
public:
	Phong(const GLchar* vertexPath, const GLchar* fragmentPath);
	~Phong();

	void update();
};

#endif