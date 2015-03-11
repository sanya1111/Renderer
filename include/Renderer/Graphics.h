#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Renderer/Buffer.h"
#include "Renderer/Geom.hpp"

namespace Renderer {

namespace Graphics {
//functions
void fill(Buffer::DrawBuffer * buf, const Buffer::DrawBuffer::Rgb &color);
void draw_line(Buffer::DrawBuffer * buf, const Geom::Line<int32_t> &what,
		const Buffer::DrawBuffer::Rgb& color);
void draw_triangle(Buffer::DrawBuffer * buf, const Geom::Triangle<int32_t> &what,
		const Buffer::DrawBuffer::Rgb & color);

}

}

#endif /* GRAPHICS_H_ */
