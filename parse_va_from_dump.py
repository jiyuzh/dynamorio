#!/usr/bin/python

import sys 
import re

def readIndex(line):
  line = re.sub('[#]', '', line)
  line = line.split("entry",1)[1].split()[0]
  return int(line)

def readPhysical(line):
  line = line.split()[5]
  return int(line, 16)

def readVirtual(line):
  line = line.split()[4]
  return int(line, 16)

# this function builds the whole VA from four 9 bit pieces 
# and returns the Va as a long int
def convertToAddr(current_va):
  return (long(sum( [ a*b for a,b in zip( current_va, [ 512**i for i in reversed(range(0, len(current_va))) ] ) ] )) * 1024 * 4)
  
tokens = [ 'PML4', 'PDPT', 'PD', 'PT' ]

# four parts of VA address,
# each element represents 9 bits and in [0 , 511]
# last element in the list correspons to LSBs of VA (PT index)
current_va = [0, 0, 0, 0]
current_path = [0, 0, 0, 0]
CR3 = 0
for line in sys.stdin:
  line = line.strip()

  if 'CR3' in line:
    CR3 = int(line.split()[2], 16)
  
#  if 'USR' in line:
  if '-page' in line: 
    for token_id, token in enumerate(tokens):  
      if token in line:  
        current_va[token_id] = readIndex(line);
        current_path[token_id] = readPhysical(line);
        # process huge pages - fill all remaining LSBs with 0s
        #for i in range(token_id+1, len(current_va)):
        #  current_va[i] = 0
        break
    if 'PT' in line:
      #print str(convertToAddr(current_va)) + ',' + str(CR3) + ',' + ','.join(map(str,current_path))
      #print str(readVirtual(line)) + ',' + str(CR3) + ',' + ','.join(map(str,current_path))
      def make_hex(x):
        return str(format(int(x), '#04x'))
      print make_hex(readVirtual(line)) + ',' + make_hex(CR3) + ',' + ','.join(map(make_hex,current_path))
    # debug
    #print(line, token, current_va, bin(convertToAddr(current_va)))

  for token_id, token in enumerate(tokens):  
    if (token + ' entry') in line:  
      current_va[token_id] = readIndex(line);
      current_path[token_id] = readPhysical(line);
      break    
