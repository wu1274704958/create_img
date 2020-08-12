#include <json.hpp>
#include <filesystem>
#include <sundry.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include "comm_in.hpp"
#include <dbg.hpp>
#include <find_path.hpp>

namespace fs = std::filesystem;

int fill(wws::Json& config,std::vector<fs::path>& back,wws::rgba_content& out, int dist, int size, int x,int y, int w, int h);

int main(int argc,char **argv)
{
	if (argc < 2)
	{
		std::cerr << "args error! tips: dir [dist] [size]" << std::endl;
		return -1;
	}
	int dist = 1;
	std::string dir(argv[1]);
	if (argc >= 3)
		dist = wws::parser<int>(argv[2]);

	int size = 2048;
	if (argc >= 4)
		size = wws::parser<int>(argv[3]);

	std::cout << "dir " << dir << " dist " << dist << " size " << size << std::endl;

	fs::path root(dir);

	if (!fs::exists(root))
	{
		std::cerr << "folder not exists!" << std::endl;
		return -1;
	}

	
	wws::Json config;
	std::vector<fs::path> back;

	wws::enum_path(root, [&back](const fs::path& f) {
		if (f.extension() == ".png" || f.extension() == ".jpg")
		{
			back.push_back(f);
		}
	});
	

	wws::rgba_content out(size, size);
	out.init();
	//getchar();
	int err = fill(config,back, out,dist,size,0,0,size,size);

	dbg(back.size());

	stbi_write_png("out.png", out.width(), out.height(), 4, out.get(), 0 );

	return 0;
}

int fill(wws::Json& config, std::vector<fs::path>& back, wws::rgba_content& out,int dist,int size, int bx, int by, int w, int h)
{
	if (back.empty()) return -1;
	
	for (int i = 0; i < back.size(); ++i)
	{
		auto& f = back[i];

		auto f_str = f.generic_string();

		int width, height, nrComponents;
		unsigned char* data = stbi_load(f_str.c_str(), &width, &height, &nrComponents, 0);

		if (w >= width && h >= height)
		{
			dbg(std::make_tuple(bx, by));
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					int idx = y * width + x;
					if (nrComponents == 3)
					{
						out.get_pixel(bx + x,by + y).set_r(data[idx * nrComponents + 0]);
						out.get_pixel(bx + x, by + y).set_g(data[idx * nrComponents + 1]);
						out.get_pixel(bx + x, by + y).set_b(data[idx * nrComponents + 2]);
					}
					else if (nrComponents == 4) {	
						out.get_pixel(bx + x, by + y).set_r(data[idx * nrComponents + 0]);
						out.get_pixel(bx + x, by + y).set_g(data[idx * nrComponents + 1]);
						out.get_pixel(bx + x, by + y).set_b(data[idx * nrComponents + 2]);
						out.get_pixel(bx + x, by + y).set_a(data[idx * nrComponents + 3]);

						out.get_pixel(bx + x, by + y) = 0xffff0000;
					}
				}
			}
			stbi_image_free(data);
			back.erase(back.begin() + i);

			int nbx = bx + width + dist;
			int nby = by + height + dist;
			int err = 0;
			
			if (size - nby > 0)
			{
				(err = fill(config, back, out, dist, size, bx, nby, width, size - nby));
			}
			if (size - nbx > 0)
			{
				(err = fill(config, back, out, dist, size, nbx, by, size - nbx, height));
			}
			if (size - nby > 0 && size - nbx > 0)
			{
				(err = fill(config, back, out, dist, size, nbx, nby, size - nbx, size - nby));
			}

			return 0;
		}
	}
	return -2;
}