# Don't spam dir logs
MAKEFLAGS += --no-print-directory

CXX := g++
CXXFLAGS := \
	-Wall -Wextra \
	-Igoogletest/googletest/include \
	-IHXC/include \
	-Ilibcpmfs/include

TEST_SRC := \
	tests/test_libcpmfs.cpp \
	tests/disk_definitions.cpp \
	tests/floppy_utils.cpp
TEST_BIN := run_tests

# Deps
GTEST_BUILD := googletest/build
GTEST_LIBS  := $(GTEST_BUILD)/lib/libgtest.a

HXC_LIB := hxc/build/libhxcfe.so

LIBCPMFS_LIB := libcpmfs/build/libcpmfs.a

.PHONY: all clean hxc gtest libcpmfs

all: $(TEST_BIN)

libcpmfs:
	@echo "Build libcpmfs"
	@$(MAKE) -C libcpmfs/ libcpmfs

hxc:
	@echo "Build libhxcfe"
	@$(MAKE) -C hxc/build/ libhxcfe.so

gtest:
	@echo "Build gtest"
	@mkdir -p $(GTEST_BUILD)
	@cd googletest/build && cmake .. && $(MAKE) gtest

$(TEST_BIN): hxc gtest libcpmfs $(TEST_SRC)
	$(CXX) $(CXXFLAGS) \
	    -I hxc/libhxcfe/sources/ \
	    $(TEST_SRC) \
	    $(HXC_LIB) \
	    $(GTEST_LIBS) \
	    $(LIBCPMFS_LIB) \
	    -o $@
# 	    -pthread

clean:
	rm -f $(TEST_BIN)
	$(MAKE) -C hxc/build/ clean
	$(MAKE) -C libcpmfs/ clean
	rm -rf googletest/build
