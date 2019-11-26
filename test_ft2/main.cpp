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

#ifdef _MSC_VER

#include <Windows.h>
#undef max
#undef min

#else

#endif  


using namespace wws;
using namespace ft2;
using namespace cgm;

void go_to_xy(int x,int y)
{
#ifdef _MSC_VER
	HANDLE hout;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hout, coord);
#else
	printf("%c[%d;%df", 0x1B, y, x);
#endif
}

void set_text(surface<cmd_content>& sur, Face& f, std::string s);

struct point {
	vec2 pos;
	vec2 v;
	vec2 tar;
	point() : pos({ -1.f,-1.f }) {
		
	}
};

int main(int argc,char **argv) {
	const char* font_path = "res/fonts/SIMHEI.TTF";
	int sur_w = 60;
	if (argc > 1)
		font_path = argv[1];
	if (argc > 2)
		sur_w = wws::parser<int>(argv[2]);
	Library lib;

	Face face;
	try
	{
		face = lib.load_face<Face>(font_path);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
	

	int x = 0;
	srand(time(nullptr));
	face.set_pixel_size(60, 60);

	surface<cmd_content> sur( sur_w, 60);
	surface<cmd_content> last(sur_w, 60);
	surface<cmd_content> back(sur_w, 60);

	int s = 90;
	
	std::vector<std::unique_ptr<point>> use;
	std::vector<std::unique_ptr<point>> out;

	use.reserve(100);
	out.reserve(100);

	auto get_out_to_use = [&out,&use]()->std::unique_ptr<point>&
	{
		if (out.empty())
		{
			use.push_back(std::unique_ptr<point>(new point()));
			return use.back();
		}
		use.push_back(std::move(out.back()));
		out.pop_back();
		return use.back();
	};

	auto get_use_to_out = [&out,&use](int x,int y)->std::unique_ptr<point>&
	{
		auto it = use.end();
		for (it = use.begin(); it != use.end(); ++it)
		{
			auto& p = *it;
			if ((p->pos.x() == x && p->pos.y() == y) || (p->tar.x() == x && p->tar.y() == y))
			{
				break;
			}
		}
		if(it != use.end())
		{
			out.push_back(std::move(*it));
			use.erase(it);
			return out.back();
		}
		else {
			throw std::runtime_error("Not found!");
		}
	};

	auto rd_out_pos = [&sur](int x,int y)->vec2 {
		constexpr int MaxW = 16;
		constexpr int M = 10;
		if (x < MaxW || (sur.w() - x) < MaxW )
		{
			int res_x = x < MaxW ? -1 : sur.w();
			int off = rand() % M;
			if (y + off < sur.h())
				return vec2{ static_cast<float>(res_x) ,static_cast<float>(y + off) };
			else 
				return vec2{ static_cast<float>(res_x) ,static_cast<float>(y - off) };
		}
		else {
			int res_y = y > (sur.h() / 2) ? sur.h() : -1;//(rand() % 2) == 0 ? -1 : sur.h();
			int off = rand() % M;

			if (x + off < sur.w())
				return vec2{ static_cast<float>(x + off), static_cast<float>(res_y)  };
			else
				return vec2{ static_cast<float>(x - off), static_cast<float>(res_y)  };
		}
	};

	auto step_unit = [](std::unique_ptr<point>& p) {
		if (p->pos.x() != p->tar.x() || p->pos.y() != p->tar.y())
		{
			if (std::abs(p->pos.x() - p->tar.x()) < 1.0 && std::abs(p->pos.y() - p->tar.y()) < 1.0)
			{
				p->pos = p->tar;
			}
			else
			{
				p->pos = p->pos + p->v;
			}
		}
		else {
			p->v.x() = 0.0f;
			p->v.y() = 0.0f;
		}
	};

	auto step = [&use, &out,step_unit]() {
		for (auto& p : use) {
			step_unit(p);
		}
		for (auto& p : out) {
			step_unit(p);
		}
	};

	auto fill = [&use,&out,&sur]() {
		for (auto& p : use) {
			sur.set_pixel(static_cast<int>(std::roundf(p->pos.x())), static_cast<int>(std::roundf(p->pos.y())),'*');
		}
		for (auto& p : out) {
			sur.set_pixel(static_cast<int>(std::roundf(p->pos.x())), static_cast<int>(std::roundf(p->pos.y())),'*');
		}
	};

	set_text(back, face, wws::to_string(s));

	auto now = std::chrono::system_clock::now();
	auto start = std::chrono::system_clock::now();
	
	bool alread_set = true;

	while (s >= 0)
	{
		go_to_xy(0, 0);
		sur.clear();
		if (alread_set)
		{
			alread_set = false;
			for (int y = 0; y < last.h(); ++y)
			{
				for (int x = 0; x < last.w(); ++x)
				{
					if (back.get_pixel(x, y) != ' ' && last.get_pixel(x, y) == ' ')
					{
						auto& p = get_out_to_use();
						if (!sur.good_pos(static_cast<int>(p->pos.x()), static_cast<int>(p->pos.y())))
							p->pos = rd_out_pos(x, y);
						p->tar = vec2{ static_cast<float>(x),static_cast<float>(y) };
						p->v = (p->tar - p->pos).unitized() * (static_cast<float>((rand() % 10) + 4) * 0.1f);
					}
					else
					if (back.get_pixel(x, y) == ' ' && last.get_pixel(x, y) != ' ')
					{
						auto& p = get_use_to_out(x, y);
						p->pos = vec2{ static_cast<float>(x),static_cast<float>(y) };
						p->tar = rd_out_pos(x, y);
						p->v = (p->tar - p->pos).unitized() * (static_cast<float>((rand() % 10) + 4) * 0.1f);
					}
				}
			}
		}

		fill();
		sur.present(std::cout);
		step();
		auto end2 = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start).count();
		start = std::chrono::system_clock::now();
		if (duration < 18)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(18 - duration));
		}

		auto end = std::chrono::system_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(end - now).count() >= 3)
		{
			--s;
			back.swap(last);
			back.clear();
			set_text(back, face, wws::to_string(s));
			alread_set = true;
			now = std::chrono::system_clock::now();
		}
	}
	
#ifdef _MSC_VER
	system("shutdown /s /t 300");
#endif 
	return 0;
}

void set_text(surface<cmd_content>& sur, Face& f, std::string s)
{
	int x = 0;
	for (auto c : s)
	{
		f.load_glyph(c);
		x += f.render_surface(sur, &CmdSurface::set_pixel, x, 0, '*');
	}
}