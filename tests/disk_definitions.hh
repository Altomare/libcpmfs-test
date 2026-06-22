#pragma once

#include <string>

#include <libcpmfs.h>
#include <libhxcfe.h>

class DiskSettings
{
public:
  DiskSettings(struct cpm_fs_attr attrs, std::string filename, int encoding)
    :attrs_(attrs), filename_(filename), encoding_(encoding) {}

  struct cpm_fs_attr attrs_;
  std::string filename_;
  int encoding_;
};

extern DiskSettings OtronaAttache;
extern DiskSettings Bondwell12;
