#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <iostream>
#include <thread>
#include <dbg.hpp>
#include <surface.hpp>
#include <gray.hpp>

using namespace wws;

template <typename Cnt,typename Pit>
int render_text(surface<Cnt>& sur, FT_GlyphSlot gs,Pit c,int bx = 0,int by = 0);

int main(int argc,char **argv) {
	const char* font_path = "C:\\Windows\\Fonts\\SIMYOU.TTF";
	
	if (argc > 1)
		font_path = argv[1];
	
	FT_Library ft_lib;

	FT_Init_FreeType(&ft_lib);
	
	FT_Face face;

	int v = 0;
	if ((v = FT_New_Face(ft_lib, font_path, 0, &face)) != 0)
	{
		FT_Done_FreeType(ft_lib);
		return -1;
	}
	int x = 0;
	//FT_Select_Charmap(face, FT_ENCODING_GB2312);
	FT_Set_Pixel_Sizes(face, 30, 30);
	
	surface<cmd_content> sur(60, 30);


	FT_UInt n1 = FT_Get_Char_Index(face,L'1');

	FT_Load_Glyph(face, n1, FT_LOAD_DEFAULT);

	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
	
	
	x = render_text(sur, face->glyph,'*',x,0);

	n1 = FT_Get_Char_Index(face, L'2');

	FT_Load_Glyph(face, n1, FT_LOAD_DEFAULT);

	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);

	render_text(sur, face->glyph, '*', x, 0);

	sur.present(std::cout);

	FT_Done_Face(face);

	FT_Done_FreeType(ft_lib);
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
