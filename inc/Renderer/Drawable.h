#ifndef PAINTER_H_
#define PAINTER_H_
#include "Renderer/Buffer.h"
#include "Renderer/Graphics.h"
namespace Renderer {

class Context;

class Drawable {
public:
	Drawable() {}
	virtual void onDraw(Context &) = 0;
	virtual ~Drawable() = default;
};

}

#endif /* PAINTER_H_ */
