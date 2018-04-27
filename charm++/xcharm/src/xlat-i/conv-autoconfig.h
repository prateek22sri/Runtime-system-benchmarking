/* conv-autoconfig.h.  Generated from conv-autoconfig.h.in by configure.  */
/* conv-autoconfig.h.in.  Generated from configure.in by autoheader.  */

/* enable ampi error checking */
#define AMPI_ERROR_CHECKING 1

/* Charm++ Release/API version number */
#define CHARM_VERSION 60800

/* whether Cray gni_pub has GNI_GetBIConfig */
#define CMK_BALANCED_INJECTION_API 1

/* bproc version */
/* #undef CMK_BPROC_VERSION */

/* build MPI. */
/* #undef CMK_BUILD_ON_MPI */

/* enable ccs */
#define CMK_CCS_AVAILABLE 1

/* enable charmdebug */
#define CMK_CHARMDEBUG 0

/* whether c compiler knows of the c11 standard */
/* #undef CMK_COMPILER_KNOWS_C11 */

/* whether c++ compiler knows of the cpp11 standard */
/* #undef CMK_COMPILER_KNOWS_CPP11 */

/* whether ucontext has pointer */
#define CMK_CONTEXT_FPU_POINTER 1

/* whether ucontext uses uc_regs union */
#define CMK_CONTEXT_FPU_POINTER_UCREGS 0

/* whether ucontext has pointer (v_regs) of vector type */
#define CMK_CONTEXT_V_REGS 0

/* Cray MAXNID */
/* #undef CMK_CRAY_MAXNID */

/* Disables conflicting macros. */
/* #undef CMK_CXX_MPI_BINDINGS */

/* whether C inline works in C */
#define CMK_C_INLINE 1

/* whether sync_add_and_fetch primitive works in C */
#define CMK_C_SYNC_ADD_AND_FETCH_PRIMITIVE 1

/* whether sync_synchronize primitives works in C */
#define CMK_C_SYNC_SYNCHRONIZE_PRIMITIVE 1

/* whether C++ allows declaration of variable sized array */
#define CMK_DECLARE_VARSIZE_ARRAY 1

/* dlopen */
#define CMK_DLL_USE_DLOPEN 1

/* whether getProcAddress works */
#define CMK_DLL_USE_WIN32 0

/* enable error checking */
#define CMK_ERROR_CHECKING 0

/* ALLCAPS */
/* #undef CMK_FORTRAN_USES_ALLCAPS */

/* NOSCORE */
/* #undef CMK_FORTRAN_USES_NOSCORE */

/* ONESCORE */
#define CMK_FORTRAN_USES_ONESCORE 1

/* TWOSCORE */
/* #undef CMK_FORTRAN_USES_TWOSCORE */

/* Allows gcc x86 assembly. */
#define CMK_GCC_X86_ASM 1

/* Allows gcc x86 assembly for atomic increment. */
#define CMK_GCC_X86_ASM_ATOMICINCREMENT 1

/* whether personality() and ADDR_NO_RANDOMIZE exist */
#define CMK_HAS_ADDR_NO_RANDOMIZE 1

/* whether compiler supports std::alignment_of */
#define CMK_HAS_ALIGNMENT_OF 1

/* whether has alloca.h */
#define CMK_HAS_ALLOCA_H 1

/* whether anon structs are permitted */
#define CMK_HAS_ANON_STRUCTS 1

/* whether has asctime */
#define CMK_HAS_ASCTIME 1

/* whether the bindprocessor() exists */
#define CMK_HAS_BINDPROCESSOR 0

/* whether PAPI exists */
/* #undef CMK_HAS_COUNTER_PAPI */

/* whether C++ library has <cstdatomic> */
/* #undef CMK_HAS_CXX0X_CSTDATOMIC */

/* whether C++ library has <atomic> */
#define CMK_HAS_CXX11_ATOMIC 1

/* whether has elf.h */
#define CMK_HAS_ELF_H 1

/* whether has fabsf */
#define CMK_HAS_FABSF 1

/* whether has fdatasync */
#define CMK_HAS_FDATASYNC_FUNC 1

/* whether has fsync */
#define CMK_HAS_FSYNC_FUNC 1

/* whether gethostname() exists */
#define CMK_HAS_GETHOSTNAME 1

/* whether getifaddrs() exists */
#define CMK_HAS_GETIFADDRS 1

/* whether getpagesize exists */
#define CMK_HAS_GETPAGESIZE 1

/* whether getpid exists */
#define CMK_HAS_GETPID 1

/* whether has get_myaddress */
#define CMK_HAS_GET_MYADDRESS 1

/* whether compiler supports std::is_constructible */
#define CMK_HAS_IS_CONSTRUCTIBLE 1

/* whether iterator_traits works */
#define CMK_HAS_ITERATOR_TRAITS 1

/* whether kill exists */
#define CMK_HAS_KILL 1

/* whether has log2 */
#define CMK_HAS_LOG2 1

/* whether has mallinfo */
#define CMK_HAS_MALLINFO 1

/* whether has malloc.h */
#define CMK_HAS_MALLOC_H 1

/* whether the mmap() syscall exists */
#define CMK_HAS_MMAP 1

/* whether mmap() accepts MAP_ANON */
#define CMK_HAS_MMAP_ANON 1

/* whether mmap() accepts MAP_NORESERVE */
#define CMK_HAS_MMAP_NORESERVE 1

/* whether has mprotect */
#define CMK_HAS_MPROTECT 1

/* whether has mstats */
#define CMK_HAS_MSTATS 0

/* whether has Multiprocessing.h */
#define CMK_HAS_MULTIPROCESSING_H 0

/* whether ntohl is available */
#define CMK_HAS_NTOHL 1

/* whether NUMA control related functions exist */
/* #undef CMK_HAS_NUMACTRL */

/* whether offsetof exists */
#define CMK_HAS_OFFSETOF 1

/* whether PMI_Get_nid exists */
#define CMK_HAS_PMI_GET_NID 1

/* whether has popen */
#define CMK_HAS_POPEN 1

/* whether the pthread_setaffinity_np() exists */
#define CMK_HAS_PTHREAD_SETAFFINITY 1

/* whether Python is installed */
#define CMK_HAS_PYTHON 0

/* whether Cray rca library is available */
#define CMK_HAS_RCALIB 1

/* whether Cray rca has rca_get_max_dimension */
#define CMK_HAS_RCA_MAX_DIMENSION 1

/* whether has regex.h */
#define CMK_HAS_REGEX_H 1

/* whether getrusage accepts RUSAGE_THREAD */
#define CMK_HAS_RUSAGE_THREAD 1

/* whether has sbrk */
#define CMK_HAS_SBRK 1

/* whether the sched_setaffinity() exists */
#define CMK_HAS_SETAFFINITY 1

/* whether the setpriority exists */
#define CMK_HAS_SETPRIORITY 1

/* whether has sleep */
#define CMK_HAS_SLEEP 1

/* whether has socklen_t */
#define CMK_HAS_SOCKLEN 1

/* whether the pthread_spin_lock exists */
#define CMK_HAS_SPINLOCK 1

/* whether has sqrtf */
#define CMK_HAS_SQRTF 1

/* whether has stdint.h */
#define CMK_HAS_STDINT_H 1

/* whether std::distance works */
#define CMK_HAS_STD_DISTANCE 1

/* whether std::inserter works */
#define CMK_HAS_STD_INSERTER 1

/* whether std::unordered_map works */
#define CMK_HAS_STD_UNORDERED_MAP 1

/* sync program */
#define CMK_HAS_SYNC 1

/* whether has sync */
#define CMK_HAS_SYNC_FUNC 1

/* whether has system */
#define CMK_HAS_SYSTEM 1

/* Allows __thread. */
#define CMK_HAS_TLS_VARIABLES 1

/* whether typeinfo/typeid works */
#define CMK_HAS_TYPEINFO 1

/* whether has _setjmp/_longjmp */
#define CMK_HAS_UNDERSCORE_SETJMP 1

/* whether has usleep */
#define CMK_HAS_USLEEP 1

/* whether has values.h */
#define CMK_HAS_VALUES_H 1

/* whether ibv_port_attr has link_layer field */
#define CMK_IBV_PORT_ATTR_HAS_LINK_LAYER 1

/* Setting load balancing timer type */
#define CMK_LBTIME_TYPE double

/* disable lb user data */
#define CMK_LB_USER_DATA 0

/* whether long double works */
#define CMK_LONG_DOUBLE_DEFINED 1

/* whether long long works */
#define CMK_LONG_LONG_DEFINED 1

/* machine name */
#define CMK_MACHINE_NAME "gni-crayxe"

/* Allows MPI_Init_thread. */
/* #undef CMK_MPI_INIT_THREAD */

/* expected message priorities are arbitrarily sized */
#define CMK_MSG_PRIO_TYPE bitvec

/* whether operator delete can be overloaded in same class */
#define CMK_MULTIPLE_DELETE 1

/* OS is Linux */
#define CMK_OS_IS_LINUX 1

/* whether is power7 */
/* #undef CMK_POWER7 */

/* Allows asm eieio assembly. */
#define CMK_PPC_ASM 0

/* whether has zlib */
#define CMK_PROJECTIONS_USE_ZLIB 1

/* Python version */
#define CMK_PYTHON_VERSION 2.6

/* disable the randomized msgq in the scheduler */
#define CMK_RANDOMIZED_MSGQ 0

/* envelope refnum field set to UInt */
#define CMK_REFNUM_TYPE unsigned short

/* disable replay */
#define CMK_REPLAYSYSTEM 0

/* disable shrinkexpand */
#define CMK_SHRINK_EXPAND 0

/* whether C++ signed char and char differ */
#define CMK_SIGNEDCHAR_DIFF_CHAR 1

/* whether to use signal-safe system() */
#define CMK_SIGSAFE_SYSTEM 0

/* whether size_t 64bit */
#define CMK_SIZET_64BIT 1

/* disable tracing comm thread */
#define CMK_SMP_TRACE_COMMTHREAD 0

/* Allows switching TLS in 32-bit. */
/* #undef CMK_TLS_SWITCHING32 */

/* Allows switching TLS 64-bit. */
#define CMK_TLS_SWITCHING64 1

/* enable tracing */
#define CMK_TRACE_ENABLED 0

/* whether glibc backtrace works */
#define CMK_USE_BACKTRACE 1

/* whether has libjpeg */
#define CMK_USE_LIBJPEG 1

/* whether has mkstemp */
#define CMK_USE_MKSTEMP 1

/* whether the poll syscall exists */
#define CMK_USE_POLL 1

/* whether charm scheduler should use an STL-based msg q */
#define CMK_USE_STL_MSGQ 0

/* disable controlpoint */
#define CMK_WITH_CONTROLPOINT 0

/* enable statistics collection */
#define CMK_WITH_STATS 0

/* whether __int128 works */
#define CMK___int128_DEFINED 1

/* whether __int128_t works */
#define CMK___int128_t_DEFINED 1

/* whether __int64 works */
#define CMK___int64_DEFINED 0

/* Define to the address where bug reports for this package should be sent. */
//#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
//#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
//#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
//#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
//#define PACKAGE_URL ""

/* Define to the version of this package. */
//#define PACKAGE_VERSION ""

/* enable qlogic */
#define QLOGIC 0
