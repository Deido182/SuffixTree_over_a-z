#include <iostream>
#include<bits/stdc++.h>
#include <chrono>
using namespace std;

/* Tested with:
 * https://www.spoj.com/problems/SARRAY/
 * 
 * Just set FIRST = 0 (not '0') and use the compiler C++(gcc 8.3)
 */

class SuffixTree {
	
	private:
		static const char FIRST = 'a';
		static const char LAST = 'z';
		static const char END = LAST + 1;
		
		void build() {
			Node *curr = root, *last = NULL;
			int from = 0;
			for(int i = 0; i < text.length(); i ++) {
				int j;
				for(j = from; j <= i; j ++, curr = curr -> suffixLink) {
					// Let's walk down to find the last node for string text[from, i - 1]
					int next = j + (curr -> labelLen);
					while(next < i) {
						int step = curr -> edges[text[next]] -> len;
						if(next + step > i)
							break;
						curr = curr -> nodes[text[next]];
						next += step;
					}
					if(curr -> edges[text[next]] == NULL) { // rule1, so a new leaf
						setLink(curr, last, NULL);
						curr -> edges[text[next]] = newEdge(next, text.length() - 1);
						continue;
					}
					if(text[(curr -> edges[text[next]] -> fi) + (i - next)] == text[i]) { // rule3, so break
						setLink(curr, last, NULL);
						break;
					}
					// rule2, so split the edge and add a new internal node
					Node *oldn = curr -> nodes[text[next]];
					Node *newn = newNode((curr -> labelLen) + (i - next));
					Edge *olde = curr -> edges[text[next]];
					Edge *newe = newEdge((olde -> fi) + (i - next), olde -> li);
					newn -> edges[text[i]] = newEdge(i, text.length() - 1); // new leaf
					newn -> edges[text[(olde -> fi) + (i - next)]] = newe;
					newn -> nodes[text[(olde -> fi) + (i - next)]] = oldn;
					curr -> nodes[text[next]] = newn;
					olde -> li = (olde -> fi) + (i - next) - 1;
					olde -> len = (olde -> li) - (olde -> fi) + 1;
					setLink(newn, last, newn);
				}
				from = j;
			}
		}
		
		class Edge {
			/*
			 * s[fi : li] = label (inclusive)
			 * 
			 * if an edge is a leaf we will set immediately last as text.length() - 1, instead of 
			 * using a global variable. This because at any step the suffix to insert is less long than 
			 * any other path already in the tree.
			 * 
			 * PAY ATTENTION: THIS version IS NOT online. To make it online we should introduce the global variable 
			 * 'end'.
			 */
			
			public:
				int fi, li, len; // firstIn, lastIn
		};
		
		class Node {
			/*
			 * Assuming that the alphabet is just {'\0', ..., 'z'}.
			 * For case with bigger alphabet we could pass to an implementation with AVL Tree or better.
			 */
			
			public:	// let's store just pointers
				Edge *edges[END + 1] = {}; 
				Node *nodes[END] = {}; 
				Node *suffixLink = NULL;
				int labelLen;
		};
		
		void setLink(Node *toLink, Node* &last, Node *newLast) {
			if(last != NULL)
				last -> suffixLink = toLink;
			last = newLast;
		}
		
		Node **nodes;
		int *nextCh;
		
		void push(Node *n, int k, int &nextToPop) {
			nodes[++ nextToPop] = n;
			nextCh[nextToPop] = k;
		}
		
		void pop(Node* &n, int &k, int &nextToPop) {
			n = nodes[nextToPop];
			k = nextCh[nextToPop --];
		}
		
		Node *N;
		int cN;
		Edge *E;
		int cE;
		
		Node *newNode(int ll) {
			N[cN].labelLen = ll;
			return &N[cN ++];
		}
		
		Edge *newEdge(int fi, int li) {
			E[cE].fi = fi;
			E[cE].li = li;
			E[cE].len = li - fi + 1;
			return &E[cE ++];
		}
		
		Node *root;	
	
	public: // Let's store just pointers
		string text;
	
		SuffixTree(string t) {
			text = t + END;
			N = (Node*)calloc(text.length(), sizeof(Node));
			cN = 0;
			E = (Edge*)calloc(text.length() * 2 - 1, sizeof(Node)); // N leaf + edges between internal vertices in a tree
			cE = 0;
			root = newNode(0);
			root -> suffixLink = root;
			nodes = new Node*[text.length()];
			nextCh = new int[text.length()];
			build();
		}
		
		// The cost of "delete" is high
		
		~SuffixTree() {
			free(N);
			free(E);
			delete[] nodes;
			delete[] nextCh;
		}
		
		int *getSuffixArray() { // Iterative DFS
			int *sa = new int[text.length() - 1]; // END did not belong to text
			int c = 0, nextToPop = -1, next;
			Node *curr;
			push(root, FIRST, nextToPop);
			while(nextToPop >= 0) {
				pop(curr, next, nextToPop);
				if(curr -> edges[END] != NULL && next == FIRST && curr != root) // Let's avoid the empty suffix
					sa[c ++] = (curr -> edges[END] -> fi) - (curr -> labelLen);
				for(int i = next; i <= LAST; i ++) {
					if(curr -> edges[i] == NULL) 
						continue;
					if(curr -> edges[i] -> li == text.length() - 1) {
						sa[c ++] = (curr -> edges[i] -> fi) - (curr -> labelLen);
						continue;
					} 
					push(curr, i + 1, nextToPop);
					push(curr -> nodes[i], FIRST, nextToPop);
					break;
				}
			}
			return sa;
		}
};
 
int main(void) {
	/*
	// For the test over SPOJ
	string s;
	cin >> s;
	int *sa = (new SuffixTree(s)) -> getSuffixArray();
	for(int i = 0; i < s.length(); i ++)
		printf("%d\n", sa[i]);
	*/
	return 0;
}
