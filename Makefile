# 
# CMSC257 - Utility Library
# Code for the CMSC257 Projects
#

# Make environment
CC = gcc
CFLAGS = -pthread


# Productions
all : shellex

shellex : shellex.c 
	$(CC) $(CFLAGS) shellex.c csapp.c -o shellex

clean :
	rm -v shellex
	

