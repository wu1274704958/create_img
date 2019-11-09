#include <ft2build.h>
#include <freetype/freetype.h>
#include <iostream>
#include <thread>
#include <dbg.hpp>


int main(int argc,char **argv) {
	const char* font_path = "C:\\Windows\\Fonts\\Raleway-Regular.ttf";
	
	if (argc > 1)
		font_path = argv[1];
	
	FT_Library ft_lib;

	FT_Init_FreeType(&ft_lib);
	
	FT_Face face;

	int v = 0;
	if ((v = FT_New_Face(ft_lib, font_path, 0, &face)) != 0)
	{
		
		return -1;
	}
	

	dbg(face->num_charmaps);
	dbg(face->num_faces);
	dbg(face->num_fixed_sizes);
	dbg(face->num_glyphs);


	FT_Done_Face(face);

	FT_Done_FreeType(ft_lib);

	return 0;
}