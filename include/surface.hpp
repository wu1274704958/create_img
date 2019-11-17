#pragma once 

#include <iostream>

namespace wws {

	class cmd_content {
	protected:
		char* ptr = nullptr;
		int w;
		int h;
	public:
		using PIXEL_TYPE = char;
		using PRESENT_ARGS_TYPE = std::ostream&;
		cmd_content(int w, int h) {
			this->w = w + 1;
			this->h = h;
			ptr = new char[this->w * this->h];
		}
		~cmd_content() {
			if (ptr)
			{
				delete[] ptr;
			}
		}
		cmd_content(const cmd_content&) = delete;
		cmd_content(cmd_content&& oth)
		{
			ptr = oth.ptr;
			oth.ptr = nullptr;
		}
		cmd_content& operator=(const cmd_content&) = delete;
		cmd_content& operator=(cmd_content&& oth)
		{
			if (ptr)
			{
				delete[] ptr;
				ptr = nullptr;
			}
			ptr = oth.ptr;
			oth.ptr = nullptr;
			return *this;
		}

		virtual void init()
		{
			for (int i = 1; i < w * h; ++i)
			{
				if (i % w == 0)
					ptr[i - 1] = '\n';
				else 
					ptr[i - 1] = ' ';
			}
			ptr[w * h - 1] = '\0';
		}

		virtual void set_pixel(int x, int y, PIXEL_TYPE p)
		{
			ptr[(y * w) + x] = p;
		}

		virtual void swap(cmd_content& oth)
		{

		}

		virtual void present(PRESENT_ARGS_TYPE a)
		{
			a << ptr << "\n";
		}

	};

	template<typename Cnt>
	class surface{
	protected:
		int width;
		int height;

		Cnt content;

		using PIXEL_TYPE = typename Cnt::PIXEL_TYPE;
		using PRESENT_ARGS_TYPE = typename Cnt::PRESENT_ARGS_TYPE;

	public:
		surface(int w, int h) : 
			content(w,h),
			width(w),
			height(h)
		{
			content.init();
		}

		surface(const surface<Cnt>&) = delete;
		surface(surface<Cnt>&&) = default;

		surface<Cnt>& operator=(const surface<Cnt>&) = delete;
		surface<Cnt>& operator=(surface<Cnt>&&) = default;

		bool good_pos(int x, int y)
		{
			return x >= 0 && x < width && y >= 0 && y < height;
		}

		void set_pixel(int x, int y, PIXEL_TYPE p)
		{
			if(good_pos(x,y))
				content.set_pixel(x,y,p);
		}

		void present(PRESENT_ARGS_TYPE a)
		{
			content.present(a);
		}
	};

	typedef surface<cmd_content> CmdSurface;
}