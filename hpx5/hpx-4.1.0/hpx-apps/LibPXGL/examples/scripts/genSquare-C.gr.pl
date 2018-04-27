# ============================================================================
#  genSquare-C.gr.pl
# ============================================================================

#  Author(s)       (c) 2005 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Nov 9, 2005

#  Last changed:   $Date: 2006/02/21 16:47:46 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.2 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Square-C graph family generator
#  Creates a suite of strongly connected square grid graphs:

#       - ideal num nodes 
#                   = power of two
#       - height    = sqrt(ideal num nodes)
#       - width     = num nodes / height
#       - num nodes = height * width
#       - num edges = 4 * num nodes - 4 * sqrt(num nodes)
#       - min edge weight = 0
#       - max edge weight = num nodes
#       - node ids in [1..num nodes]

#  Note: no square grid can have 2^p nodes for p = odd number
#        thus num nodes (the actual number of nodes of the generated graph) 
#        may be different from ideal num nodes = 2^p

#  Usage: > perl genSquare-C.gr.pl
# ============================================================================

# param setup:

$NEXP   = 20;      # fixed number of nodes of the graph = 2^$NEXP
$MINW   = 0;       # min weight of an arc
$START  = 0;       # max arc weight C goes from 4^$START to 4^$END
$END    = 15;
$SEED   = 971;     # seed of pseudo-random generator
$TRIALS = 1;       # number of trials per data point

$GENPROG = "../gens/grid/spgrid.exe";
$DIR     = "../inputs/Square-C";
$FORMAT  = "$DIR/Square-C.%s.%s.gr";


# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Square-C family graph generator [square grid]\n";

# creates directory (if does not exist)
system "mkdir -p $DIR";

#sets seed
srand ($SEED);

# graph size setup
$idealn = (1<<$NEXP);
$HEIGHT = int(sqrt($idealn));
$WIDTH  = int($idealn / $HEIGHT);
$n      = $HEIGHT * $WIDTH;

# generation loop:
for ( $p = $START; $p < $END + 1 ; $p++ ) { 

    $C = (1 << (2 * $p));
    $str = $NEXP % 2 ? "n=$n < $idealn=2^$NEXP" : "n=$n=2^$NEXP";
    print "\n>> Generating graph trials for data point [$str, C = 4^$p = $C]\n";

    # generate trials per data point
    for ( $trial = 0; $trial < $TRIALS; ++$trial) {
        $pathname = sprintf $FORMAT, $p, $trial;
        $trialseed = int(rand(0x7FFFFFFF));
        printf "   - Generating file %s [seed %-10s] ...", $pathname, $trialseed;
        system "$GENPROG $WIDTH $MINW $C $HEIGHT $MINW $C $trialseed > $pathname";
        print " [DONE]\n";
    }
}


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

