#ifndef PAINTER_H_
#define PAINTER_H_
#include "Renderer/Log.h"
#include "Renderer/Exception.h"
#include "Renderer/Buffer.h"

namespace Renderer{

	class Drawable{
	protected:
		virtual void on_draw(Buffer::DrawBuffer &, int delta) = 0;
		virtual ~Drawable() = default;
	};
}



#endif /* PAINTER_H_ */
