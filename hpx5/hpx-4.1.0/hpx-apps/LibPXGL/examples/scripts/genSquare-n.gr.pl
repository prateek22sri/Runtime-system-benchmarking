# ============================================================================
#  genSquare-n.gr.pl
# ============================================================================

#  Author(s)       (c) 2005 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Nov 9, 2005

#  Last changed:   $Date: 2006/02/21 16:47:46 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.2 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Square-n graph family generator
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

#  Usage: > perl genSquare-n.gr.pl
# ============================================================================

# param setup:

$START   = 10;      # raw num nodes goes from 2^$START to 2^$END
$END     = 21;      # please adjust this limit to fit your available mem
$LOWCOST = 0;       # minimum cost of an edge (hi cost = num nodes)
$SEED    = 971;     # seed of pseudo-random generator
$TRIALS  = 1;       # number of trials per data point

$GENPROG = "../gens/grid/spgrid.exe";
$DIR     = "../inputs/Square-n";
$FORMAT  = "$DIR/Square-n.%s.%s.gr";


# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Square-n family graph generator [square grid]\n";


# creates directory (if does not exist)
system "mkdir -p $DIR";


#sets seed
srand ($SEED);


# generation loop:
for ( $p = $START; $p < $END + 1 ; $p++ ) { 

    $idealn = (1<<$p);
    $HEIGHT = int(sqrt($idealn));
    $WIDTH  = int($idealn / $HEIGHT);
    $n = $HEIGHT * $WIDTH;

    $str = $p % 2 ? "n=$n < $idealn=2^$p" : "n=$n=2^$p";
    print "\n>> Generating graph trials for data point [$str]\n";

    # generate trials per data point
    for ( $trial = 0; $trial < $TRIALS; ++$trial) {
        $pathname = sprintf $FORMAT, $p, $trial;
        $trialseed = int(rand(0x7FFFFFFF));
        printf "   - Generating file %s [seed %-10s] ...", $pathname, $trialseed;
        system "$GENPROG $WIDTH $LOWCOST $n $HEIGHT $LOWCOST $n $trialseed > $pathname";
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

