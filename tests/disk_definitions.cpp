
#include <string>

#include "disk_definitions.hh"

/* Otrona Attaché, CP/M 2.2, default configuration with 48 TPI drives.
 * First 3 tracks of side 0 are reserved for CP/M. */
DiskSettings OtronaAttache(
  "Otrona_Attache",
  {
    .cylinders = 40,
    .heads = 2,
    .sector_count = 10,
    .sector_size = 512,
    .block_size = 2048,
    .max_dir_entries = 128,
    .boot_cylinders = 0,
    .skip_first_cylinders = 3,
  },
  "disks/otrona.td0",
  ISOIBM_MFM_ENCODING,
  1);

/* Bondwell 12 - CP/M 2.2
 * 2 tracks reserved for CP/M. */
DiskSettings Bondwell12(
  "Bondwell_12",
  {
    .cylinders = 40,
    .heads = 1,
    .sector_count = 18,
    .sector_size = 256,
    .block_size = 2048,
    .max_dir_entries = 64,
    .boot_cylinders = 2,
    .skip_first_cylinders = 0,
  },
  "disks/bondwl12.td0",
  ISOIBM_MFM_ENCODING,
  0);
