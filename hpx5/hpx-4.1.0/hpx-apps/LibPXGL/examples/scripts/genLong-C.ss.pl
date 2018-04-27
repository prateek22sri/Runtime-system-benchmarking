# ============================================================================
#  genLong-C.ss.pl
# ============================================================================

#  Author(s)       (c) 2005 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Nov 11, 2005

#  Last changed:   $Date: 2006/02/21 16:47:46 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.2 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Random sources generator for the Long-C family
#  Creates a suite of single-source shortest path
#  problem-specific auxiliary files.

#  Usage: > perl genLong-C.ss.pl
# ============================================================================

# param setup:

$NUMSRC = 10;      # num sources
$NEXP   = 20;      # fixed number of nodes of the graph = 2^$NEXP
$START  = 0;       # max arc weight C goes from 4^$START to 4^$END
$END    = 15;
$SEED   = 971;     # seed of pseudo-random generator
$TRIALS = 1;       # number of trials per data point

$DIR     = "../inputs/Long-C";
$FORMAT = "$DIR/Long-C.%s.%s.ss";


# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Random sources generator for the Long-C family\n";

# creates directory (if does not exist)
system "mkdir -p $DIR";

# graph size setup
$n = 1 << $NEXP;

# generation loop:
for ( $p = $START; $p < $END + 1 ; $p++ ) { 

    #init seed
    srand ($SEED);

    $C = (1 << (2 * $p));
    print "\n>> Generating ss instances for data point [n = 2^$NEXP, C = 4^$p = $C]\n";

    # generate trials per data point
    for ( $trial = 0; $trial < $TRIALS; ++$trial) {

        $pathname = sprintf $FORMAT, $p, $trial;
        $trialseed = int(rand(0x7FFFFFFF));
        printf "   - Generating file %s [seed %-10s] ...",$pathname, $trialseed;

        # open file in write mode
        open FILE, ">$pathname" or die "Cannot open $pathname for write :$!";

        # write file header
        print FILE "c 9th DIMACS Implementation Challenge: Shortest Paths\n";
        print FILE "c http://www.dis.uniroma1.it/~challenge9\n";
        print FILE "c Long-C family ss problem instance\n";
        print FILE "c\n";
        print FILE "p aux sp ss $NUMSRC\n";
        print FILE "c graph contains $n nodes\n";
        print FILE "c file contains $NUMSRC$ source lines\n";
        print FILE "c\n";

        # write $NUMSRC random sources to the file
        for ( $i = 0; $i < $NUMSRC; ++$i ) {
            $src = 1 + int(rand($n));            
            print FILE "s $src\n";
        }

        # close file
        close FILE;
        
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

