BEGIN {
  i=0
  print "locality,rank,iteration,event,time,offset,relative"
}

/^START_LOG/ {
  print "#",$0 > "/dev/stderr"
}

/^PROGNAME/ {
  print "#",$0 > "/dev/stderr"
}

/^Elapsed/ {
  print "#",$0 > "/dev/stderr"
}

/^Run completed:/ {
  print "#",$0 > "/dev/stderr"
}

/Problem size/ {
  print "#",$0 > "/dev/stderr"
}

/Iteration count/ {
  print "#",$0 > "/dev/stderr"
}

/Final/ {
  print "#",$0 > "/dev/stderr"
}

/trace sample:/ {
  print i++",",$3,$4,$5,$6,$7,$8,$9
}

END {
  print "\n\n" > "/dev/stderr"
}
