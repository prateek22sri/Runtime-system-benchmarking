#!/usr/bin/env bash
if [ ! -d config ]; then mkdir config; fi

URL="http://stout.crest.iu.edu/photon"
DIR=$(mktemp -d photon.XXXXXXX --tmpdir=/tmp)
CONTRIB=src/contrib

# set the array of tarball deps here
contrib_tars=("libfabric-1.4.1.tar.bz2")
# set matching md5sums for each tarball
contrib_md5=("8c541ce6208a566dfbb89ae25b8420df")
# and matching target contrib directories
contrib_dirs=("libfabric")

CMD="wget --quiet -O"
if ! type "wget" > /dev/null; then
  CMD="curl --silent -o"
fi
echo -n "Downloading and extracting contrib tarballs..."
for ((i=0; i<${#contrib_tars[@]}; i++)); do
  dir=${contrib_dirs[$i]}
  file=${contrib_tars[$i]}
  md5=${contrib_md5[$i]}

  if [ -f ${CONTRIB}/$dir/.md5 ]; then
      omd5=`cat ${CONTRIB}/$dir/.md5`
  else
      omd5=""
  fi

  if [ ! -f ${CONTRIB}/$dir/configure ] || [ ! "$omd5" == "$md5" ]; then
      echo -n "$dir..."
      $CMD ${DIR}/$file ${URL}/$dir/$md5/$file
      if [ $? -ne 0 ]; then
	  echo
	  echo "!!! Error downloading $dir contrib with md5sum $md5"
	  continue
      fi
      mkdir -p ${CONTRIB}/$dir
      tar --strip-components=1 -xf ${DIR}/$file -C ${CONTRIB}/$dir
      echo -n $md5 > ${CONTRIB}/$dir/.md5
  fi
done
rm -rf ${DIR}
echo "DONE"

set -e
autoreconf --force --install -I config || exit 1
rm -rf autom4te.cache
