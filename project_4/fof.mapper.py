#!/usr/bin/python

import sys
import re

for line in sys.stdin:
	line = line.strip()
	strs = line.split()
	if(len(strs)<3):
		continue

	first_word = strs[0]
	val = 1
	for i in range(1,len(strs)):
		for j in range(i+1,len(strs)):
			sort_list = [strs[0],strs[i],strs[j]]
			sort_list.sort()
			print("%s_%s_%s\t%s".format(str(sort_list[0]),str(sort_list[1]),str(sort_list[2]),str(val)))
			