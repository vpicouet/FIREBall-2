# Makefile for handling contents of camera_examples directory
# 'make' 			builds all examples

all:
	@find ./* -mindepth 1 -depth -path "*/*/[Mm]akefile" -execdir make \; 

cleano:
	find ./* -mindepth 1 -depth -path "*/*/[Mm]akefile" -execdir make clean \; # Doesn't clean executables!
	
clean: cleano
	find ./* -mindepth 1 -perm -u=w -perm -g=w -perm -a=x -print0 | xargs -0 rm -fv # Remove compiled executables.


