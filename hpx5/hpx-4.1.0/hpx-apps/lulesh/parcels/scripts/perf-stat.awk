BEGIN {
  SUBSEP=","
  header="Lulesh Version"SUBSEP"Network"SUBSEP"Configuration"SUBSEP"Link"SUBSEP"Problem Size"SUBSEP"Cores"SUBSEP"Processes"SUBSEP"PE"SUBSEP"Domains"SUBSEP"x"SUBSEP"Iterations"SUBSEP"RC"SUBSEP"Time"SUBSEP"SBN3"SUBSEP"PosVel"SUBSEP"Monoq"
}

/^lulesh$/ {
}

/^size:/ {
  size=$2
}

/^np:/ {
  np=$2
}

/^n:/ {
  n=$2
}

/^x/ {
  x=$2
}

/^i:/ {
  i=$2
}

/^pe:/ {
  pe=$2
}

/^net:/ {
  net=$2
}

/^build:/ {
  build=$2
}

/^link:/{
  link=$2
}

/:Elapsed time =/ {
  time=$4
}

/:time_in_SBN3/ {
  sbn3=$3
}

/:time_in_PosVel/ {
  posvel=$3
}

/:time_in_MonoQ/ {
  monoq=$3
}

/^[[0-9]*,[0-9]*]<stdout>:[0-9]*,/ {
  split($0,a,":")
  split(a[2],b,",")
  counters[b[2]]+=b[1]
  delete a
  delete b
}

/^hselul/ {
  if (header != "") {
    for (key in counters) {
      header=header SUBSEP key
    }
    print header
    header=""
  }
  rc=$2
  cores=np*pe
  sample="parcels" SUBSEP net SUBSEP build SUBSEP link SUBSEP size SUBSEP cores SUBSEP np SUBSEP pe SUBSEP n SUBSEP x SUBSEP i SUBSEP rc SUBSEP time SUBSEP sbn3 SUBSEP posvel SUBSEP monoq

  for (key in counters) {
    sample=sample SUBSEP counters[key]
  }
  print sample
  delete counters
}
