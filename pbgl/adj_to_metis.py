#!/usr/bin/env python

# This file converts an adjacency list into a .gr file accepted by the METIS reader delete the last two lines from a vim editor to make this code run

import itertools
from itertools import islice
from pprint import pprint

class adj_to_metis:

    # initializing variables
    dict = {}

    def __init__(self):
        self.convert_to_metis()

    # def num_vertices(self):
    #     return len(self.dict)
    #
    def num_edges(self):
        count = 0
        for k,v in self.dict.iteritems():
            count += len(v)
        return count

    def convert_to_metis(self):

        num_vertices = 0
        num_edges = 0
        count = 1

        with open("file10", "r") as f:

            num_edges = int(next(islice(f, 4 - 1, 4)).split(':')[1].rstrip())
            num_vertices = int(next(islice(f, 1 - 1, 1)).split(':')[1].rstrip())



            if count == 1:
                for x in range(1,num_vertices+1):
                    self.dict[x] = []
                    count+=1

            for line in itertools.islice(f, 1, None):
                a = [int(i) for i in line.rstrip().split()]
                # print a
                if a[0] not in self.dict:
                    print "Error!"
                    if a[1] not in self.dict:
                        print "Error!"
                else:
                    self.dict[a[0]].append((a[1], a[2]))
                    if a[1] not in self.dict:
                        print "Error!"

        # pprint(self.dict)
        g = open('file10.gr', 'w')
        print >> g, (str(num_vertices) + " " + str(num_edges) + " 1")
        for k in sorted(self.dict.iterkeys()):
            data = [str(element) for tup in self.dict[k] for element in tup ]
            g.write(' '.join(data) + '\n')


        g.close()

        if self.num_edges() == num_edges:
            print "Affirmative! Edge match confirmed"
        else:
            print "Negative! Edges match refused!"


def main():
    adj_to_metis()

if __name__== "__main__":
    main()
