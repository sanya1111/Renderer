#ifndef PAINTER_H_
#define PAINTER_H_
#include "Renderer/Buffer.h"
#include "Renderer/Graphics.h"
namespace Renderer {


class Drawable {
public:
	Drawer drawer;
	bool flag_finit;
	Drawable();
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer &) = 0;
	virtual ~Drawable() = default;
};

}

#endif /* PAINTER_H_ */
