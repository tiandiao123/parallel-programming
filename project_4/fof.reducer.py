#!/usr/bin/python

import sys
import re


def printFriends(word1,word2,word3):
	list = [word2,word3]
	list.sort()
	print "%s %s %s"%(str(word1),str(list[0]),str(list[1]))

previous_word = None
count_word = 0
for line in sys.stdin:
	key,value = line.split('\t')

	if previous_word == None:
		previous_word = key
		count_word = 1
		continue
	
	if key == previous_word:
		count_word+=1
		if count_word == 3:
			triangles = key.split('_')
			for i in range(3):
				printFriends(triangles[i],triangles[(i+1)%3],triangles[(i+2)%3])
	else:
		previous_word = key
		count_word = 1




