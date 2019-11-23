#include <iostream>
#include <png.h>
#include <tuple>
#include <memory>
#include <sundry.hpp>
#include <functional>
#include <comm.hpp>
#include <serialization.hpp>
#include <matrix2.hpp>

using namespace std;
namespace ps = ::png_sundry;
using namespace ps;

#define CONST_ARGS png_imagep, png_imagep, unique_ptr<png_byte[]>&, unique_ptr<png_byte[]>&

void f1(std::tuple<dyn_op,ps::hex, ps::hex>&, CONST_ARGS);
void f2(std::tuple<dyn_op, ps::hex, dyn_op, ps::hex, unsigned int, ps::hex>&, CONST_ARGS);

auto handlers = make_tuple(
	FuncWithArgs<void,std::tuple<dyn_op,ps::hex, ps::hex>,CONST_ARGS>(f1),
	FuncWithArgs<void,std::tuple<dyn_op, ps::hex, dyn_op, ps::hex, unsigned int, ps::hex>, CONST_ARGS>(f2)
);


int main(int argc, char** argv)
{
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
			if (fi < tuple_size_v<decltype(handlers)> && fi >= 0)
			{
				try {
					fwa_tup_run(handlers, fi, argv, 5,argc - 5, &cxt,&clip, cxt_ptr, clip_ptr);
				}
				catch (std::exception e)
				{
					cerr << "error: "<< e.what() << endl;
					return -1;
				}
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

void f1(std::tuple<dyn_op,ps::hex,ps::hex>& args,png_imagep cxt,png_imagep clip,unique_ptr<png_byte[]>& cxt_ptr, unique_ptr<png_byte[]>& clip_ptr)
{
	auto [op,color,dst_color] = args;
	for (int y = 0; y < cxt->height; ++y)
	{
		for (int x = 0; x < cxt->width; ++x)
		{
			int curr = y * cxt->width + x;
			int* cc = reinterpret_cast<int*>(&clip_ptr[curr * 4]);
			if (op.cmp( *cc , color))
			{
				int* cxt_c = reinterpret_cast<int*>(&cxt_ptr[curr * 4]);
				*cxt_c = dst_color;
			}
		}
	}
}

#define F2_MAX_CAN_YING 32
#define F2_LIGHT_POS {0.0f,0.0f}

void f2(std::tuple<dyn_op, ps::hex, dyn_op, ps::hex,unsigned int,ps::hex>& args,png_imagep cxt, png_imagep clip, unique_ptr<png_byte[]>& cxt_ptr, unique_ptr<png_byte[]>& clip_ptr)
{
	using namespace cgm;

	auto [op1, c1, op2, c2,max_sp,sp_c] = args;

	vec2 light_pos(F2_LIGHT_POS);
	light_pos.x() *= static_cast<float>(cxt->width);
	light_pos.y() *= static_cast<float>(cxt->height);

	for (int y = 0; y < cxt->height; ++y)
	{
		for (int x = 0; x < cxt->width; ++x)
		{
			int curr = y * cxt->width + x;
			int* cc = reinterpret_cast<int*>(&clip_ptr[curr * 4]);
			if (op1.cmp( *cc , c1))
			{
				vec2 lv = vec2({static_cast<float>(x),static_cast<float>(y)}) - light_pos;
				vec2 lv_unit = lv.unitized();
				int color = sp_c;
				for (int i = 1; i < max_sp; ++i)
				{
					vec2 last = lv - (lv_unit * static_cast<float>(i)) + light_pos;
					if (last.x() >= 0 && last.y() >= 0)
					{
						int last_curr = static_cast<int>(last.y())* cxt->width + static_cast<int>(last.x());
						int* last_cc = reinterpret_cast<int*>(&clip_ptr[last_curr * 4]);
						if (op2.cmp( *last_cc , c2))
						{
							int* last_cxt_c = reinterpret_cast<int*>(&cxt_ptr[last_curr * 4]);
							color = /**last_cxt_c*/ 0x0 & color;
							color |= (static_cast<int>((1.0 - static_cast<float>(i) / static_cast<float>(max_sp)) * 255.f) << 24);
							break;
						}
					}
				}
				int* cxt_c = reinterpret_cast<int*>(&cxt_ptr[curr * 4]);
				*cxt_c = color;
			}
		}
	}
}