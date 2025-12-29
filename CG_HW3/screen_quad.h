#ifndef SCREEN_QUAD_H
#define SCREEN_QUAD_H

#include "headers.h"

class ScreenQuad {
public:
	// ScreenQuad Public Methods.
	ScreenQuad();
	~ScreenQuad();

	void CreatBuffer();
	void Render();

private:
	GLuint vbo_id_;
	GLuint vao_id_;
	std::vector<float> quad_vertices;
};

#endif // !G_BUFFER_H
