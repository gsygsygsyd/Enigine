#pragma once

#include <GLEW/glew.h>

class Framebuffer
{
public:
	void Init()
	{
		GLuint FBO;
		glCreateFramebuffers(1, &FBO);

		
		glCreateRenderbuffers(1, );
		glNamedBufferStorage(DepthBuf, );

	};

	GLuint FBO, DepthBuf, PosBuf, NormBuf, ColorBuf;

	GLuint Width, Height;
};