/* Copyright (c) 2025 Arthur DAUZAT
 * SPDX-License-Identifier: BSD-3-Clause */

#include <gtest/gtest.h>
#include <libcpmfs.h>

#include "disk_definitions.hh"
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
class BasicTest : public testing::TestWithParam<DiskSettings *> {
public:
  BasicTest() {
    settings_ = GetParam();
    image_ = new HxCFloppyImage(*settings_);
  }
  ~BasicTest() { delete image_; }

  void SetUp() override { image_->reset_disk(); }

  DiskSettings *settings_;
  HxCFloppyImage *image_;
};

/* List all files */
TEST_P(BasicTest, ListFiles) {
  struct cpm_fs_file *cpmfile;
  struct cpm_fs_dir *dirp;
  struct cpm_fs *fs;

  CHECK_LIBCPMFS(cpm_fs_new(&settings_->attrs_, &cpm_get_sector,
                            &cpm_set_sector, image_->userdata(), &fs));

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
TEST_P(BasicTest, ReadFiles) {
  struct cpm_fs_file_handle *f;
  struct cpm_fs_file *cpmfile;
  struct cpm_fs_dir *dirp;
  struct cpm_fs *fs;
  size_t c_read;

  CHECK_LIBCPMFS(cpm_fs_new(&settings_->attrs_, &cpm_get_sector,
                            &cpm_set_sector, image_->userdata(), &fs));
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

INSTANTIATE_TEST_SUITE_P(
    CpmFloppyGauntlet, BasicTest, testing::Values(&OtronaAttache, &Bondwell12),
    [](const testing::TestParamInfo<DiskSettings *> &info) {
      return info.param->name_;
    });

/* Delete every file then write a very big one then read back and check.
 * File has a size of 131k (256 * 512) */
TEST_P(BasicTest, WriteReadBack) {
  struct cpm_fs_file_handle *f;
  struct cpm_fs_file *cpmfile;
  struct cpm_fs_dir *dirp;
  struct cpm_fs *fs;
  uint8_t buf[512];
  size_t c_read;
  size_t c_written;

  CHECK_LIBCPMFS(cpm_fs_new(&settings_->attrs_, &cpm_get_sector,
                            &cpm_set_sector, image_->userdata(), &fs));

  /* Iterate over files and delete them */
  CHECK_LIBCPMFS(cpm_fs_opendir(fs, &dirp));
  CHECK_LIBCPMFS(cpm_fs_readdir(fs, dirp, &cpmfile));
  while (cpmfile) {
    CHECK_LIBCPMFS(cpm_fs_unlink(fs, cpmfile->d_name, cpmfile->d_user));
    CHECK_LIBCPMFS(cpm_fs_readdir(fs, dirp, &cpmfile));
  }
  CHECK_LIBCPMFS(cpm_fs_closedir(fs, dirp));

  /* Write */
  CHECK_LIBCPMFS(cpm_fs_open(fs, "testfil.tst", CPM_MODE_RDWR, 7, &f));
  for (int i = 0; i < 256; ++i) {
    buf[0] = 'A' + (i % 26);
    CHECK_LIBCPMFS(cpm_fs_write(fs, f, buf, 512, &c_written));
    ASSERT_EQ(c_written, (size_t)512);
  }
  CHECK_LIBCPMFS(cpm_fs_close(fs, f));

  /* Read back and check values */
  CHECK_LIBCPMFS(cpm_fs_open(fs, "testfil.tst", CPM_MODE_RDONLY, 7, &f));
  for (int i = 0; i < 135; ++i) {
    CHECK_LIBCPMFS(cpm_fs_read(fs, f, buf, 512, &c_read));
    ASSERT_EQ(buf[0], 'A' + (i % 26));
  }
  CHECK_LIBCPMFS(cpm_fs_close(fs, f));

  CHECK_LIBCPMFS(cpm_fs_destroy(fs));
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}