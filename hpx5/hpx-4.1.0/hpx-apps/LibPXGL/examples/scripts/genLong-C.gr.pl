# ============================================================================
#  genLong-C.gr.pl
# ============================================================================

#  Author(s)       (c) 2005 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Nov 11, 2005

#  Last changed:   $Date: 2005/11/15 11:50:52 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.1 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Long-C graph family generator
#  Creates a suite of strongly connected long grid graphs:

#       - num nodes = 2^20 (fixed)
#       - num rows  = num nodes / 16
#       - num edges = 4 * num nodes - 32 - 2 * num rows
#       - min arc weight = 0
#       - max arc weight C goes from 1 to 2^30
#       - node ids in [1..num nodes]

#  Usage: > perl genLong-C.gr.pl
# ============================================================================

# param setup:

$HEIGHT = 16;      # long grid height
$NEXP   = 20;      # fixed number of nodes of the graph = 2^$NEXP
$MINW   = 0;       # min weight of an arc
$START  = 0;       # max arc weight C goes from 4^$START to 4^$END
$END    = 15;
$SEED   = 971;     # seed of pseudo-random generator
$TRIALS = 1;       # number of trials per data point

$GENPROG = "../gens/grid/spgrid.exe";
$DIR     = "../inputs/Long-C";
$FORMAT  = "$DIR/Long-C.%s.%s.gr";


# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Long-C family graph generator [long grid, height=16]\n";

# creates directory (if does not exist)
system "mkdir -p $DIR";

#sets seed
srand ($SEED);

# graph size setup
$n = 1 << $NEXP;
$WIDTH = $n / $HEIGHT;

# generation loop:
for ( $p = $START; $p < $END + 1 ; $p++ ) { 

    $C = (1 << (2 * $p));
    print "\n>> Generating graph trials for data point [n = 2^$NEXP, C = 4^$p = $C]\n";

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

