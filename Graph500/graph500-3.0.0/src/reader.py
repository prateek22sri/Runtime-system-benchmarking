import numpy as np


# edge list
f = open("test1", "r")
a = np.fromfile(f, dtype=np.uint32)
print [list(a[i:i+3]) for i in range(0,len(a),3)]

# edge weights
f = open("test1.weights", "r")
a = np.fromfile(f, dtype=np.uint32)
print a

