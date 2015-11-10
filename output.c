#include "output.h"
#include <string.h>
#include <stdio.h>

const char indent[] = "  ";

void printtree(const Node *node, const char *pre);
void routingtree(const Node *node, const char identifier[], const char pre[]);

void routingtree(const Node *node, const char id[], const char pre[])
{
	char buf[LLEN], nid[ARRSIZE];
	const int len = strlen(id);
	strcpy(nid, id);
	nid[len+1]='\0';
	strcpy(buf, pre);
	strcat(buf, node->path);
	if ((strcmp(node->device, "") == 0) && (node->subsize == 1)) {
		if (strcmp(node->path, "/") != 0) //根目錄不另加/
			strcat(buf, "/");
		routingtree(node->sub[0], id, buf);
	} else {
		printf("%s[label=\"%s\\n\\[%s\\]\"];\n", id, buf, node->device);
		for (int i = 0; i < node->subsize; i++) {
			nid[len] = 'a' + i;
			routingtree(node->sub[i], nid, "");
			printf("%s -- %s;\n", id, nid);
		}
	}
}

void output_dot(const Node *node)
{
	printf("strict graph mountgraph{\n");
	routingtree(node, "a", "");
	printf("}\n");
}

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
