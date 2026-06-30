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
  int read_sector(uint32_t cylinder, uint32_t head, uint32_t sector,
                  uint8_t *out_sector) const;
  int write_sector(uint32_t cylinder, uint32_t head, uint32_t sector,
                   const uint8_t *in_sector);

  void reset_disk(void);

private:
  DiskSettings &settings_;
  HXCFE *hxcfe_;

  HXCFE_FLOPPY *floppy_ref_;
  HXCFE_FLOPPY *floppy_;

  HXCFE_IMGLDR *loader_;
  HXCFE_SECTORACCESS *sector_access_;
};

/* C callbacks for libcpmfs */
extern "C" {
int cpm_get_sector(void *userdata, uint32_t cylinder, uint32_t head,
                   uint32_t sector, uint8_t *out_sector);
int cpm_set_sector(void *userdata, uint32_t cylinder, uint32_t head,
                   uint32_t sector, uint8_t *in_sector);
} /* extern "C" */
