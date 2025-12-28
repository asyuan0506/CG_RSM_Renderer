#ifndef G_BUFFER_H
#define G_BUFFER_H

#include "headers.h"

class G_Buffer {
	public:
	// GBuffer Public Methods.
	G_Buffer(const int&, const int&);
	~G_Buffer();

	unsigned int GetGBuffer() const { return gBuffer; }
	unsigned int GetGPosition() const { return gPosition; }
	unsigned int GetGNormal() const { return gNormal; }
	unsigned int GetGAlbedoSpec() const { return gAlbedoSpec; }
	unsigned int GetAttachment(const int index) const { return attachments[index]; }
	void Draw();

	private:
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec;
	unsigned int attachments[3];
	unsigned int rboDepth;
};

#endif // !G_BUFFER_H
