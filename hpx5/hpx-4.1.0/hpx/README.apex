--------------------------------------------------------------------------------
General Instructions for building HPX-5 with APEX measurement/adaptation support
--------------------------------------------------------------------------------

To include APEX support in HPX-5, add the APEX pkgconfig path to the 
PKG_CONFIG_PATH environment variable before running ./configure. 
For example:

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/path/to/apex/installation/lib/pkgconfig

Then, add the "--with-apex --enable-instrumentation" options to the configure. For example:

./configure --enable-photon --enable-mpi --with-apex={=system,PKG} --enable-instrumentation

For instructions on downloading and building APEX, please see:
https://github.com/khuck/xpress-apex

To build HPX-5 with APEX on Edison or Cori (@NERSC), do the following:

-----------------------------------------------------
1) load the right modules
-----------------------------------------------------

module swap PrgEnv-intel PrgEnv-gnu
module load gcc/5.3.0
module load cmake/3.3.2
module load automake/1.15
module load autoconf
module unload darshan
module load papi/5.4.1.3

-----------------------------------------------------
2) build APEX
-----------------------------------------------------

APEX is built with CMake. The minimum CMake settings needed
for APEX are:

-DCMAKE_INSTALL_PREFIX=<some path to an installation location>

1. Get the code (two methods):

wget https://github.com/khuck/xpress-apex/archive/v1.0.tar.gz
tar -xvzf v1.0.tar.gz

...or...

git clone https://github.com/khuck/xpress-apex.git
cd xpress-apex
git checkout v1.0

2. Enter the repo directory, make a build directory:

cd xpress-apex-1.0
mkdir build
cd build

3. configure using CMake:

cmake -DCMAKE_INSTALL_PREFIX=<installation-path> \
   -DUSE_BFD=TRUE -DUSE_ACTIVEHARMONY=TRUE -DUSE_OTF2=TRUE ..

Enabling BFD (binutils), Active Harmony and OTF2 will enable the 
most useful and relevant options for use with HPX.  If the 
libraries are not found by CMake, the build will automatically
download and build them. (See the APEX documentation for more
details: https://github.com/khuck/xpress-apex)

4. build with make:

make
make test
make doc
make install

Other CMake setting, depending on you needs can be set:
https://github.com/khuck/xpress-apex

-----------------------------------------------------
3) Build HPX
-----------------------------------------------------

# get HPX from gitlab
git checkout git@gitlab.crest.iu.edu:extreme/hpx.git

# set environment variables for HPX for photon support
export CRAYPE_LINK_TYPE=dynamic
export HPX_PHOTON_BACKEND=ugni

#set Pkgconfig for APEX
XPRESS_APEX=${HOME}/src/xpress-apex/install/lib/pkgconfig
if [ ${PKG_CONFIG_PATH} ] ; then
    export PKG_CONFIG_PATH=$XPRESS_APEX:${PKG_CONFIG_PATH}
else
    export PKG_CONFIG_PATH=$XPRESS_APEX
fi

# set the path to hpx-autotools, wherever they are
export PATH=/path/to/install/bin:$PATH
# bootstrap to make the configure script with APEX options
./bootstrap
# run configure
./configure --prefix=path-to-hpx-install --enable-mpi CC=cc PHOTON_CARGS="--enable-ugni --disable-libfabric" --enable-photon --with-apex --enable-instrumentation
make && make install

-----------------------------------------------------
4) Build LULESH
-----------------------------------------------------

# (get hpx-apps from gitlab)
cd hpx-apps/lulesh/parcels
$ export PKG_CONFIG_PATH=/path-to-hpx-install/lib/pkgconfig:$PKG_CONFIG_PATH
$ export LD_LIBRARY_PATH=/path-to-hpx-install/lib:$LD_LIBRARY_PATH

Edit the lulesh-hpx.c

diff --git a/lulesh/parcels/lulesh-hpx.c b/lulesh/parcels/lulesh-hpx.c
index 9cd2fc5..3313a46 100644
--- a/lulesh/parcels/lulesh-hpx.c
+++ b/lulesh/parcels/lulesh-hpx.c
@@ -18,6 +18,10 @@
 #include "lulesh-hpx.h"
 #include "trace.h"

+#if defined(HAVE_APEX)
+#include "apex.h"
+#endif
+
 #ifdef DIAGNOSTICS
 uint64_t remote_comms = 0;
 uint64_t total_comms = 0;
@@ -476,6 +480,10 @@ int main(int argc, char **argv) {
     printf(" Number of domains: %d nx: %d maxcycles: %d core-major ordering: %d\n",nDoms,nx,maxcycles,core_major);
   }

+#ifdef HAVE_APEX
+  apex_setup_power_cap_throttling();
+#endif
+
   e = hpx_run(&_main, &nDoms, &nx, &maxcycles);
   hpx_finalize();
   return e;

./bootstrap
./configure CC=cc CFLAGS="-O3"
make

# Set runtime environment variables for APEX before the srun call.

export APEX_PROFILE_OUTPUT=0
export APEX_SCREEN_OUTPUT=1
export APEX_POLICY=1
export APEX_CONCURRENCY=1
export APEX_THROTTLING=1
export APEX_ENERGY_THROTTLING=1
export APEX_THROTTLING_MAX_THREADS=48
export APEX_THROTTLING_MIN_THREADS=12
export APEX_THROTTLING_MAX_WATTS=200.0
export APEX_THROTTLING_MIN_WATTS=180.0

salloc -N 42
srun -N 42 ./luleshparcels -n 1000 -x 48 -i 100 --hpx-threads=48 --hpx-network=pwc

# To see the concurrency charts (contact khuck@cs.uoregon.edu for a python script
# to aggregate them into one script):

module load gnuplot
gnuplot-persist concurrency.*.gnuplot

# Documentation references

1. User documentation to APEX - http://khuck.github.io/xpress-apex/.
2. Detailed HPX-5 specific APEX documentation can be found at:
http://khuck.github.io/xpress-apex/usage/#hpx-5-indiana-university.
