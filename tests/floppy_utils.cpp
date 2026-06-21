/* Copyright (c) 2025 Arthur DAUZAT
 * SPDX-License-Identifier: BSD-3-Clause */

#include "floppy_utils.hh"

#include <cstdio>
#include <cstring>
#include <stdexcept>

CpmFloppyImage::CpmFloppyImage(const char *path, const cpm_fs_attr &attrs)
	: attrs_(attrs)
{
	FILE *fp = std::fopen(path, "rb+");
	if (!fp)
		throw std::runtime_error("failed to open image file");

	if (std::fseek(fp, 0, SEEK_END) != 0) {
		std::fclose(fp);
		throw std::runtime_error("failed to seek image file");
	}
	long size = std::ftell(fp);
	std::fseek(fp, 0, SEEK_SET);
	if (size < 0) {
		std::fclose(fp);
		throw std::runtime_error("failed to determine file size");
	}

	data_.resize(static_cast<size_t>(size));
	size_t got = std::fread(data_.data(), 1, data_.size(), fp);
	std::fclose(fp);
	if (got != data_.size())
		throw std::runtime_error("failed to read full image");
}

uint32_t CpmFloppyImage::chs_to_offset(uint32_t cylinder, uint32_t head, uint32_t sector) const
{
	uint32_t cylinder_size = attrs_.sector_count * attrs_.sector_size;
	uint32_t offset = cylinder * cylinder_size * 2;
	offset += (sector - 1) * attrs_.sector_size;
	if (head == 1)
		offset += attrs_.sector_count * attrs_.sector_size;
	return offset;
}

int CpmFloppyImage::get_sector(uint32_t cylinder, uint32_t head, uint32_t sector, uint8_t *out_sector) const
{
	uint32_t offset = chs_to_offset(cylinder, head, sector);
	if (static_cast<size_t>(offset) + attrs_.sector_size > data_.size())
		return -1;
	std::memcpy(out_sector, data_.data() + offset, attrs_.sector_size);
	return 0;
}

int CpmFloppyImage::set_sector(uint32_t cylinder, uint32_t head, uint32_t sector, const uint8_t *in_sector)
{
	uint32_t offset = chs_to_offset(cylinder, head, sector);
	if (static_cast<size_t>(offset) + attrs_.sector_size > data_.size())
		return -1;
	std::memcpy(data_.data() + offset, in_sector, attrs_.sector_size);
	return 0;
}

extern "C" int cpm_get_sector(void *userdata,
			      uint32_t cylinder,
			      uint32_t head,
			      uint32_t sector,
			      uint8_t *out_sector)
{
	auto *image = static_cast<CpmFloppyImage *>(userdata);
	if (!image)
		return -1;
	return image->get_sector(cylinder, head, sector, out_sector);
}

extern "C" int cpm_set_sector(void *userdata,
			      uint32_t cylinder,
			      uint32_t head,
			      uint32_t sector,
			      uint8_t *in_sector)
{
	auto *image = static_cast<CpmFloppyImage *>(userdata);
	if (!image)
		return -1;
	return image->set_sector(cylinder, head, sector, in_sector);
}