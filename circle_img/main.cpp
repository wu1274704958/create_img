#include <iostream>
#include <png.h>
#include <gray.hpp>
#include <matrix2.hpp>
#include <serialization.hpp>

using namespace std;

int main(int argc,char **argv)
{
	//system("pause");

	float r_ratio = 1.0;

	if (argc >= 4)
		r_ratio = wws::parser<float>(argv[3]);

	if (argc >= 3)
	{
		png_image image;

		memset(&image, 0, sizeof(png_image));
		image.version = PNG_IMAGE_VERSION;

		if (png_image_begin_read_from_file(&image, argv[1]) != 0)
		{
			png_bytep buffer;

			image.format = PNG_FORMAT_RGBA;

			size_t byte_size = static_cast<std::size_t>(PNG_IMAGE_SIZE(image));
			size_t size = byte_size / 4;

			buffer = (unsigned char*)malloc(byte_size);

			float w_2 = static_cast<float>( image.width / 2 );
			float h_2 = static_cast<float>( image.height /2 );
			float r = w_2 * r_ratio;

			cout <<  "byte_size : " << byte_size << endl;
			cout << "size : " << size << endl;

			if (buffer != NULL &&
				png_image_finish_read(&image, NULL/*background*/, buffer,
					0/*row_stride*/, NULL/*colormap*/) != 0)
			{
				/* Now write the image out to the second argument.  In the write
				 * call 'convert_to_8bit' allows 16-bit data to be squashed down to
				 * 8 bits; this isn't necessary here because the original read was
				 * to the 8-bit format.
				 */

				for (int y = 0; y < image.height; ++y)
				{
					for (int x = 0; x < image.width; ++x)
					{
						cgm::vec2 l = cgm::vec2({ static_cast<float>(x),static_cast<float>(y) }) - cgm::vec2( { w_2,h_2 } );
						float off = 0.0f;
						if ((off = (l.len() - r)) >= 0.f)
						{
							int curr = y * image.width + x;
							int* c = reinterpret_cast<int*>(&buffer[curr * 4]);
							
							if (off > 5.0f)
							{
								*c = 0x00ffffff;
							}
							else {
								unsigned int alpha = static_cast<unsigned int>( (1.0f - ( off / 5.0f)) * 255);
								*c &= 0x00ffffff;
								*c |= (alpha << 24);
							}
						}
					}
				}


				if (png_image_write_to_file(&image, argv[2], 0/*convert_to_8bit*/,
					buffer, 0/*row_stride*/, NULL/*colormap*/) != 0)
				{
					/* The image has been written successfully. */
					exit(0);
				}
				if (buffer)
					free(buffer);
			}
			else
			{
				/* Calling png_image_free is optional unless the simplified API was
				 * not run to completion.  In this case, if there wasn't enough
				 * memory for 'buffer', we didn't complete the read, so we must
				 * free the image:
				 */
				if (buffer == NULL)
					png_image_free(&image);
				else
					free(buffer);
			}
		}
		std::cerr << "begin read failed " << image.message << endl;
		return 0;
	}
	std::cerr << "Not input file!" << endl;
	return -1;
}