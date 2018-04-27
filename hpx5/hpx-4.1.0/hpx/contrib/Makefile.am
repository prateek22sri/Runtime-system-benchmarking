SUBDIRS       = lz4
DIST_SUBDIRS  = libffi hwloc photon libcuckoo lz4
EXTRA_DIST    = jemalloc libffi-mic valgrind

if BUILD_LIBFFI
SUBDIRS      += $(LIBFFI_CONTRIB_DIR)
endif

if BUILD_HWLOC
SUBDIRS      += hwloc
endif

if BUILD_PHOTON
SUBDIRS      += photon
endif

if BUILD_JEMALLOC
SUBDIRS      += jemalloc
endif

if BUILD_LIBCUCKOO
SUBDIRS      += libcuckoo
endif

if BUILD_URCU
SUBDIRS      += userspace-rcu
endif

check-recursive: all-recursive
	echo "HPX ignore above warnings, we do not check contrib projects"
