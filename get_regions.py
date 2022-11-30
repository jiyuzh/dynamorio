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

regions = sorted(regions, key=lambda x: x[1], reverse=True)

for x in regions:
  print (str(x[0])+','+str(x[0] + x[1]))
