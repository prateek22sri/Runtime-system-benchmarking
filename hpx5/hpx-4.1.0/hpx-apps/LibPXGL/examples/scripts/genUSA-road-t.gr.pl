# ============================================================================
#  genUSA-road-t.gr.pl
# ============================================================================

#  Author(s)       (c) 2006 Camil Demetrescu, Andrew Goldberg
#  License:        See the end of this file for license information
#  Created:        Feb 15, 2006

#  Last changed:   $Date: 2006/02/21 21:45:33 $
#  Changed by:     $Author: demetres $
#  Revision:       $Revision: 1.1 $

#  9th DIMACS Implementation Challenge: Shortest Paths
#  http://www.dis.uniroma1.it/~challenge9

#  Installer for the USA-road-t family
#  Downloads the USA-road-t family from www.dis.uniroma1.it

#  Usage: > perl genUSA-road-t.gr.pl
# ============================================================================

# param setup:

$URL = "http://www.dis.uniroma1.it/challenge9/data/USA-road-t/USA-road-t";
$DIR = "../inputs/USA-road-t";

# header:
print "\n* 9th DIMACS Implementation Challenge: Shortest Paths\n";
print   "* http://www.dis.uniroma1.it/~challenge9\n";
print   "* Installer for the USA-road-t family\n";

# creates directory (if does not exist)
system "mkdir -p $DIR";

# instance download routine
sub DOGEN {
    $EXT = $_[0]; # graph instance (e.g., CTR, BAY, etc.)
    system "cd $DIR; echo bye | ftp $URL.$EXT.gr.gz";
#   system "cd $DIR; echo bye | ftp $URL.$EXT.co.gz";
    system "gunzip $DIR/*.$EXT.gr.gz";
}

# download instances
&DOGEN("NY");
&DOGEN("BAY");
&DOGEN("COL");
&DOGEN("FLA");
&DOGEN("NE");
&DOGEN("NW");
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

