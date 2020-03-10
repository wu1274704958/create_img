#pragma once
#include "ft2pp.hpp"
#include <memory>
#include <vector>
#include <matrix2.hpp>
#include <comm.hpp>
#include <functional>

namespace wws{

    

    struct point {
	    cgm::vec2 pos;
	    cgm::vec2 v;
	    cgm::vec2 tar;
	    point() : pos({ -1.f,-1.f }) {
            
	    }
    };

	template<typename Cnt>
    struct ASDrive
    {
        virtual bool is_end() = 0;
        virtual void set_text(surface<Cnt>& sur,typename Cnt::PIXEL_TYPE pt) = 0;
        virtual void step() = 0;
        virtual ~ASDrive(){}
    };

	template<typename Cnt>
    struct AniSurface
    {
        AniSurface(int w,int h,ASDrive<Cnt>* drive,typename Cnt::PIXEL_TYPE pt,int reserve = 100) : 
            sur( w, h),
            last(w, h),
            back(w, h),
            drive(drive),
			fill_byte(pt)
        {
            use.reserve(reserve);
	        out.reserve(reserve);
        }
        std::unique_ptr<point>& get_out_to_use()
	    {
	    	if (out.empty())
	    	{
	    		use.push_back(std::unique_ptr<point>(new point()));
	    		return use.back();
	    	}
	    	use.push_back(std::move(out.back()));
	    	out.pop_back();
	    	return use.back();
	    }

	    std::unique_ptr<point>& get_use_to_out (int x,int y)
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
	    }

	    cgm::vec2 rd_out_pos (int x,int y){
		    if (x < out_MaxW || (sur.w() - x) < out_MaxW )
		    {
		    	int res_x = x < out_MaxW ? -1 : sur.w();
		    	int off = rand() % out_M;
		    	if (y + off < sur.h())
		    		return cgm::vec2{ static_cast<float>(res_x) ,static_cast<float>(y + off) };
		    		else 
		    		return cgm::vec2{ static_cast<float>(res_x) ,static_cast<float>(y - off) };
		    }
		    else {
		    	int res_y = y > (sur.h() / 2) ? sur.h() : -1;//(rand() % 2) == 0 ? -1 : sur.h();
		    	int off = rand() % out_M;

		    	if (x + off < sur.w())
		    		return cgm::vec2{ static_cast<float>(x + off), static_cast<float>(res_y)  };
		    	else
		    		return cgm::vec2{ static_cast<float>(x - off), static_cast<float>(res_y)  };
	        }
	    }

	    void step_unit(std::unique_ptr<point>& p) 
        {
		    if (p->pos.x() != p->tar.x() || p->pos.y() != p->tar.y())
		    {
		    	if (std::abs(p->pos.x() - p->tar.x()) < 1.0 && std::abs(p->pos.y() - p->tar.y()) < 1.0)
		    	{
		    		p->pos = p->tar;
		    	}
		    	else
		    	{
		    		move_to(p->pos,p->v,p->tar);
		    	}
		    }
		    else {
		    	p->v.x() = 0.0f;
		    	p->v.y() = 0.0f;
		    }
	    }

		virtual void move_to(cgm::vec2& pos,cgm::vec2 v,cgm::vec2 tar)
		{
			if(move_to_func)
				move_to_func(pos,v,tar);
			else
				pos = pos + v;
		}

	void step() {
		for (auto& p : use) {
			step_unit(p);
		}
		for (auto& p : out) {
			step_unit(p);
		}
	};

	void fill() {
		for (auto& p : use) {
			sur.set_pixel(static_cast<int>(std::roundf(p->pos.x())), static_cast<int>(std::roundf(p->pos.y())),fill_byte);
		}
		for (auto& p : out) {
			sur.set_pixel(static_cast<int>(std::roundf(p->pos.x())), static_cast<int>(std::roundf(p->pos.y())),fill_byte);
		}
	};

    bool good_out_MaxW(int v)
    {
        return v >= 0;
    }  
    int get_out_MaxW()
    {
        return out_MaxW;
    }    
    void set_out_MaxW(int v)
    {
        if(good_out_MaxW(v))
            out_MaxW = v;   
    }

    bool good_out_M(int v)
    {
        return v >= 0;
    }  
    int get_out_M()
    {
        return out_M;
    }    
    void set_out_M(int v)
    {
        if(good_out_M(v))
            out_M = v;   
    }

    bool good_transfer_sec(int v)
    {
        return v > 0;
    }  
    int get_transfer_sec()
    {
        return transfer_sec;
    }    
    void set_transfer_sec(int v)
    {
        if(good_transfer_sec(v))
            transfer_sec = v;   
    }

    bool good_min_frame_ms(int v)
    {
        return v > 0;
    }  
    int get_min_frame_ms()
    {
        return min_frame_ms;
    }    
    void set_min_frame_ms(int v)
    {
        if(good_min_frame_ms(v))
            min_frame_ms = v;   
    }

    void go()
    {
        drive->set_text(back,fill_byte);

	    now = std::chrono::system_clock::now();
	    start = std::chrono::system_clock::now();
    
	    alread_set = true;

	    while (!drive->is_end())
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
	    					p->tar = cgm::vec2{ static_cast<float>(x),static_cast<float>(y) };
	    					p->v = (p->tar - p->pos).unitized() * (static_cast<float>((rand() % 10) + 5) * 0.06f);
	    				}
	    				else
	    				if (back.get_pixel(x, y) == ' ' && last.get_pixel(x, y) != ' ')
	    				{
	    					auto& p = get_use_to_out(x, y);
	    					p->pos = cgm::vec2{ static_cast<float>(x),static_cast<float>(y) };
	    					p->tar = rd_out_pos(x, y);
	    					p->v = (p->tar - p->pos).unitized() * (static_cast<float>((rand() % 10) + 5) * 0.05f);
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
	    	if (duration < min_frame_ms)
	    	{
	    		std::this_thread::sleep_for(std::chrono::milliseconds(min_frame_ms - duration));
	    	}

	    	auto end = std::chrono::system_clock::now();
	    	if (std::chrono::duration_cast<std::chrono::seconds>(end - now).count() >= transfer_sec)
	    	{
	    		drive->step();
	    		back.swap(last);
	    		back.clear();
				if(!drive->is_end())
	    			drive->set_text(back,fill_byte);
	    		alread_set = true;
	    		now = std::chrono::system_clock::now();
	    	}
	    }
    }

    typename Cnt::PIXEL_TYPE fill_byte;
	std::function<void(cgm::vec2&,cgm::vec2,cgm::vec2)> move_to_func;

    protected:
        int out_MaxW = 16;
        int out_M = 10;

        std::vector<std::unique_ptr<point>> use;
	    std::vector<std::unique_ptr<point>> out;

        surface<Cnt> sur;
	    surface<Cnt> last;
	    surface<Cnt> back;

        std::chrono::system_clock::time_point now;
	    std::chrono::system_clock::time_point start;

        int transfer_sec = 3;
        int min_frame_ms = 18;
	
	    bool alread_set;

        ASDrive<Cnt>* drive = nullptr;
    };

}