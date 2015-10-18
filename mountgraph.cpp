#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using std::ifstream;
using std::cout;
using std::endl;

#define LLEN 280
#define ARRSIZE 60

const char indent[] = "  ";

typedef struct node {
	char path[LLEN];
	char device[LLEN];
	struct node **sub;
	int subsize;
} Node;

Node *new_node(const char *path, const char *device);
void add_sub(Node *node, Node *sub);
Node *clone_node(Node *node);
Node *totree(const char * const *paths, int size, const char *device);
Node *get_tree(ifstream &fin);
Node *merge(Node *tree1, Node *tree2); //合併兩路徑相同節點（及其子節點）
void printtree(Node *node, const char *pre);

int main(int argc, char *argv[])
{
	const char filename[] = "/proc/mounts";
	ifstream fin(filename);
	Node *root=NULL, *node=NULL;
	root = get_tree(fin);
	if (root == NULL) {
		return 1;
	}
	while ((node=get_tree(fin)) != NULL) {
		root = merge(root, node);
	}
	fin.close();
	printtree(root, "");
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

void add_sub(Node *node, Node *sub)
{
	node->sub[node->subsize] = (Node *)malloc(sizeof(Node));
	node->sub[node->subsize++] = sub;
}

Node *clone_node(Node *node)
{
	int i;
	Node *n = new_node(node->path, node->device);
	for (i = 0; i < node->subsize; i++) {
		n->sub[i] = clone_node(node->sub[i]);
	}
	n->subsize = node->subsize;
	return n;
}

Node *totree(const char * const *paths, int size, const char *device)
{
	Node *root = new_node(paths[0], "");
	Node *p = root, *q;
	for (int i = 1; i < int(size); i++) {
		q = new_node(paths[i], "");
		add_sub(p, q);
		p = q;
	}
	strcpy(p->device ,device);
	return root;
}

Node *get_tree(ifstream &fin)
{
	Node *root = NULL;
	char line[LLEN];
	if (fin.getline(line, LLEN)) {
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

Node *merge(Node *tree1, Node *tree2)
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
		bool used2[l2];
		for (int i = 0; i < l2; i++)
			used2[i] = false;
		Node *t1 = NULL, *t2 = NULL;
		for (int i = 0; i < l1; i++) {
			bool used1flag = false;
			t1 = tree1->sub[i];
			for (int j = 0; j < l2; j++)
				if (!used2[j]) {
					t2 = tree2->sub[j];
					if (strcmp(t1->path, t2->path) == 0) {
						add_sub(tree, merge(t1, t2));
						used2[j] = true;
						used1flag = true;
					}
				}
			if (!used1flag) {
				add_sub(tree, t1);
			}
		}
		for (int i = 0; i < l2; i++)
			if (!used2[i])
				add_sub(tree, tree2->sub[i]);
	}
	free(tree1);
	free(tree2);
	return tree;
}

void printtree(Node *node, const char *pre)
{
	int count = 0;
	int li = strlen(indent);
	char buf[li+1];
	buf[li] = '\0';
	for (int i = 0; i < int(strlen(pre)/2); i++) {
		strncpy(buf, pre+i*li, li);
		if (strcmp(buf, indent) != 0)
			break;
		count++;
	}
	char next[LLEN] = "";
	for (int i = 0; i < count+1; i++)
		strcat(next, indent);
	if (strcmp(node->device, "") == 0) {
		char cur[LLEN] = "\0";
		strcat(cur, pre); strcat(cur, node->path); strcat(cur, "/");
		if (node->subsize == 1) {
			strcpy(next, cur);
		} else {
			cout << cur << endl;
		}
	} else {
		cout << pre << node->path << " (" << node->device << ")" << endl;
	}
	for (int i = 0; i < int(node->subsize); i++) {
		printtree(node->sub[i], next);
	}
}

