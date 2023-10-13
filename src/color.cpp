#include "color.h"

unsigned int BlendRGBA(unsigned int sourceColor, unsigned int destColor, double alpha) {
	unsigned int sourceR = (sourceColor >> 16) & 0xFF;
	unsigned int sourceG = (sourceColor >> 8) & 0xFF;
	unsigned int sourceB = sourceColor & 0xFF;
	unsigned int sourceA = (sourceColor >> 24) & 0xFF;

	unsigned int destR = (destColor >> 16) & 0xFF;
	unsigned int destG = (destColor >> 8) & 0xFF;
	unsigned int destB = destColor & 0xFF;
	unsigned int destA = (destColor >> 24) & 0xFF;

	unsigned int blendedR = static_cast<unsigned int>((sourceR * alpha) + (destR * (1.0f - alpha)));
	unsigned int blendedG = static_cast<unsigned int>((sourceG * alpha) + (destG * (1.0f - alpha)));
	unsigned int blendedB = static_cast<unsigned int>((sourceB * alpha) + (destB * (1.0f - alpha)));
	unsigned int blendedA = static_cast<unsigned int>((sourceA * alpha) + (destA * (1.0f - alpha)));

	unsigned int blendedColor =
		(blendedA << 24) | (blendedR << 16) | (blendedG << 8) | blendedB;

	return blendedColor;
}