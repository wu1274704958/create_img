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

struct ImageData
{
	unsigned char* data = nullptr;
	int width = 0, height = 0, nrComponents = 0;

	ImageData()
	{}

	ImageData(unsigned char* data, int width, int height, int nrComponents) : data(data),
		width(width),
		height(height),
		nrComponents(nrComponents)
	{}

	ImageData(std::string path)
	{
		data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	}

	ImageData(const ImageData&) = delete;
	ImageData& operator=(const ImageData&) = delete;

	ImageData(ImageData&& oth)
	{
		data = oth.data;
		width = oth.width;
		height = oth.height;
		nrComponents = oth.nrComponents;

		oth.data = nullptr;
		oth.clear();
	}
	ImageData& operator=(ImageData&& oth)
	{
		if (data)
			stbi_image_free(data);

		data = oth.data;
		width = oth.width;
		height = oth.height;
		nrComponents = oth.nrComponents;

		oth.data = nullptr;
		oth.clear();

		return *this;
	}
	void clear()
	{
		width = height = nrComponents = 0;
	}
	bool good()
	{
		return data != nullptr;
	}

	~ImageData()
	{
		if (data)
			stbi_image_free(data);
		clear();
	}
};


int fill(wws::Json& config,std::vector<fs::path>& back,wws::rgba_content& out,std::unordered_map<std::string, ImageData>& map, int dist, int x,int y, int w, int h);



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
	std::unordered_map<std::string, ImageData> map;

	wws::enum_path(root, [&back](const fs::path& f) {
		if (f.extension() == ".png" || f.extension() == ".jpg")
		{
			back.push_back(f);
		}
	});
	

	wws::rgba_content out(size, size);
	out.init();
	getchar();
	int err = fill(config,back, out,map,dist,0,0,size,size);

	dbg(back.size());

	stbi_write_png("out.png", out.width(), out.height(), 4, out.get(), 0 );

	return 0;
}

void copy_to(ImageData* img,wws::rgba_content& out,int bx,int by)
{
	for (int y = 0; y < img->height; ++y)
	{
		for (int x = 0; x < img->width; ++x)
		{
			int idx = y * img->width + x;
			if (img->nrComponents == 3)
			{
				out.get_pixel(bx + x, by + y).set_r(img->data[idx * img->nrComponents + 0]);
				out.get_pixel(bx + x, by + y).set_g(img->data[idx * img->nrComponents + 1]);
				out.get_pixel(bx + x, by + y).set_b(img->data[idx * img->nrComponents + 2]);
			}
			else if (img->nrComponents == 4) {
				out.get_pixel(bx + x, by + y).set_r(img->data[idx * img->nrComponents + 0]);
				out.get_pixel(bx + x, by + y).set_g(img->data[idx * img->nrComponents + 1]);
				out.get_pixel(bx + x, by + y).set_b(img->data[idx * img->nrComponents + 2]);
				out.get_pixel(bx + x, by + y).set_a(img->data[idx * img->nrComponents + 3]);
			}
		}
	}
}

int fill(wws::Json& config, std::vector<fs::path>& back, wws::rgba_content& out,std::unordered_map<std::string, ImageData>& map,int dist, int bx, int by, int w, int h)
{
	if (back.empty()) return -1;
	dbg(std::make_tuple(bx, by,w,h));
	ImageData* img_ = nullptr;
	int idx = -1;
	int max_size = 0;

	for (int i = 0; i < back.size(); ++i)
	{
		auto& f = back[i];

		auto f_str = f.generic_string();

		ImageData* img;
		if (map.find(f_str) != map.end())
		{
			img = &(map[f_str]);
		}
		else {
			map[f_str] = ImageData(f_str);
			img = &(map[f_str]);
		}

		if (img->good() && w >= img->width && h >= img->height)
		{
			if (img->width * img->height > max_size)
			{
				img_ = img;
				max_size = img->width * img->height;
				idx = i;
			}
		}
	}

	if (img_)
	{

		copy_to(img_, out, bx, by);

		back.erase(back.begin() + idx);

		int nbx = bx + img_->width + dist;
		int nby = by + img_->height + dist;
		int err = 0;
		int nh = by + h;
		int nw = bx + w;

		if (nh - nby > 0)
		{
			(err = fill(config, back, out, map, dist, bx, nby, img_->width, nh - nby));
		}
		if (nw - nbx > 0)
		{
			(err = fill(config, back, out, map, dist, nbx, by, nw - nbx, img_->height));
		}
		if (nh - nby > 0 && nw - nbx > 0)
		{
			(err = fill(config, back, out, map, dist, nbx, nby, nw - nbx, nh - nby));
		}

		return 0;
	}
	return -2;
}