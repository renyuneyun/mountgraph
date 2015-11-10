#ifndef DEFS_H
#define DEFS_H

#define LLEN 280
#define ARRSIZE 60
#define BOOL char
#define TRUE 1
#define FALSE 0

typedef struct node {
	char path[LLEN];
	char device[LLEN];
	struct node **sub;
	int subsize;
} Node;

#endif

