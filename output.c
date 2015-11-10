#include "output.h"
#include <string.h>
#include <stdio.h>

const char indent[] = "  ";

void printtree(const Node *node, const char *pre);

void output_ascii(const Node *node)
{
	printtree(node, "");
}

void printtree(const Node *node, const char *pre)
{
	int count = 0;
	int li = strlen(indent);
	char buf[li+1];
	buf[li] = '\0';
	for (int i = 0; i < (int)(strlen(pre)/2); i++) {
		strncpy(buf, pre+i*li, li);
		if (strcmp(buf, indent) != 0)
			break;
		count++;
	}
	char next[LLEN] = "";
	for (int i = 0; i < count+1; i++)
		strcat(next, indent);
	if (strcmp(node->device, "") == 0) { //無設備=非掛載點
		char cur[LLEN] = "\0";
		strcat(cur, pre); strcat(cur, node->path);
		if (strcmp(node->path, "/") != 0) //根目錄不另加/
			strcat(cur, "/");
		if (node->subsize == 1) {
			strcpy(next, cur);
		} else {
			printf("%s\n", cur);
		}
	} else {
		printf("%s%s (%s)\n", pre, node->path, node->device);
	}
	for (int i = 0; i < (int)node->subsize; i++) {
		printtree(node->sub[i], next);
	}
}
