#!/usr/bin/env python3

print("pybind11 test for refspec")

import sys
 
# appending a path
sys.path.append('../src')

try:
    import refspec
except:
    print("Error importing main module")

classes = {'SpecConfig':'Ntaps,Average1Size,Average2Size,AverageSize()'}

bag = {}

for item in classes.keys():

    try:
        bag[item]=eval("refspec."+item+"()")
    except:
        print("Error importing "+item)

    attrs = classes[item].split(",")
    for attr in attrs:
        try:
            print(item, attr, eval("bag['"+item+"']."+attr))
        except:
            print("Error in", item, attr)

