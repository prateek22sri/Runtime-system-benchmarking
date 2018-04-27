# ============================================================================
#  genRandom4-n.gr.pl
# ============================================================================

#  Author(s)       (c) 2005 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Nov 8, 2005

#  Last changed:   $Date: 2006/02/21 16:47:46 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.2 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Random4-n graph family generator
#  Creates a suite of strongly connected random graphs with:

#       - num nodes = power of two
#       - num edges = 4 * num nodes
#       - min edge weight = 0
#       - max edge weight = num nodes
#       - node ids in [1..num nodes]

#  Usage: > perl genRandom4-n.gr.pl
# ============================================================================

# param setup:

$START   = 10;      # num nodes goes from 2^$START to 2^$END
$END     = 21;      # please adjust this limit to fit your available mem
$LOWCOST = 0;       # minimum cost of an edge (hi cost = num nodes)
$SEED    = 971;     # seed of pseudo-random generator
$TRIALS  = 1;       # number of trials per data point

$GENPROG = "../gens/rand/sprand.exe";
$DIR     = "../inputs/Random4-n";
$FORMAT  = "$DIR/Random4-n.%s.%s.gr";


# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Random4-n family graph generator [degree 4]\n";


# creates directory (if does not exist)
system "mkdir -p $DIR";


#sets seed
srand ($SEED);


# generation loop:
for ( $p = $START; $p < $END + 1 ; $p++ ) { 

    $n = (1<<$p);
    print "\n>> Generating graph trials for data point [n=2^$p=$n]\n";

    # generate trials per data point
    for ( $trial = 0; $trial < $TRIALS; ++$trial) {
        $pathname = sprintf $FORMAT, $p, $trial;
        $trialseed = int(rand(0x7FFFFFFF));
        printf "   - Generating file %s [seed %-10s] ...", $pathname, $trialseed;
        $m = $n * 4;
        system "$GENPROG $n $m $LOWCOST $n $trialseed > $pathname";
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

