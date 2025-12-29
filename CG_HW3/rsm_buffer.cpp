#include "rsm_buffer.h"

RSMBuffer::RSMBuffer()
{
	glGenFramebuffers(1, &rsm_buffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, rsm_buffer_);

	// - position color buffer
	glGenTextures(1, &rsm_pos_);
	glBindTexture(GL_TEXTURE_2D, rsm_pos_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, RSM_SIZE, RSM_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rsm_pos_, 0);

	// - normal color buffer
	glGenTextures(1, &rsm_normal_);
	glBindTexture(GL_TEXTURE_2D, rsm_normal_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, RSM_SIZE, RSM_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rsm_normal_, 0);

	// - flux color buffer
	glGenTextures(1, &rsm_flux_);
	glBindTexture(GL_TEXTURE_2D, rsm_flux_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RSM_SIZE, RSM_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, rsm_flux_, 0);

	// - depth color buffer
	glGenTextures(1, &rsm_depth_);
	glBindTexture(GL_TEXTURE_2D, rsm_depth_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, RSM_SIZE, RSM_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// If sampling outside the shadow map, return 1.0 (far plane) ?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rsm_depth_, 0);
	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	attachments[2] = GL_COLOR_ATTACHMENT2;
	glDrawBuffers(3, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RSMBuffer::~RSMBuffer()
{
	glDeleteFramebuffers(1, &rsm_buffer_);
	glDeleteTextures(1, &rsm_pos_);
	glDeleteTextures(1, &rsm_normal_);
	glDeleteTextures(1, &rsm_flux_);
	glDeleteTextures(1, &rsm_depth_);
}
