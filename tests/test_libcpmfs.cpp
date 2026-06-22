/* Copyright (c) 2025 Arthur DAUZAT
 * SPDX-License-Identifier: BSD-3-Clause */

#include <gtest/gtest.h>
#include <libcpmfs.h>

#include "floppy_utils.hh"

struct cpm_fs_attr otrona_attrs = {
    .cylinders = 40,
    .heads = 2,
    .sector_count = 10,
    .sector_size = 512,
    .block_size = 2048,
    .max_dir_entries = 128,
    .boot_cylinders = 0,
    .skip_first_cylinders = 3,
};

#define CHECK_LIBCPMFS(call)                                                   \
  {                                                                            \
    enum cpm_fs_status status = call;                                          \
    ASSERT_EQ(status, CPM_SUCCESS) << cpm_fs_status_str(status);               \
  }

namespace {
class BasicTest : public testing::Test {
protected:
  BasicTest()
      : attrs_(&otrona_attrs),
        image_(CpmFloppyImage("disks/otrona.img", otrona_attrs)) {}

  struct cpm_fs_attr *attrs_;
  CpmFloppyImage image_;
};

/* List all files */
TEST_F(BasicTest, ListFiles) {
  struct cpm_fs_file *cpmfile;
  struct cpm_fs_dir *dirp;
  struct cpm_fs *fs;

  CHECK_LIBCPMFS(cpm_fs_new(&otrona_attrs, &cpm_get_sector, &cpm_set_sector,
                            image_.userdata(), &fs));

  CHECK_LIBCPMFS(cpm_fs_opendir(fs, &dirp));

  CHECK_LIBCPMFS(cpm_fs_readdir(fs, dirp, &cpmfile));
  while (cpmfile) {
    std::cout << cpmfile->d_name << "\t";
    CHECK_LIBCPMFS(cpm_fs_readdir(fs, dirp, &cpmfile));
  }
  std::cout << std::endl;

  CHECK_LIBCPMFS(cpm_fs_closedir(fs, dirp));

  CHECK_LIBCPMFS(cpm_fs_destroy(fs));
}

/* Read every file and check if the announced size matches what we read */
TEST_F(BasicTest, ReadFiles) {
  struct cpm_fs_file_handle *f;
  struct cpm_fs_file *cpmfile;
  struct cpm_fs_dir *dirp;
  struct cpm_fs *fs;
  size_t c_read;

  CHECK_LIBCPMFS(cpm_fs_new(&otrona_attrs, &cpm_get_sector, &cpm_set_sector,
                            image_.userdata(), &fs));
  CHECK_LIBCPMFS(cpm_fs_opendir(fs, &dirp));

  CHECK_LIBCPMFS(cpm_fs_readdir(fs, dirp, &cpmfile));
  while (cpmfile) {
    if (cpmfile->d_size == 0) {
      CHECK_LIBCPMFS(cpm_fs_readdir(fs, dirp, &cpmfile));
      continue;
    }

    uint8_t *buf = (uint8_t *)calloc(cpmfile->d_size, 1);
    ASSERT_NE(buf, (uint8_t *)NULL);

    CHECK_LIBCPMFS(
        cpm_fs_open(fs, cpmfile->d_name, CPM_MODE_RDONLY, cpmfile->d_user, &f));
    CHECK_LIBCPMFS(cpm_fs_read(fs, f, buf, cpmfile->d_size, &c_read));
    CHECK_LIBCPMFS(cpm_fs_close(fs, f));

    ASSERT_EQ(c_read, cpmfile->d_size)
        << cpmfile->d_name << ": number of bytes read differ from filesize";

    free(buf);

    CHECK_LIBCPMFS(cpm_fs_readdir(fs, dirp, &cpmfile));
  }

  CHECK_LIBCPMFS(cpm_fs_closedir(fs, dirp));
  CHECK_LIBCPMFS(cpm_fs_destroy(fs));
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}