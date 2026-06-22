/* Copyright (c) 2025 Arthur DAUZAT
 * SPDX-License-Identifier: BSD-3-Clause */

#pragma once

#include <cstdint>
#include <vector>

#include <libcpmfs.h>

#include "disk_definitions.hh"


class HxCFloppyImage {
public:
	HxCFloppyImage(DiskSettings &settings);
	~HxCFloppyImage();
	void *userdata() { return this; }
	int read_sector(uint32_t cylinder, uint32_t head, uint32_t sector, uint8_t *out_sector) const;
	int write_sector(uint32_t cylinder, uint32_t head, uint32_t sector, const uint8_t *in_sector);
private:
	DiskSettings &settings_;
	HXCFE *hxcfe_;
	HXCFE_FLOPPY *floppy_;
	HXCFE_IMGLDR *loader_;
	HXCFE_SECTORACCESS *sector_access_;
};


class CpmFloppyImage {
public:
	CpmFloppyImage(const char *path, const cpm_fs_attr &attrs);

	void *userdata() { return this; }

	int get_sector(uint32_t cylinder, uint32_t head, uint32_t sector, uint8_t *out_sector) const;
	int set_sector(uint32_t cylinder, uint32_t head, uint32_t sector, const uint8_t *in_sector);

private:
	uint32_t chs_to_offset(uint32_t cylinder, uint32_t head, uint32_t sector) const;

	cpm_fs_attr attrs_;
	std::vector<uint8_t> data_;
};

extern "C" {

/* C callbacks for libcpmfs */
int cpm_get_sector(void *userdata,
		    uint32_t cylinder,
		    uint32_t head,
		    uint32_t sector,
		    uint8_t *out_sector);

int cpm_set_sector(void *userdata,
		    uint32_t cylinder,
		    uint32_t head,
		    uint32_t sector,
		    uint8_t *in_sector);

} /* extern "C" */