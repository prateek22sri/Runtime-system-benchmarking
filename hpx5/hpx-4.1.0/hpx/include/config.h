/* include/config.h.  Generated from config.h.in by configure.  */
/* include/config.h.in.  Generated from configure.ac by autoheader.  */

/* We have hwloc support */
#define BUILD_HWLOC 1

/* Enable debugging stuff */
/* #undef ENABLE_DEBUG */

/* Enable instrumentationging stuff */
/* #undef ENABLE_INSTRUMENTATION */

/* Enable loggging stuff */
/* #undef ENABLE_LOGGING */

/* We have agas */
/* #undef HAVE_AGAS */

/* We have a GAS allocator available */
#define HAVE_ALLOCATOR 1

/* We have apex support */
/* #undef HAVE_APEX */

/* We have global memory */
#define HAVE_AS_GLOBAL 1

/* We have registered memory */
/* #undef HAVE_AS_REGISTERED */

/* Define to 1 if the system has the type `CACHE_DESCRIPTOR'. */
/* #undef HAVE_CACHE_DESCRIPTOR */

/* Define to 1 if the system has the type `CACHE_RELATIONSHIP'. */
/* #undef HAVE_CACHE_RELATIONSHIP */

/* Define if the assembler supports CFI directives. */
#define HAVE_CFI_DIRECTIVES 1

/* Define to 1 if you have the `clz' function. */
/* #undef HAVE_CLZ */

/* Define to 1 if you have the `clzl' function. */
/* #undef HAVE_CLZL */

/* Define to 1 if you have the <CL/cl_ext.h> header file. */
/* #undef HAVE_CL_CL_EXT_H */

/* Have 16-byte compare and swap */
#define HAVE_CMPXCHG16B 1

/* Define to 1 if you have the `cpuset_setaffinity' function. */
/* #undef HAVE_CPUSET_SETAFFINITY */

/* Define to 1 if you have the `cpuset_setid' function. */
/* #undef HAVE_CPUSET_SETID */

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if we have -lcuda */
/* #undef HAVE_CUDA */

/* Define to 1 if you have the <cuda.h> header file. */
/* #undef HAVE_CUDA_H */

/* Define to 1 if you have the <cuda_runtime_api.h> header file. */
/* #undef HAVE_CUDA_RUNTIME_API_H */

/* define if the compiler supports basic C++11 syntax */
/* #undef HAVE_CXX11 */

/* Define to 1 if you have the declaration of `CL_DEVICE_TOPOLOGY_AMD', and to
   0 if you don't. */
/* #undef HAVE_DECL_CL_DEVICE_TOPOLOGY_AMD */

/* Define to 1 if you have the declaration of `CTL_HW', and to 0 if you don't.
   */
#define HAVE_DECL_CTL_HW 0

/* Define to 1 if you have the declaration of `fabsf', and to 0 if you don't.
   */
#define HAVE_DECL_FABSF 1

/* Define to 1 if you have the declaration of `getexecname', and to 0 if you
   don't. */
#define HAVE_DECL_GETEXECNAME 0

/* Define to 1 if you have the declaration of `GetModuleFileName', and to 0 if
   you don't. */
#define HAVE_DECL_GETMODULEFILENAME 0

/* Define to 1 if you have the declaration of `getprogname', and to 0 if you
   don't. */
#define HAVE_DECL_GETPROGNAME 0

/* Define to 1 if you have the declaration of `HW_NCPU', and to 0 if you
   don't. */
#define HAVE_DECL_HW_NCPU 0

/* Define to 1 if you have the declaration of
   `nvmlDeviceGetMaxPcieLinkGeneration', and to 0 if you don't. */
/* #undef HAVE_DECL_NVMLDEVICEGETMAXPCIELINKGENERATION */

/* Define to 1 if you have the declaration of `pthread_getaffinity_np', and to
   0 if you don't. */
#define HAVE_DECL_PTHREAD_GETAFFINITY_NP 1

/* Define to 1 if you have the declaration of `pthread_setaffinity_np', and to
   0 if you don't. */
#define HAVE_DECL_PTHREAD_SETAFFINITY_NP 1

/* Embedded mode; just assume we do not have Valgrind support */
#define HAVE_DECL_RUNNING_ON_VALGRIND 0

/* Define to 1 if you have the declaration of `snprintf', and to 0 if you
   don't. */
#define HAVE_DECL_SNPRINTF 1

/* Define to 1 if you have the declaration of `strcasecmp', and to 0 if you
   don't. */
#define HAVE_DECL_STRCASECMP 1

/* Define to 1 if you have the declaration of `strtoull', and to 0 if you
   don't. */
#define HAVE_DECL_STRTOULL 1

/* Define to 1 if you have the declaration of `_putenv', and to 0 if you
   don't. */
#define HAVE_DECL__PUTENV 0

/* Define to 1 if you have the declaration of `_SC_LARGE_PAGESIZE', and to 0
   if you don't. */
#define HAVE_DECL__SC_LARGE_PAGESIZE 0

/* Define to 1 if you have the declaration of `_SC_NPROCESSORS_CONF', and to 0
   if you don't. */
#define HAVE_DECL__SC_NPROCESSORS_CONF 1

/* Define to 1 if you have the declaration of `_SC_NPROCESSORS_ONLN', and to 0
   if you don't. */
#define HAVE_DECL__SC_NPROCESSORS_ONLN 1

/* Define to 1 if you have the declaration of `_SC_NPROC_CONF', and to 0 if
   you don't. */
#define HAVE_DECL__SC_NPROC_CONF 0

/* Define to 1 if you have the declaration of `_SC_NPROC_ONLN', and to 0 if
   you don't. */
#define HAVE_DECL__SC_NPROC_ONLN 0

/* Define to 1 if you have the declaration of `_SC_PAGESIZE', and to 0 if you
   don't. */
#define HAVE_DECL__SC_PAGESIZE 1

/* Define to 1 if you have the declaration of `_SC_PAGE_SIZE', and to 0 if you
   don't. */
#define HAVE_DECL__SC_PAGE_SIZE 1

/* Define to 1 if you have the declaration of `_strdup', and to 0 if you
   don't. */
#define HAVE_DECL__STRDUP 0

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Define to 1 if you have the `ffs' function. */
#define HAVE_FFS 1

/* Define to 1 if you have the `ffsl' function. */
#define HAVE_FFSL 1

/* Define to 1 if you have the `fls' function. */
/* #undef HAVE_FLS */

/* Define to 1 if you have the `flsl' function. */
/* #undef HAVE_FLSL */

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if the system has the type `GROUP_AFFINITY'. */
/* #undef HAVE_GROUP_AFFINITY */

/* Define to 1 if the system has the type `GROUP_RELATIONSHIP'. */
/* #undef HAVE_GROUP_RELATIONSHIP */

/* Define to 1 if you have the `host_info' function. */
/* #undef HAVE_HOST_INFO */

/* We have hpx++ */
/* #undef HAVE_HPXPP */

/* Have support for hugetlbfs */
/* #undef HAVE_HUGETLBFS */

/* We have hwloc support */
#define HAVE_HWLOC 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* We have the jemalloc allocator */
#define HAVE_JEMALLOC 1

/* Define to 1 if the system has the type `KAFFINITY'. */
/* #undef HAVE_KAFFINITY */

/* define to 1 if the C compiler supports KNC */
/* #undef HAVE_KNC */

/* Define to 1 if you have the <kstat.h> header file. */
/* #undef HAVE_KSTAT_H */

/* We have the libcuckoo hash table */
#define HAVE_LIBCUCKOO 1

/* We have libffi */
#define HAVE_LIBFFI 1

/* We have support for libffi complex numbers */
/* #undef HAVE_LIBFFI_COMPLEX */

/* Define to 1 if we have -lgdi32 */
/* #undef HAVE_LIBGDI32 */

/* Define to 1 if we have -lkstat */
/* #undef HAVE_LIBKSTAT */

/* Define to 1 if we have -llgrp */
/* #undef HAVE_LIBLGRP */

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the <libudev.h> header file. */
/* #undef HAVE_LIBUDEV_H */

/* Define to 1 if the system has the type `LOGICAL_PROCESSOR_RELATIONSHIP'. */
/* #undef HAVE_LOGICAL_PROCESSOR_RELATIONSHIP */

/* Define to 1 if you have the <mach/mach_host.h> header file. */
/* #undef HAVE_MACH_MACH_HOST_H */

/* Define to 1 if you have the <mach/mach_init.h> header file. */
/* #undef HAVE_MACH_MACH_INIT_H */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `memalign' function. */
#define HAVE_MEMALIGN 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* We have metis support */
/* #undef HAVE_METIS */

/* MPI support available */
#define HAVE_MPI 1

/* We have a high speed network available */
#define HAVE_NETWORK 1

/* Define to 1 if you have the <numaif.h> header file. */
/* #undef HAVE_NUMAIF_H */

/* Define to 1 if the system has the type `NUMA_NODE_RELATIONSHIP'. */
/* #undef HAVE_NUMA_NODE_RELATIONSHIP */

/* Define to 1 if you have the <NVCtrl/NVCtrl.h> header file. */
/* #undef HAVE_NVCTRL_NVCTRL_H */

/* Define to 1 if you have the <nvml.h> header file. */
/* #undef HAVE_NVML_H */

/* Define to 1 if you have the `openat' function. */
#define HAVE_OPENAT 1

/* Have support for opencl */
/* #undef HAVE_OPENCL */

/* We have percolation */
/* #undef HAVE_PERCOLATION */

/* Photon support available */
/* #undef HAVE_PHOTON */

/* Define to 1 if you have the <picl.h> header file. */
/* #undef HAVE_PICL_H */

/* Enable PMI */
/* #undef HAVE_PMI */

/* Enable Cray PMI */
/* #undef HAVE_PMI_CRAY_EXT */

/* Define to 1 if you have the `posix_memalign' function. */
#define HAVE_POSIX_MEMALIGN 1

/* Define to 1 if the system has the type `PROCESSOR_CACHE_TYPE'. */
/* #undef HAVE_PROCESSOR_CACHE_TYPE */

/* Define to 1 if the system has the type `PROCESSOR_GROUP_INFO'. */
/* #undef HAVE_PROCESSOR_GROUP_INFO */

/* Define to 1 if the system has the type `PROCESSOR_NUMBER'. */
/* #undef HAVE_PROCESSOR_NUMBER */

/* Define to 1 if the system has the type `PROCESSOR_RELATIONSHIP'. */
/* #undef HAVE_PROCESSOR_RELATIONSHIP */

/* Define to '1' if program_invocation_name is present and usable */
#define HAVE_PROGRAM_INVOCATION_NAME 1

/* Define to 1 if the system has the type `PSAPI_WORKING_SET_EX_BLOCK'. */
/* #undef HAVE_PSAPI_WORKING_SET_EX_BLOCK */

/* Define to 1 if the system has the type `PSAPI_WORKING_SET_EX_INFORMATION'.
   */
/* #undef HAVE_PSAPI_WORKING_SET_EX_INFORMATION */

/* Define to 1 if you have the <pthread_np.h> header file. */
/* #undef HAVE_PTHREAD_NP_H */

/* Have PTHREAD_PRIO_INHERIT. */
#define HAVE_PTHREAD_PRIO_INHERIT 1

/* Define to 1 if the system has the type `pthread_t'. */
#define HAVE_PTHREAD_T 1

/* We have AGAS-based rebalancing */
/* #undef HAVE_REBALANCING */

/* Define to 1 if the system has the type `RelationProcessorPackage'. */
/* #undef HAVE_RELATIONPROCESSORPACKAGE */

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if the system has the type `ssize_t'. */
#define HAVE_SSIZE_T 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strtoull' function. */
/* #undef HAVE_STRTOULL */

/* Define to '1' if sysctl is present and usable */
#define HAVE_SYSCTL 1

/* Define to '1' if sysctlbyname is present and usable */
/* #undef HAVE_SYSCTLBYNAME */

/* Define to 1 if the system has the type
   `SYSTEM_LOGICAL_PROCESSOR_INFORMATION'. */
/* #undef HAVE_SYSTEM_LOGICAL_PROCESSOR_INFORMATION */

/* Define to 1 if the system has the type
   `SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX'. */
/* #undef HAVE_SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX */

/* Define to 1 if you have the <sys/cpuset.h> header file. */
/* #undef HAVE_SYS_CPUSET_H */

/* Define to 1 if you have the <sys/lgrp_user.h> header file. */
/* #undef HAVE_SYS_LGRP_USER_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/sysctl.h> header file. */
#define HAVE_SYS_SYSCTL_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* We have the tbbmalloc allocator */
/* #undef HAVE_TBBMALLOC */

/* Define to 1 if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* We have the urcu libraries */
#define HAVE_URCU 1

/* Define to 1 if you have the <valgrind/valgrind.h> header file. */
/* #undef HAVE_VALGRIND_VALGRIND_H */

/* Define to 1 if you have the <X11/keysym.h> header file. */
#define HAVE_X11_KEYSYM_H 1

/* Define to 1 if you have the <X11/Xlib.h> header file. */
#define HAVE_X11_XLIB_H 1

/* Define to 1 if you have the <X11/Xutil.h> header file. */
#define HAVE_X11_XUTIL_H 1

/* Define to 1 if the system has the `__builtin_clz' built-in function */
#define HAVE___BUILTIN_CLZ 1

/* Define to 1 if the system has the `__builtin_clzl' built-in function */
#define HAVE___BUILTIN_CLZL 1

/* Define to 1 if the system has the `__builtin_ctzl' built-in function */
#define HAVE___BUILTIN_CTZL 1

/* Define to 1 if the system has the `__builtin_expect' built-in function */
#define HAVE___BUILTIN_EXPECT 1

/* Define to 1 if the system has the `__builtin_popcountl' built-in function
   */
#define HAVE___BUILTIN_POPCOUNTL 1

/* Define to 1 if the system has the `__builtin_unreachable' built-in function
   */
#define HAVE___BUILTIN_UNREACHABLE 1

/* Define to '1' if __progname is present and usable */
#define HAVE___PROGNAME 1

/* Cacheline size */
#define HPX_CACHELINE_SIZE 64

/* OS Memory Page Size */
#define HPX_PAGE_SIZE 4096

/* The HPX interface version */
#define HPX_VERSION ""

/* Define to 1 on AIX */
/* #undef HWLOC_AIX_SYS */

/* Define to 1 on BlueGene/Q */
/* #undef HWLOC_BGQ_SYS */

/* Whether C compiler supports symbol visibility or not */
#define HWLOC_C_HAVE_VISIBILITY 1

/* Define to 1 on Darwin */
/* #undef HWLOC_DARWIN_SYS */

/* Whether we are in debugging mode or not */
/* #undef HWLOC_DEBUG */

/* Define to 1 on *FREEBSD */
/* #undef HWLOC_FREEBSD_SYS */

/* Whether your compiler has __attribute__ or not */
#define HWLOC_HAVE_ATTRIBUTE 1

/* Whether your compiler has __attribute__ aligned or not */
#define HWLOC_HAVE_ATTRIBUTE_ALIGNED 1

/* Whether your compiler has __attribute__ always_inline or not */
#define HWLOC_HAVE_ATTRIBUTE_ALWAYS_INLINE 1

/* Whether your compiler has __attribute__ cold or not */
#define HWLOC_HAVE_ATTRIBUTE_COLD 1

/* Whether your compiler has __attribute__ const or not */
#define HWLOC_HAVE_ATTRIBUTE_CONST 1

/* Whether your compiler has __attribute__ deprecated or not */
#define HWLOC_HAVE_ATTRIBUTE_DEPRECATED 1

/* Whether your compiler has __attribute__ format or not */
#define HWLOC_HAVE_ATTRIBUTE_FORMAT 1

/* Whether your compiler has __attribute__ hot or not */
#define HWLOC_HAVE_ATTRIBUTE_HOT 1

/* Whether your compiler has __attribute__ malloc or not */
#define HWLOC_HAVE_ATTRIBUTE_MALLOC 1

/* Whether your compiler has __attribute__ may_alias or not */
#define HWLOC_HAVE_ATTRIBUTE_MAY_ALIAS 1

/* Whether your compiler has __attribute__ nonnull or not */
#define HWLOC_HAVE_ATTRIBUTE_NONNULL 1

/* Whether your compiler has __attribute__ noreturn or not */
#define HWLOC_HAVE_ATTRIBUTE_NORETURN 1

/* Whether your compiler has __attribute__ no_instrument_function or not */
#define HWLOC_HAVE_ATTRIBUTE_NO_INSTRUMENT_FUNCTION 1

/* Whether your compiler has __attribute__ packed or not */
#define HWLOC_HAVE_ATTRIBUTE_PACKED 1

/* Whether your compiler has __attribute__ pure or not */
#define HWLOC_HAVE_ATTRIBUTE_PURE 1

/* Whether your compiler has __attribute__ sentinel or not */
#define HWLOC_HAVE_ATTRIBUTE_SENTINEL 1

/* Whether your compiler has __attribute__ unused or not */
#define HWLOC_HAVE_ATTRIBUTE_UNUSED 1

/* Whether your compiler has __attribute__ warn unused result or not */
#define HWLOC_HAVE_ATTRIBUTE_WARN_UNUSED_RESULT 1

/* Whether your compiler has __attribute__ weak alias or not */
#define HWLOC_HAVE_ATTRIBUTE_WEAK_ALIAS 1

/* Define to 1 if your `ffs' function is known to be broken. */
/* #undef HWLOC_HAVE_BROKEN_FFS */

/* Define to 1 if you have the `clz' function. */
/* #undef HWLOC_HAVE_CLZ */

/* Define to 1 if you have the `clzl' function. */
/* #undef HWLOC_HAVE_CLZL */

/* Define to 1 if the CPU_SET macro works */
#define HWLOC_HAVE_CPU_SET 1

/* Define to 1 if the CPU_SET_S macro works */
#define HWLOC_HAVE_CPU_SET_S 1

/* Define to 1 if you have the `cudart' SDK. */
/* #undef HWLOC_HAVE_CUDART */

/* Define to 1 if function `clz' is declared by system headers */
/* #undef HWLOC_HAVE_DECL_CLZ */

/* Define to 1 if function `clzl' is declared by system headers */
/* #undef HWLOC_HAVE_DECL_CLZL */

/* Define to 1 if function `ffs' is declared by system headers */
#define HWLOC_HAVE_DECL_FFS 1

/* Define to 1 if function `ffsl' is declared by system headers */
#define HWLOC_HAVE_DECL_FFSL 1

/* Define to 1 if function `fls' is declared by system headers */
/* #undef HWLOC_HAVE_DECL_FLS */

/* Define to 1 if function `flsl' is declared by system headers */
/* #undef HWLOC_HAVE_DECL_FLSL */

/* Define to 1 if function `strncasecmp' is declared by system headers */
#define HWLOC_HAVE_DECL_STRNCASECMP 1

/* Define to 1 if you have the `ffs' function. */
#define HWLOC_HAVE_FFS 1

/* Define to 1 if you have the `ffsl' function. */
#define HWLOC_HAVE_FFSL 1

/* Define to 1 if you have the `fls' function. */
/* #undef HWLOC_HAVE_FLS */

/* Define to 1 if you have the `flsl' function. */
/* #undef HWLOC_HAVE_FLSL */

/* Define to 1 if you have the GL module components. */
/* #undef HWLOC_HAVE_GL */

/* Define to 1 if you have libudev. */
/* #undef HWLOC_HAVE_LIBUDEV */

/* Define to 1 if you have the `libxml2' library. */
/* #undef HWLOC_HAVE_LIBXML2 */

/* Define to 1 if building the Linux PCI component */
#define HWLOC_HAVE_LINUXPCI 1

/* Define to 1 if mbind is available. */
/* #undef HWLOC_HAVE_MBIND */

/* Define to 1 if migrate_pages is available. */
/* #undef HWLOC_HAVE_MIGRATE_PAGES */

/* Define to 1 if move_pages is available. */
/* #undef HWLOC_HAVE_MOVE_PAGES */

/* Define to 1 if you have the `NVML' library. */
/* #undef HWLOC_HAVE_NVML */

/* Define to 1 if glibc provides the old prototype (without length) of
   sched_setaffinity() */
/* #undef HWLOC_HAVE_OLD_SCHED_SETAFFINITY */

/* Define to 1 if you have the `OpenCL' library. */
/* #undef HWLOC_HAVE_OPENCL */

/* Define to 1 if the hwloc library should support dynamically-loaded plugins
   */
/* #undef HWLOC_HAVE_PLUGINS */

/* `Define to 1 if you have pthread_getthrds_np' */
/* #undef HWLOC_HAVE_PTHREAD_GETTHRDS_NP */

/* Define to 1 if pthread mutexes are available */
#define HWLOC_HAVE_PTHREAD_MUTEX 1

/* Define to 1 if glibc provides a prototype of sched_setaffinity() */
#define HWLOC_HAVE_SCHED_SETAFFINITY 1

/* Define to 1 if set_mempolicy is available. */
/* #undef HWLOC_HAVE_SET_MEMPOLICY */

/* Define to 1 if you have the <stdint.h> header file. */
#define HWLOC_HAVE_STDINT_H 1

/* Define to 1 if function `syscall' is available */
#define HWLOC_HAVE_SYSCALL 1

/* Define to 1 if you have the `windows.h' header. */
/* #undef HWLOC_HAVE_WINDOWS_H */

/* Define to 1 if X11 headers including Xutil.h and keysym.h are available. */
#define HWLOC_HAVE_X11_KEYSYM 1

/* Define to 1 if you have x86 cpuid */
#define HWLOC_HAVE_X86_CPUID 1

/* Define to 1 on HP-UX */
/* #undef HWLOC_HPUX_SYS */

/* Define to 1 on Irix */
/* #undef HWLOC_IRIX_SYS */

/* Define to 1 on Linux */
#define HWLOC_LINUX_SYS 1

/* Define to 1 on *NETBSD */
/* #undef HWLOC_NETBSD_SYS */

/* Define to 1 on OSF */
/* #undef HWLOC_OSF_SYS */

/* The size of `unsigned int', as computed by sizeof */
#define HWLOC_SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof */
#define HWLOC_SIZEOF_UNSIGNED_LONG 8

/* Define to 1 on Solaris */
/* #undef HWLOC_SOLARIS_SYS */

/* The hwloc symbol prefix */
#define HWLOC_SYM_PREFIX libhpx_

/* The hwloc symbol prefix in all caps */
#define HWLOC_SYM_PREFIX_CAPS LIBHPX_

/* Whether we need to re-define all the hwloc public symbols or not */
#define HWLOC_SYM_TRANSFORM 1

/* Define to 1 on unsupported systems */
/* #undef HWLOC_UNSUPPORTED_SYS */

/* The library version, always available, even in embedded mode, contrary to
   VERSION */
#define HWLOC_VERSION "1.11.4"

/* Define to 1 on WINDOWS */
/* #undef HWLOC_WIN_SYS */

/* Define to 1 on x86_32 */
/* #undef HWLOC_X86_32_ARCH */

/* Define to 1 on x86_64 */
#define HWLOC_X86_64_ARCH 1

/* libhpx major version */
#define LIBHPX_MAJOR_VERSION 4

/* libhpx minor version */
#define LIBHPX_MINOR_VERSION 1

/* libhpx patch version */
#define LIBHPX_PATCH_VERSION 0

/* The libhpx library version */
#define LIBHPX_VERSION ""

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "hpx-5"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "hpx@indiana.edu"

/* Define to the full name of this package. */
#define PACKAGE_NAME "hpx-5"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "hpx-5 4.1.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "hpx-5"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.1.0"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 8

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Optimize URCU as far as license allows */
#define URCU_INLINE_SMALL_FUNCTIONS 1

/* Enable extensions on HP-UX. */
#ifndef _HPUX_SOURCE
# define _HPUX_SOURCE 1
#endif


/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "4.1.0"

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */

/* Are we building for HP-UX? */
#define _HPUX_SOURCE 1

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define the POSIX version */
#define _POSIX_C_SOURCE 200809L

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define this to the process ID type */
#define hwloc_pid_t pid_t

/* Define this to the thread ID type */
#define hwloc_thread_t pthread_t
