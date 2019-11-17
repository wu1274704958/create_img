#include "ft2pp.hpp"
#include <iostream>
#include <thread>
#include <dbg.hpp>
#include <surface.hpp>
#include <gray.hpp>

using namespace wws;
using namespace ft2;

template <typename Cnt,typename Pit>
int render_text(surface<Cnt>& sur, FT_GlyphSlot gs,Pit c,int bx = 0,int by = 0);

int main(int argc,char **argv) {
	const char* font_path = "C:\\Windows\\Fonts\\simhei.ttf";
	
	if (argc > 1)
		font_path = argv[1];
	
	Library lib;

	Face face = lib.load_face<Face>(font_path);

	int x = 0;
	
	face.set_pixel_size(30, 30);
	
	surface<cmd_content> sur(60, 30);


	face.load_glyph(L'1');

	x += face.render_surface(sur,&CmdSurface::set_pixel, x, 0,'*');

	face.load_glyph(L'2');

	x += face.render_surface(sur, &CmdSurface::set_pixel, x, 0, '*');

	face.load_glyph(L'3');

	x += face.render_surface(sur, &CmdSurface::set_pixel, x, 0, '*');

	sur.present(std::cout);

	system("pause");

	return 0;
}

template<typename Cnt,typename Pit>
int render_text(surface<Cnt>& sur, FT_GlyphSlot gs,Pit c,int bx,int by)
{
	auto bits = &gs->bitmap;
	constexpr int CS = sizeof(char) * 8;

	int a = gs->bitmap_left + bx;
	int b = (gs->face->size->metrics.y_ppem) - bits->rows + by;

	/*for (int i = 0; i < bits->rows * bits->pitch; ++i)
	{
		printBin(bits->buffer[i],false);
		if ((i + 1) % bits->pitch == 0)
			std::cout << "\n";
	}*/

	for (int y = 0; y < bits->rows; ++y)
	{
		for (int x = 0; x < bits->pitch * CS; ++x)
		{
			if ((bits->buffer[(y * bits->pitch) + (x / CS)] << (x % CS)) & 0x80)
			{
				sur.set_pixel(a + x, b + y, c);
			}
		}
	}

	return gs->advance.x / 64;
}
