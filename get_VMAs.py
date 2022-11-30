#!/usr/bin/env python

import numpy as np 
import sys

data = np.loadtxt(sys.stdin, dtype=np.int64)
data = np.sort(data)

prev = None
regions = []
region_size = 0
cur = data[0]
for i in range(len(data)):
  if prev is not None:
    if prev != data[i] - 1:
      regions.append((cur, region_size))
      region_size = 0
      cur = data[i]
  prev = data[i]
  region_size += 1

regions = sorted(regions, key=lambda x: x[0], reverse=False)

VMA = []

cur_region = (regions[0][0], regions[0][0] + regions[0][1])
for region in regions[1:]:
	if (( region[0] - cur_region[1]) < 1000 ):
		cur_region = (cur_region[0], region[0] + region[1])
	else:
		VMA.append(cur_region)
		cur_region = (region[0], region[0] + region[1])

#	print cur_region

VMA.append(cur_region)

	
for x in VMA:
	print x 
  

#regions = sorted(regions, key=lambda x: x[1], reverse=True)
#
#for x in regions:
#  #print (str(x[0])+','+str(x[0] + x[1]))
#  print (str(x[0])+','+str(x[0] + x[1]),x[1])
