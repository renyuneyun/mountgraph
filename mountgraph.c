#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include "output.h"

void print_help();
int handle_cmdargs(int argc, char *argv[]);
Node *new_node(const char *path, const char *device);
void delete_node(Node *node);
void add_sub(Node *node, Node *sub);
Node *clone_node(const Node *node);
Node *totree(char * const *paths, int size, const char *device);
Node *get_tree(FILE *fp);
Node *merge(const Node *tree1, const Node *tree2); //合併兩路徑相同節點（及其子節點）

int main(int argc, char *argv[])
{
	if (handle_cmdargs(argc, argv) == 0)
		return 0;
	const char filename[] = "/proc/mounts";
	FILE *fp = fopen(filename, "r");
	Node *root=NULL, *node=NULL, *newroot=NULL;
	root = get_tree(fp);
	if (root == NULL) {
		return 1;
	}
	while ((node=get_tree(fp)) != NULL) {
		newroot = merge(root, node);
		delete_node(root);
		delete_node(node);
		root = newroot;
	}
	fclose(fp);
	output_dot(root);
	delete_node(root);
	return 0;
}

void print_help(char *progname)
{
	printf("Usage: %s\n", progname);
}

int handle_cmdargs(int argc, char *argv[])
{
	if (argc == 1)
		return 1;
	if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
		print_help(argv[0]);
	}
	return 0;
}

Node *new_node(const char *path, const char *device)
{
	const int size = 100;
	Node *n = (Node *) malloc(sizeof(Node));
	strcpy(n->path, path);
	strcpy(n->device, device);
	n->sub = (Node **) malloc(sizeof(Node *)*size);
	n->subsize = 0;
	return n;
}

void delete_node(Node *node)
{
	while (node->subsize > 0) {
		delete_node(node->sub[--node->subsize]);
	}
	free(node->sub);
	free(node);
}

void add_sub(Node *node, Node *sub)
{
	node->sub[node->subsize++] = sub;
}

Node *clone_node(const Node *node)
{
	int i;
	Node *n = new_node(node->path, node->device);
	for (i = 0; i < node->subsize; i++) {
		n->sub[i] = clone_node(node->sub[i]);
	}
	n->subsize = node->subsize;
	return n;
}

Node *totree(char * const *paths, int size, const char *device)
{
	Node *root = new_node(paths[0], "");
	Node *p = root, *q;
	for (int i = 1; i < (int)size; i++) {
		q = new_node(paths[i], "");
		add_sub(p, q);
		p = q;
	}
	strcpy(p->device ,device);
	return root;
}

Node *get_tree(FILE *fp)
{
	Node *root = NULL;
	char line[LLEN];
	if (fgets(line, LLEN, fp)) {
		char buf[strlen(line)+1];
		strcpy(buf, line);
		char *path=NULL, *device=NULL;
		device = strtok(buf, " ");
		path = strtok(NULL, " ");
		char **paths;
		int size = 0;
		paths = (char **) malloc(sizeof(char)*ARRSIZE);
		char *ipath=NULL;
		paths[size] = (char *) malloc(sizeof(char)*LLEN);
		strcpy(paths[size++], "");
		ipath = strtok(path, "/");
		while (ipath != NULL) {
			paths[size] = (char *) malloc(sizeof(char)*LLEN);
			strcpy(paths[size++], ipath);
			ipath = strtok(NULL, "/");
		}
		strcpy(paths[0], "/");
		root = totree(paths, size, device);
		while (size > 0) {
			free(paths[--size]);
		}
		free(paths);
	}
	return root;
}

Node *merge(const Node *tree1, const Node *tree2)
{
	// tree1->path == tree2->path
	Node *tree = NULL;
	if ((strcmp(tree2->device,"")!=0) && (strcmp(tree1->device,tree2->device)!=0)) {
		tree = clone_node(tree2);
	} else if (tree1->subsize == 0) {
		tree = clone_node(tree2);
		strcpy(tree->device, tree1->device);
	} else {
		tree = new_node(tree1->path, tree1->device);
		int l1 = tree1->subsize, l2 = tree2->subsize;
		BOOL used2[l2];
		for (int i = 0; i < l2; i++)
			used2[i] = FALSE;
		Node *t1 = NULL, *t2 = NULL;
		for (int i = 0; i < l1; i++) {
			BOOL used1flag = FALSE;
			t1 = tree1->sub[i];
			for (int j = 0; j < l2; j++)
				if (!used2[j]) {
					t2 = tree2->sub[j];
					if (strcmp(t1->path, t2->path) == 0) {
						add_sub(tree, merge(t1, t2));
						used2[j] = TRUE;
						used1flag = TRUE;
					}
				}
			if (!used1flag) {
				add_sub(tree, clone_node(t1));
			}
		}
		for (int i = 0; i < l2; i++)
			if (!used2[i])
				add_sub(tree, clone_node(tree2->sub[i]));
	}
	return tree;
}

