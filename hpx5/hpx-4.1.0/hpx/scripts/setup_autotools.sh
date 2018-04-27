#!/bin/bash

if [ "$#" -lt 1 ]; then
    echo "Must specify an install prefix!"
    exit
fi

PREFIX=$1

DLDIR=$(mktemp -d hpx.XXXXXXX --tmpdir=/tmp)
SAVEDIR=$PWD
SAVEPATH=$PATH
PATH=${PREFIX}/bin:$SAVEPATH

M4=m4-1.4.17
AM=automake-1.15
AC=autoconf-2.69
LT=libtool-2.4.6

CMD="wget -O"
if ! type "wget" > /dev/null; then
  CMD="curl -o"
fi

echo -n "Downloading packages to ${DLDIR}..."
${CMD} ${DLDIR}/${M4}.tar.gz http://ftp.gnu.org/gnu/m4/${M4}.tar.gz 
${CMD} ${DLDIR}/${AM}.tar.gz http://ftp.gnu.org/gnu/automake/${AM}.tar.gz
${CMD} ${DLDIR}/${AC}.tar.gz http://ftp.gnu.org/gnu/autoconf/${AC}.tar.gz
${CMD} ${DLDIR}/${LT}.tar.gz http://ftpmirror.gnu.org/libtool/${LT}.tar.gz
echo "done"

cd $DLDIR

for PKG in $M4 $AC $AM $LT; do
    echo -n "Installing $PKG..."
    tar -xf ${PKG}.tar.gz
    cd $PKG
    ./configure --prefix=${PREFIX}
    make -j 4 && make install
    cd ..
    echo "done"
done

PATH=$SAVEPATH
cd $SAVEDIR
rm -rf $DLDIR

cat <<EOF
Packages installed to:

$PREFIX.  

Update your PATH environment variable to use the new autotools.

For example:
   $ export PATH=${PREFIX}/bin:\$PATH
EOF
