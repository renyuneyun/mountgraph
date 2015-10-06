#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>

using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::stringstream;
using std::endl;

#define MAX(a,b) a>b ? a : b
#define BUFSIZE 80

string indent = "  ";

typedef struct node {
	string path;
	string device;
	vector<struct node *> sub;
} Node;

void printtree(Node *node, string pre);
Node *clone_node(Node *node);
Node *totree(vector<string> paths, string device);
Node *get_tree(ifstream &fin);
Node *merge(Node *tree1, Node *tree2);

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
		if (node->sub.size() == 1) {
			next = cur;
		} else {
			cout << cur << endl;
		}
	} else {
		cout << pre << node->path << "/ (" << node->device << ")" << endl;
	}
	for (int i = 0; i < int(node->sub.size()); i++) {
		printtree(node->sub[i], next);
	}
}

Node *clone_node(Node *node)
{
	Node *n = new Node;
	n->path = node->path;
	n->device = node->device;
	n->sub = vector<Node *>(node->sub);
	return n;
}

Node *totree(vector<string> paths, string device)
{
	Node *root = new Node();
	Node *p = root, *q;
	root->path = paths[0];
	for (int i = 1; i < int(paths.size()); i++) {
		q = new Node();
		q->path = paths[i];
		p->sub.push_back(q);
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
		vector<string> paths;
		char *ipath=NULL;
		paths.push_back("");
		ipath = strtok(path, "/");
		while (ipath != NULL) {
			paths.push_back(ipath);
			ipath = strtok(NULL, "/");
		}
		root = totree(paths, device);
	}
	return root;
}

Node *merge(Node *tree1, Node *tree2)
{
	Node *tree = NULL;
	if ((tree2->device != "") && (tree1->device != tree2->device)) {
		tree = clone_node(tree2);
	} else if (tree1->sub.size() == 0) {
		tree = clone_node(tree1);
		tree->sub = vector<Node *>(tree2->sub);
	} else {
		tree = new Node;
		tree->path = tree1->path;
		tree->device = tree1->device;
		int l1 = tree1->sub.size(), l2 = tree2->sub.size();
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
						tree->sub.push_back(merge(t1, t2));
						used2[j] = true;
						used1flag = true;
					}
				}
			if (!used1flag) {
				tree->sub.push_back(t1);
			}
		}
		for (int i = 0; i < l2; i++)
			if (!used2[i])
				tree->sub.push_back(tree2->sub[i]);
	}
	delete tree1;
	delete tree2;
	return tree;
}
