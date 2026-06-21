List of system & data disks for various CP/M machines

Below is a list of CP/M disk images with their settings and where I got them from.

| Computer          | Type  | CP/M | Bootable | Source           |
| ----------------- | ----- | ---- | -------- | ---------------- | 
| Bondwell 12       | 5.25" | 2.2  |  Yes     | Don Maslin       |
| Amstrad PCW8256   | 3.5"  |      |          |                  |
| Osborne 1         | 5.25" |      |          |                  |
| Osborne Vixen     | 5.25" |      |          |                  |
| Epson QX-10       | 5.25" |      |          |                  |
| Otrona Attaché    | 5.25" | 2.2  |          |                  |
| Kaypro 2          | 5.25" |      |          |                  |
| Sanco 8003        | 5.25" |      |          | Philippe Conseil |
| Bondwell 2        | 3.5"  |      |          |                  |
| Northstar Horizon | 5.25" |      |          |                  | Hard sector
| Amstrad CPC6128   | 3"    |      |          |                  |
| Pertec PCC-2000   | 8"    |      |          |                  |
| XOR Data Science  | 8"    |      |          |                  |
| Vector graphics?
| Research Machines 380Z?
| Heath H89/Zenith Z89
| Heath H90/Zenith Z90

Below is a table of disk parameters

```C
/* Otrona Attaché standard configuration (48 TPI drives) - CP/M 2.2
 * First 3 tracks of side 0 are reserved for CP/M. */
struct cpm_fs_attr otrona_attache = {
	.sector_size          = 512,
	.sector_count         = 10,
	.cylinders            = 40,
	.heads                = 2,
	.block_size           = 2048,
	.boot_cylinders       = 0,
	.skip_first_cylinders = 3,
	.max_dir_entries      = 128,
};

/* Sanco 8003 - CP/M 2.2
 * Side 0 Track 1 is reserved for system: 16 sectors of 256 bytes each. */
// TODO

/* Kaypro II - CP/M 2.2
 * First track of each side reserved for CP/M.
 * A few blocks beyond directory are reserved. (TODO: support that) */
struct cpm_fs_attr kaypro_2 = {
	.sector_size          = 512,
	.sector_count         = 10,
	.cylinders            = 40,
	.heads                = 1,
	.block_size           = 1024,
	.boot_cylinders       = 1,
	.skip_first_cylinders = 0,
	.max_dir_entries      = 64,
};

/* Bondwell 12 - CP/M 2.2
 * 2 tracks reserved for CP/M. */
struct cpm_fs_attr bondwell_12 = {
	.sector_size          = 256,
	.sector_count         = 18,
	.cylinders            = 40,
	.heads                = 1,
	.block_size           = 2048,
	.boot_cylinders       = 2,
	.skip_first_cylinders = 0,
	.max_dir_entries      = 64,
};

```

