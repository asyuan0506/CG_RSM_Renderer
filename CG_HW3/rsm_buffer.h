#ifndef RSM_BUFFER_H
#define RSM_BUFFER_H

#include "headers.h"

class RSMBuffer {
public:
	// GBuffer Public Methods.
	RSMBuffer();
	~RSMBuffer();

	unsigned int GetRSMBuffer() const { return rsm_buffer_; }
	unsigned int GetRSMPos() const { return rsm_pos_; }
	unsigned int GetRSMNormal() const { return rsm_normal_; }
	unsigned int GetRSMFlux() const { return rsm_flux_; }
	unsigned int GetRSMDepth() const { return rsm_depth_; }
	unsigned int GetAttachment(const int index) const { return attachments[index]; }
	int GetRSMSize() const { return RSM_SIZE; }

private:
	unsigned int rsm_buffer_;
	unsigned int rsm_pos_, rsm_normal_, rsm_flux_, rsm_depth_;
	unsigned int attachments[4];
	const int RSM_SIZE = 1024;
	// unsigned int rboDepth;
};

#endif // !G_BUFFER_H
