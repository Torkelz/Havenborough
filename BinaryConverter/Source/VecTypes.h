#pragma once

struct uivec4
{
	unsigned int x, y, z, w;

	uivec4() {}
	uivec4(unsigned int p_X, unsigned int p_Y, unsigned int p_Z, unsigned int p_W)
		:	x(p_X),
			y(p_Y),
			z(p_Z),
			w(p_W)
	{}
};
