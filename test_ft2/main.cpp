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

#include <Windows.h>
#undef max
#undef min

using namespace wws;
using namespace ft2;
using namespace cgm;

void go_to_xy(int x,int y)
{
	HANDLE hout;
	COORD coord;
	coord.X = x;
	coord.Y = y;
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hout, coord);
}

void set_text(surface<cmd_content>& sur, Face& f, std::string s);

struct point {
	vec2 pos;
	vec2 v;
	vec2 tar;
};

int main(int argc,char **argv) {
	const char* font_path = "C:\\Windows\\Fonts\\simhei.ttf";
	
	if (argc > 1)
		font_path = argv[1];
	
	Library lib;

	Face face = lib.load_face<Face>(font_path);

	int x = 0;
	srand(time(nullptr));
	face.set_pixel_size(30, 30);

	surface<cmd_content> sur(60, 30);
	surface<cmd_content> back(60, 30);

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
		use.push_back(std::move(out[0]));
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
		constexpr int MaxW = 8;
		constexpr int M = 5;
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
			int res_y = (rand() % 2) == 0 ? -1 : sur.h();
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
			if (std::abs(p->pos.x() - p->tar.x()) < 1.0 || std::abs(p->pos.y() - p->tar.y()) < 1.0)
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
		sur.clear();
		for (auto& p : use) {
			sur.set_pixel(static_cast<int>(std::roundf(p->pos.x())), static_cast<int>(std::roundf(p->pos.y())),'*');
		}
		for (auto& p : out) {
			sur.set_pixel(static_cast<int>(std::roundf(p->pos.x())), static_cast<int>(std::roundf(p->pos.y())),'*');
		}
	};
	
	auto& p = get_out_to_use();
	p->pos = vec2{ 0.0f,0.0f };//rd_out_pos(0, 0);
	p->v = vec2{ 1.0f,1.0f }.unitized();
	p->tar = vec2{ 30.f,30.f };

	while (true)
	{
		go_to_xy(0, 0);
		set_text(back, face, wws::to_string(s));

		fill();
		sur.present(std::cout);
		step();
		Sleep(30);
	}
	

	system("pause");

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
	sur.present(std::cout);
}