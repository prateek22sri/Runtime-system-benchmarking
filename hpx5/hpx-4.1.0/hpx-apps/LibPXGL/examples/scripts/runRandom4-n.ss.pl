# ============================================================================
#  runRandom4-n.ss.pl
# ============================================================================

#  Author(s)       (c) 2005 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Nov 20, 2005

#  Last changed:   $Date: 2005/11/20 23:36:25 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.1 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Random4-n family experiment driver
#  runs the ss solver on instances in the Random4-n family

#  Usage: > perl runRandom4-n.ss.pl
# ============================================================================

# param setup:
$START   = 10;      # num nodes goes from 2^$START to 2^$END
$END     = 10;      # please adjust this limit to fit your available mem
$TRIALS  = 1;       # number of trials per data point

$RESFILE   = "../results/Random4-n.ss.res";
$PREFIX    = "$ARGV[0]/inputs/Random4-n/Random4-n";
$SOLVER    = "../sssp";
$GRAPH     = "$PREFIX.%s.%s.gr";
$AUX       = "$PREFIX.%s.%s.ss";

#print $ARGV[0];
# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Random4-n family ss core experiment\n";

# open result file
open FILE, ">$RESFILE" or die "Cannot open $RESFILE for write :$!";

# generation loop:
for ( $p = $START; $p < $END + 1 ; $p++ ) { 

    # run trials per data point
    for ( $trial = 0; $trial < $TRIALS; ++$trial) {
		
        $graphname = sprintf $GRAPH, $p, $trial;
        $auxname   = sprintf $AUX, $p, $trial;
	print `working with $graphname`;
	# run experiment and collect stdout
	#$out = `mpirun -np 1 $SOLVER $graphname $auxname`;
	$out = `mpirun -np 2 --map-by node $SOLVER   -T 2  -s 20000 -p 4294967296 $graphname $auxname`;
#mpirun -np 4 --map-by node $SOLVER  -c 8  -s 8192 -p 32000000000  $graphname $auxname`;
	    print FILE $out;
	}
}

close FILE;


# ============================================================================
# Copyright (C) 2005 Camil Demetrescu, Andrew Goldberg

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
# ============================================================================

