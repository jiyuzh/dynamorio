#!/usr/bin/python

import sys 

for line in sys.stdin:
  line = line.strip()
  int_value = long(line, 16)
  new_int_value = (int_value >> (12 + 9))

  print(new_int_value)
