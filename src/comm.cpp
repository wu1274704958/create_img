#include <comm.hpp>

#ifdef _MSC_VER

#include <Windows.h>
#undef max
#undef min

#else

#endif  


namespace wws{
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
}