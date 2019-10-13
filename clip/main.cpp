#include <iostream>
#include <png.h>
#include <tuple>
#include <memory>
#include <include/sundry.hpp>

using namespace std;


int main(int argc, char** argv)
{
	if (argc >= 4)
	{
		png_image cxt, clip;

		memset(&cxt, 0, sizeof(png_image));
		memset(&clip, 0, sizeof(png_image));

		cxt.version = clip.version = PNG_IMAGE_VERSION;


	}
	else {
		std::cerr << "Not input file!" << endl;
		return -1;
	}

	return 0;
}