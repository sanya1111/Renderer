#include "Renderer/Graphics.h"

/*void Renderer::Graphics::fill(Buffer::DrawBuffer* buf,
		const Buffer::DrawBuffer::Rgb& color) {
	for (int32_t i = 0; i < buf->height; i++) {
		for (int32_t j = 0; j < buf->width; j++) {
			buf->at(i, j).set(color);
		}
	}
}

void Renderer::Graphics::draw_line(Buffer::DrawBuffer* buf,
		const Geom::Line<int32_t>& arg, const Buffer::DrawBuffer::Rgb& color) {
	Geom::Line<int32_t>  what = arg;
	bool swapped = false;
	if(std::abs(what.begin.x - what.end.x) < std::abs(what.begin.y - what.end.y)){
		swapped = true;
		std::swap(what.begin.x, what.begin.y);
		std::swap(what.end.x, what.end.y);
	}
	int32_t dx = -what.begin.x +  what.end.x;
	int32_t dy = -what.begin.y + what.end.y;
	int32_t delta_x = Geom::sign(dx);
	int32_t delta_y = Geom::sign(dy);
	int32_t every = 0;
	if(delta_y)
		every = std::max(1, abs(dx/ dy));
	int32_t cury = what.begin.y;
	for(int32_t i = what.begin.x, j = 0; i != what.end.x; i += delta_x, j++){
		if(every && j % every == 0){
			cury += delta_y;
		}
		if(!swapped){
			buf->at(i, cury).set(color);
		} else {
			buf->at(cury, i).set(color);
		}
	}
}

void Renderer::Graphics::draw_triangle(Buffer::DrawBuffer* buf,
		const Geom::Triangle<int32_t>& what,
		const Buffer::DrawBuffer::Rgb& color) {
	for(int8_t i = 0; i < 3; i++){
		draw_line(buf, what.lines[i], color);
	}
}
*/
