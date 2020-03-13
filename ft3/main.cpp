#include "ft2pp.hpp"
#include <iostream>
#include <thread>
#include <dbg.hpp>
#include <surface.hpp>
#include <gray.hpp>
#include <string>
#include "serialization.hpp"
#include <matrix2.hpp>
#include <memory>
#include <random>
#include <chrono>
#include <math.h>
#include <thread>
#include <ani_surface.hpp>

#ifdef _MSC_VER

#include <Windows.h>
#undef max
#undef min

#else

#endif  


using namespace wws;
using namespace ft2;


void set_text(surface<cmd_content>& sur, Face& f, std::string s,char pt);

struct Drive : public ASDrive<cmd_content>
{
	Drive(Face& f,std::wstring s) : face(f) , str(std::move(s)){}
	bool is_end()
	{
		return p > str.size();
	}
	void set_text(surface<cmd_content>& sur,char f_c) override
	{
		if(p == str.size())
		{
			face.set_pixel_size(60,60);
			::set_text(sur,face,"@_@",f_c);
		}else{
			face.load_glyph(str[p]);
			CenterOffEx custom;
			face.render_surface(sur,custom, &CmdSurface::set_pixel, 0, 0, f_c);
		}
	}
	void step() override
	{
		++p;
	}
	std::ostream& get_present() override
	{
		return std::cout;
	}
	int p = 0;
	std::wstring str;
	Face& face;
};




int main(int argc,char **argv) {
	const char* font_path = "res/fonts/SIMHEI.TTF";
	int sur_w = 90;
	if (argc > 1)
		font_path = argv[1];
	if (argc > 2)
		sur_w = wws::parser<int>(argv[2]);
	Library lib;

	Face face;

	try
	{
		dbg(font_path);
		face = lib.load_face<Face>(font_path);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
	

	srand(time(nullptr));
	face.set_pixel_size(90, 90);
	face.select_charmap(FT_ENCODING_UNICODE);

	auto drive = std::make_shared<Drive>(face,L"奥斯卡大奖离开撒娇的凉快撒角度来看");

	AniSurface as(90,90,drive.get(),'#',100);

	as.move_to_func = [](cgm::vec2& pos,cgm::vec2 v,cgm::vec2 tar)
	{
		auto len = (tar - pos).len() * 0.1f;
		pos = pos + ( v * len);
	};

	as.go();
	
#ifdef _MSC_VER
	//system("shutdown /s /t 300");
#endif 
	return 0;
}

void set_text(surface<cmd_content>& sur, Face& f, std::string s,char pt)
{
	int x = 0;
	for (auto c : s)
	{
		f.load_glyph(c);
		CenterOff custom;
		x += f.render_surface(sur,custom, &CmdSurface::set_pixel, x, 0, pt);
	}
}