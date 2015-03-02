#pragma once
class RGBA{
public:
	int r, g, b, a;
	bool alpha;
	RGBA(int _r, int _g, int _b, int _a) : r(_r), g(_g),
			b(_b), a(_a), alpha(true) {}
	RGBA(int _r, int _g, int _b) : r(_r), g(_g),
			b(_b), a(-1), alpha(false) {}
};



