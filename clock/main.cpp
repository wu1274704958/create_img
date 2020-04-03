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
#include <chrono>
#include <iomanip>
#include <strstream>

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
	Drive(Face& f) : face(f)
	{
		str = get_time_str(has_p);
	}

	std::string get_time_str(bool has_p)
	{
		std::strstream ss; 
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		
    	auto now_c = std::chrono::system_clock::to_time_t(now);
        auto t = std::localtime(&now_c);
		ss <<  
			(t->tm_hour < 10 ? "0" : "") << 
			t->tm_hour << (has_p ? ':' : ' ') << 
			(t->tm_min < 10 ? "0" : "") << 
			t->tm_min << ':' << 
			(t->tm_sec < 10 ? "0" : "") << 
			t->tm_sec << '\0';
		return ss.str();
	}

	bool is_end()
	{
		return false;
	}
	void set_text(surface<cmd_content>& sur,char f_c) override
	{
		{
			::set_text(sur,face,str,f_c);
		}
	}
	void step() override
	{
		str = get_time_str(has_p);
	}
	std::ostream& get_present() override
	{
		return std::cout;
	}
	bool need_transfar(uint32_t ms,bool to_use_stable,bool to_out_stable) override 
	{
		bool f = to_use_stable && ms >= 1000;
		if(f)
		{
			has_p = !has_p;
		}
		return f;
	}
	bool has_p = true;
	std::string str;
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
	face.set_pixel_size(32, 32);
	face.select_charmap(FT_ENCODING_UNICODE);

	auto drive = std::make_shared<Drive>(face);

	AniSurface as(130,32,drive.get(),'#',300);
	as.to_out_speed = 0.09f;
	as.to_use_speed = 0.1f;

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