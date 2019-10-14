#pragma once 
#include <iostream>
#include <memory>
#include <tuple>
#include <optional>
#include <png.h>

#define PNG_FAILED_CODE 0
namespace png_sundry {
	std::optional<std::tuple<std::unique_ptr<png_byte[]>, std::size_t>> load_png(const char* file, uint32_t format, png_image* image)
	{
		if (!file || !image) return std::nullopt;
		if (png_image_begin_read_from_file(image, file) == PNG_FAILED_CODE)//error
		{
			return std::nullopt;
		}

		std::size_t byte_size;
		image->format = format;
		byte_size = static_cast<std::size_t>(PNG_IMAGE_SIZE(*image));
		std::unique_ptr<png_byte[]> buffer{ new png_byte[byte_size] };

		if (!buffer)
		{
			png_image_free(image);
			return std::nullopt;
		}

		if (png_image_finish_read(image, nullptr, buffer.get(), 0, nullptr) == PNG_FAILED_CODE)
		{
			return std::nullopt;
		}

		return std::make_optional(std::make_tuple(std::move(buffer), byte_size));
	}

	void init_image(png_imagep pimg, uint32_t ver = PNG_IMAGE_VERSION)
	{
		memset(pimg, 0, sizeof(std::remove_pointer_t<png_imagep>));
		pimg->version = ver;
	}

}