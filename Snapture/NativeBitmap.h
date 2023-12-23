#pragma once
#include <vector>

struct NativeBitmap {
	int height, width;
	std::vector<uint8_t> Buffer;
};