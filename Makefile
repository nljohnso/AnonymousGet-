all: awget ss

awget: awget.c
	c99 -o awget awget.c common.c 

ss: ss.c
	c99 -o ss ss.c common.c -lpthread

clean:
	$(RM) awget ss
