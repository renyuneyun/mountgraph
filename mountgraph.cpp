#include <iostream>
#include <fstream>
#include <cstring>

using std::ifstream;
using std::string;
using std::cout;
using std::endl;

#define MAX(a,b) a>b ? a : b
#define BUFSIZE 80

string indent = "  ";

typedef struct node {
	string path;
	string device;
	struct node **sub;
	int subsize;
} Node;

Node *new_node(string path, string device);
void add_sub(Node *node, Node *sub);
Node *clone_node(Node *node);
Node *totree(string paths[], int size, string device);
Node *get_tree(ifstream &fin);
Node *merge(Node *tree1, Node *tree2); //合併兩路徑相同節點（及其子節點）
void printtree(Node *node, string pre);

int main(int argc, char *argv[])
{
	string filename = "/proc/mounts";
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

Node *new_node(string path, string device)
{
	const int size = 100;
	Node *n = new Node;
	n->path = path;
	n->device = device;
	n->sub = new Node*[size];
	n->subsize = 0;
	return n;
}

void add_sub(Node *node, Node *sub)
{
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

Node *totree(string paths[], int size, string device)
{
	Node *root = new_node(paths[0], "");
	Node *p = root, *q;
	for (int i = 1; i < int(size); i++) {
		q = new_node(paths[i], "");
		add_sub(p, q);
		p = q;
	}
	p->device = device;
	return root;
}

Node *get_tree(ifstream &fin)
{
	Node *root = NULL;
	string line;
	if (getline(fin, line)) {
		int length = int(line.length());
		char buf[MAX(length, BUFSIZE)];
		strcpy(buf, line.c_str());
		char *path=NULL, *device=NULL;
		device = strtok(buf, " ");
		path = strtok(NULL, " ");
		string paths[100];
		int size = 0;
		char *ipath=NULL;
		paths[size++] = "";
		ipath = strtok(path, "/");
		while (ipath != NULL) {
			paths[size++] = ipath;
			ipath = strtok(NULL, "/");
		}
		paths[0] = "/";
		root = totree(paths, size, device);
	}
	return root;
}

Node *merge(Node *tree1, Node *tree2)
{
	// tree1->path == tree2->path
	Node *tree = NULL;
	if ((tree2->device != "") && (tree1->device != tree2->device)) {
		tree = clone_node(tree2);
	} else if (tree1->subsize == 0) {
		tree = clone_node(tree2);
		tree->device = tree1->device;
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
					if (t1->path == t2->path) {
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
	delete tree1;
	delete tree2;
	return tree;
}

void printtree(Node *node, string pre)
{
	int count = 0;
	int li = indent.size();
	for (int i = 0; i < int(pre.size()/2); i++) {
		if (pre.substr(i*li, li) != indent)
			break;
		count++;
	}
	string next = "";
	for (int i = 0; i < count+1; i++)
		next += indent;
	if (node->device == "") {
		string cur = pre + node->path + "/";
		if (node->subsize == 1) {
			next = cur;
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

