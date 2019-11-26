#pragma once
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

#include <iostream>

namespace ft2 {
	class Library {
	public:
		Library() {
			if (InsteNum >= 1)
			{
				throw std::runtime_error("Has an instance!");
			}
			++InsteNum;
			FT_Init_FreeType(&lib);
		}
		~Library() {
			--InsteNum;
			FT_Done_FreeType(lib);
		}
		Library(const Library&) = delete;
		Library(Library&&) = delete;
		Library& operator=(const Library&) = delete;
		Library& operator=(Library&&) = delete;

		template<typename T>
		T load_face(const char* path, int idx = 0)
		{
			return T(lib,path,idx);
		}

	private:
		FT_Library lib = nullptr;
		static int InsteNum;
	};

	inline int Library::InsteNum = 0;

	class Face {
	public:
		Face() {

		}
		Face(FT_Library lib,const char*path,int idx) {
			int error;
			if ((error = FT_New_Face(lib, path, idx, &face)) == FT_Err_Unknown_File_Format)
			{
				throw std::runtime_error("Unknown File Format");
			}
			else if (error == FT_Err_Cannot_Open_Resource)
			{
				throw std::runtime_error("Cannot Open Resource");
			}
		}
		Face(const Face&) = delete;
		Face(Face&& oth) {
			face = oth.face;
			oth.face = nullptr;
		};
		Face& operator=(const Face&) = delete;
		Face& operator=(Face&& oth)
		{
			if (face)
				FT_Done_Face(face);
			face = oth.face;
			oth.face = nullptr;
			return *this;
		}
		~Face() {
			if(face)
				FT_Done_Face(face);
		}

		void set_pixel_size(uint32_t w,uint32_t h) {
			if(face)
				FT_Set_Pixel_Sizes(face, w, h);
		}

		void select_charmap(FT_Encoding_ cm)
		{
			if (face)
				FT_Select_Charmap(face, cm);
		}
		
		void load_glyph(FT_ULong c, uint32_t load_flag = FT_LOAD_DEFAULT, FT_Render_Mode render_flag = FT_RENDER_MODE_MONO)
		{
			if (face)
			{
				uint32_t n1 = FT_Get_Char_Index(face, c);
				FT_Load_Glyph(face, n1, load_flag);
				FT_Render_Glyph(face->glyph, render_flag);
			}
		}

		FT_GlyphSlot glyph_slot()
		{
			return face->glyph;
		}

		template<typename Sur, typename Ret,typename ...Oth>
		int render_surface(Sur& sur, Ret(Sur::* set_pixel)(int,int,Oth...) ,int bx,int by,Oth&&...oth)
		{
			if (!face)
				return 0;
			auto gs = glyph_slot();
			auto bits = &gs->bitmap;
			constexpr int CS = sizeof(char) * 8;

			/*FT_Glyph glyph;
			FT_Get_Glyph(gs, &glphy);

			FT_BBox box;
			FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &box);*/

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
						(sur.*set_pixel)(a + x, b + y,std::forward<Oth>(oth)...);
					}
				}
			}

			return gs->advance.x / 64;
		}
		

	private:
		FT_Face face = nullptr;
	};

}