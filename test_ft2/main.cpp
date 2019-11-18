#include "ft2pp.hpp"
#include <iostream>
#include <thread>
#include <dbg.hpp>
#include <surface.hpp>
#include <gray.hpp>

using namespace wws;
using namespace ft2;


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