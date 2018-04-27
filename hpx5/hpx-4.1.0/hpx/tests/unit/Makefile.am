noinst_HEADERS   = tests.h

noinst_PROGRAMS  = $(TESTS)

AM_CPPFLAGS      = $(HPX_APPS_CPPFLAGS) $(APEX_CFLAGS) -I$(top_srcdir)/include -Wno-unused
AM_CFLAGS        = $(HPX_APPS_CFLAGS) $(APEX_CFLAGS)
AM_CXXFLAGS      = $(HPX_APPS_CXXFLAGS)
AM_LDFLAGS       = $(HPX_APPS_LDFLAGS) -no-install
LDADD            = $(HPX_APPS_LDADD)

LOG_COMPILER = $(TESTS_CMD)

TEST_EXTENSIONS = .cpp .c

TESTS = allreduce               \
        bcast                   \
        call_when               \
        call_vectored           \
        cxx_raii                \
        gas_alloc               \
        gas_alloc_dist          \
        gas_coll                \
        gas_global_alloc        \
        gas_memget              \
        gas_memput              \
        gas_move                \
        gas_set_affinity        \
        init                    \
        lco_get_remote          \
        lco_allreduce           \
        lco_and                 \
        lco_array               \
        lco_collectives         \
        lco_futures             \
        lco_gencount            \
        lco_reduce              \
        lco_sema                \
        lco_setget              \
        lco_user                \
        libhpx_boot             \
        libhpx_cond             \
        parcel_continuation     \
        parcel_create           \
        parcel_send             \
        parcel_send_coalesced   \
        parcel_send_rendezvous  \
        parcel_send_through     \
        process                 \
        runtime                 \
        thread_cont_action      \
        thread_continue         \
        thread_create           \
        thread_gettlsid         \
        thread_sigmask          \
        thread_yield

if ENABLE_LENGTHY_TESTS
TESTS           += lco_wait
endif

if HAVE_APEX
TESTS           += apex
endif

if HAVE_PERCOLATION
TESTS           += percolation
endif

# For some reason I need to explicitly set C++ source files
libhpx_boot_SOURCES                 = libhpx_boot.cpp
cxx_raii_SOURCES                    = cxx_raii.cpp
parcel_send_SOURCES                 = parcel_send.cpp
thread_yield_SOURCES                = thread_yield.cpp

# Override libhpx tests so that they have the right paths and flags, which are
# the LIBHPX versions rather than the HPX_APPS version.
libhpx_boot_CPPFLAGS                = $(LIBHPX_CPPFLAGS) -I$(top_srcdir)/include -Wno-unused
libhpx_boot_CFLAGS                  = $(LIBHPX_CFLAGS)
libhpx_cond_CPPFLAGS                = $(LIBHPX_CPPFLAGS) -I$(top_srcdir)/include -Wno-unused
libhpx_cond_CFLAGS                  = $(LIBHPX_CFLAGS)

apex_DEPENDENCIES                   = $(HPX_APPS_DEPS)
allreduce_DEPENDENCIES              = $(HPX_APPS_DEPS)
bcast_DEPENDENCIES                  = $(HPX_APPS_DEPS)
call_when_DEPENDENCIES              = $(HPX_APPS_DEPS)
call_vectored_DEPENDENCIES          = $(HPX_APPS_DEPS)
cxx_raii_DEPENDENCIES               = $(HPX_APPS_DEPS)
gas_alloc_DEPENDENCIES              = $(HPX_APPS_DEPS)
gas_alloc_dist_DEPENDENCIES         = $(HPX_APPS_DEPS)
gas_coll_DEPENDENCIES               = $(HPX_APPS_DEPS)
gas_global_alloc_DEPENDENCIES       = $(HPX_APPS_DEPS)
gas_memget_DEPENDENCIES             = $(HPX_APPS_DEPS)
gas_memput_DEPENDENCIES             = $(HPX_APPS_DEPS)
gas_move_DEPENDENCIES               = $(HPX_APPS_DEPS)
gas_set_affinity_DEPENDENCIES       = $(HPX_APPS_DEPS)
init_DEPENDENCIES                   = $(HPX_APPS_DEPS)
lco_allreduce_DEPENDENCIES          = $(HPX_APPS_DEPS)
lco_and_DEPENDENCIES                = $(HPX_APPS_DEPS)
lco_array_DEPENDENCIES              = $(HPX_APPS_DEPS)
lco_collectives_DEPENDENCIES        = $(HPX_APPS_DEPS)
lco_futures_DEPENDENCIES            = $(HPX_APPS_DEPS)
lco_gencount_DEPENDENCIES           = $(HPX_APPS_DEPS)
lco_get_remote_DEPENDENCIES         = $(HPX_APPS_DEPS)
lco_reduce_DEPENDENCIES             = $(HPX_APPS_DEPS)
lco_sema_DEPENDENCIES               = $(HPX_APPS_DEPS)
lco_setget_DEPENDENCIES             = $(HPX_APPS_DEPS)
lco_user_DEPENDENCIES               = $(HPX_APPS_DEPS)
lco_wait_DEPENDENCIES               = $(HPX_APPS_DEPS)
libhpx_boot_DEPENDENCIES            = $(HPX_APPS_DEPS)
libhpx_cond_DEPENDENCIES            = $(HPX_APPS_DEPS)
parcel_continuation_DEPENDENCIES    = $(HPX_APPS_DEPS)
parcel_create_DEPENDENCIES          = $(HPX_APPS_DEPS)
parcel_send_DEPENDENCIES            = $(HPX_APPS_DEPS)
parcel_send_coalesced_DEPENDENCIES  = $(HPX_APPS_DEPS)
parcel_send_rendezvous_DEPENDENCIES = $(HPX_APPS_DEPS)
parcel_send_through_DEPENDENCIES    = $(HPX_APPS_DEPS)
percolation_DEPENDENCIES            = $(HPX_APPS_DEPS)
process_DEPENDENCIES                = $(HPX_APPS_DEPS)
runtime_DEPENDENCIES                = $(HPX_APPS_DEPS)
thread_cont_action_DEPENDENCIES     = $(HPX_APPS_DEPS)
thread_continue_DEPENDENCIES        = $(HPX_APPS_DEPS)
thread_create_DEPENDENCIES          = $(HPX_APPS_DEPS)
thread_gettlsid_DEPENDENCIES        = $(HPX_APPS_DEPS)
thread_sigmask_DEPENDENCIES         = $(HPX_APPS_DEPS)
thread_yield_DEPENDENCIES           = $(HPX_APPS_DEPS)
