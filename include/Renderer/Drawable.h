#ifndef PAINTER_H_
#define PAINTER_H_
#include "Renderer/Log.h"
#include "Renderer/Exception.h"
#include "Renderer/Buffer.h"

namespace Renderer {

class DrawableInfo{
public:
	bool flag_finit;
	DrawableInfo();
};

class Drawable {
public:
	virtual void onDraw(Buffer::DrawBuffer *, DrawableInfo *) = 0;
	virtual ~Drawable() = default;
};

}

#endif /* PAINTER_H_ */
