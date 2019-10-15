#include <iostream>
#include <png.h>
#include <tuple>
#include <memory>
#include <include/sundry.hpp>
#include <functional>
#include <comm.hpp>
#include <serialization.hpp>

using namespace std;
namespace ps = ::png_sundry;
using namespace ps;

typedef std::function<void(png_imagep, png_imagep, unique_ptr<png_byte[]>&, unique_ptr<png_byte[]>&)> HandlerFT;

void f1(png_imagep cxt, png_imagep clip, unique_ptr<png_byte[]>& cxt_ptr, unique_ptr<png_byte[]>& clip_ptr);

int main(int argc, char** argv)
{

	HandlerFT handlers[] = { f1 };

	if (argc >= 5)
	{
		int fi = 0;
		try {
			fi = wws::parser<int>(argv[4]);
		}
		catch (...)
		{
			cerr << "parser failed!" << endl;
			return -1;
		}

		png_image cxt, clip;

		init_image(&cxt);
		init_image(&clip);

		auto cxt_buf = load_png(argv[1], PNG_FORMAT_RGBA, &cxt);
		auto clip_buf = load_png(argv[2], PNG_FORMAT_BGRA, &clip);

		if (cxt_buf && clip_buf)
		{
			auto [cxt_ptr,cxt_size] = std::move(cxt_buf.value());
			auto [clip_ptr, clip_size] = std::move(clip_buf.value());

			if (cxt.width != clip.width || cxt.height != clip.height)
			{
				std::cerr << "Size not same!" << endl;
				return -1;
			}
			if (fi < wws::arrLen(handlers) && fi >= 0)
			{
				handlers[fi](&cxt, &clip, cxt_ptr, clip_ptr);
			}else{
				cerr << "not find this handler!" << endl;
				return -1;
			}

			png_image_write_to_file(&cxt, argv[3], 0/*convert_to_8bit*/,
				cxt_ptr.get(), 0/*row_stride*/, NULL/*colormap*/);
		}
		else
		{
			std::cerr << "load error : " << endl;
			return -1;
		}
	}
	else {
		std::cerr << "Not input file!" << endl;
		return -1;
	}

	return 0;
}

void f1(png_imagep cxt,png_imagep clip,unique_ptr<png_byte[]>& cxt_ptr, unique_ptr<png_byte[]>& clip_ptr)
{
	for (int y = 0; y < cxt->height; ++y)
	{
		for (int x = 0; x < cxt->width; ++x)
		{
			int curr = y * cxt->width + x;
			int* cc = reinterpret_cast<int*>(&clip_ptr[curr * 4]);
			if (*cc == 0xffffffff)
			{
				int* cxt_c = reinterpret_cast<int*>(&cxt_ptr[curr * 4]);
				*cxt_c = 0x00ffffff;
			}
		}
	}
}