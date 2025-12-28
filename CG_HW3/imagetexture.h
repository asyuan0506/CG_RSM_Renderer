#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "headers.h"

// Texture Declarations.
class ImageTexture
{
public:
	// Texture Public Methods.
	ImageTexture(const std::string filePath);
	ImageTexture(const float ka_r, const float ka_g, const float ka_b); // Create a 1x1 texture with the given color Kd.
	~ImageTexture();

	void Bind(GLenum textureUnit);
	void Preview();
	std::string GetPath() const { return texFilePath; }

private:
	// Texture Private Data.
	std::string texFilePath;
	GLuint textureObj;
	int imageWidth;
	int imageHeight;
	int numChannels;
	cv::Mat texImage;
};

#endif

