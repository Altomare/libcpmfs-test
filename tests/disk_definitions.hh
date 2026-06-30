#pragma once

#include <string>

#include <libcpmfs.h>
#include <libhxcfe.h>

class DiskSettings {
public:
  DiskSettings(const std::string &name, struct cpm_fs_attr attrs,
               std::string filename, int encoding, int first_sector)
      : name_(name), attrs_(attrs), filename_(filename), encoding_(encoding),
        first_sector_(first_sector) {}

  std::string name_;
  struct cpm_fs_attr attrs_;
  std::string filename_;
  int encoding_;
  /* TODO: Something better than this */
  int first_sector_;
};

extern DiskSettings OtronaAttache;
extern DiskSettings Bondwell12;
