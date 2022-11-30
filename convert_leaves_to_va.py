#!/usr/bin/python

import sys 

for line in sys.stdin:
  line = line.strip()
  lbound, hbound = line.split(",", 1)
  lbound = long(lbound) << (12 + 9)
  hbound = long(hbound) << (12 + 9)

  print str(format(lbound, '#04x')) + ',' + str(format(hbound, '#04x'))
