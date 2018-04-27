# ============================================================================
#  genUSA-road-d.ss.pl
# ============================================================================

#  Author(s)       (c) 2006 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Feb 15, 2006

#  Last changed:   $Date: 2006/02/17 12:05:29 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.2 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Random sources generator for the USA-road-d family
#  Creates a suite of single-source shortest path
#  problem-specific auxiliary files.

#  Usage: > perl genUSA-road-d.ss.pl
# ============================================================================

# param setup:

$MINNSRC = 10;      # num sources for the largest graph
$SEED    = 971;     # seed of pseudo-random generator

$SRCGEN  = "../gens/utils/genSources.pl";
$FORMAT  = "../inputs/USA-road-d/USA-road-d.%s";

# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Random source generator for the USA-road-d family\n";

# routine that extracts the number of nodes and edges from a .gr file
sub GET_GRAPH_NM {

    $param = $_[0]; 

    # open graph file
    open GRFILE, "$param" or die "Cannot open $param for read :$!";

    # get number of nodes and edges of the graph
    $found = 0;
    loop: while(<GRFILE>) {
        @line = split /\s/, $_;
        if ($line[0] eq "p") {
            $n = $line[2];
            $m = $line[3];
            $found = 1;
            last loop;
        }
    }

    close GRFILE;

    if ($found == 0) {
        print "Error: graph does not contain a problem line\n";
        exit;
    }
}

# source creation routine
sub DOGEN {
    $EXT  = $_[0]; # graph instance (e.g., CTR, BAY, etc.)

    # get graph size
    $filename = sprintf $FORMAT, $EXT;
    &GET_GRAPH_NM("$filename.gr");
    $nsrc = int($MINNSRC * ($maxn / $n));

    $prefix = sprintf $FORMAT, $EXT;
    system "perl $SRCGEN $prefix.gr $prefix.ss $nsrc $SEED";
}

# get size of the largest instance
$largestfilename = sprintf $FORMAT, "CTR";
&GET_GRAPH_NM("$largestfilename.gr");
$maxn = $n;

#create instances
&DOGEN("NY");
&DOGEN("BAY");
&DOGEN("COL");
&DOGEN("FLA");
&DOGEN("NW");
&DOGEN("NE");
&DOGEN("CAL");
&DOGEN("LKS");
&DOGEN("E");
&DOGEN("W");
&DOGEN("CTR");


# ============================================================================
# Copyright (C) 2006 Camil Demetrescu, Andrew Goldberg

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

