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

void findSmallestString(string s, int k, char *ans) {
	int len = s.length();
    ans[len] = '\0';
    if(k == 0) {
    	for(int i = 0; i < len; i ++)
    		ans[i] = s[i];
    	return;
	}
    string r = s;
    reverse(r.begin(), r.end());
    string sr = s + r;
    SuffixTree *st = new SuffixTree(sr);
    int *sa = st -> getSuffixArray();
    int nextPos = 0;
    for(int i = 0; i < (len << 1) && nextPos < len && k > 0; i ++) {
        if(sa[i] >= len) {
    		// segment reversed
    		if(sa[i] - len > len - 1 - nextPos)
    			continue;
    		int end = len - nextPos;
    		for(int j = sa[i] - len; j < end; j ++)
    			ans[nextPos ++] = r[j];
    		k --;
    	} else if(sa[i] == nextPos) {
    		ans[nextPos] = s[nextPos];
    		nextPos ++;
    	}
    }
    if(nextPos != len) {
    	string e1 = s.substr(nextPos, len);
    	string e2 = r.substr(0, len - nextPos);
    	string end = (e1.compare(e2) <= 0 ? e1 : e2);
    	for(int i = 0; i < len - nextPos; i ++)
    		ans[nextPos + i] = end[i];
    }
    delete st;
    delete[] sa;
}
 
int main(void) {
	const int MAX_SIZE = (int)1e6 + 2;
	const int MAX_SIZE_NAME = (int)1e2;;
	int testCases, len;
	long long start, end;
	char s[MAX_SIZE];
	char ans[MAX_SIZE];
	char name[MAX_SIZE_NAME];
	FILE *nameList = fopen("NameList.txt", "r");
	FILE *results = fopen("FastTreeOutput/Results.txt", "w");
	FILE *time = fopen("FastTreeOutput/Time.txt", "w");
	fscanf(nameList, "%d", &testCases);
	while(testCases -- > 0) {
		fscanf(nameList, "%s", name);
		FILE *testCase = fopen(name, "r");
		fscanf(testCase, "%d%s", &len, s);
		start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		findSmallestString(s, len + 1, ans); // len + 1 means "divide s as many times you prefer"
		end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		fclose(testCase);
		fprintf(results, "%s\n", ans);
		fprintf(time, "%s: %d ms\n", name, end - start);
		printf("%s DONE - remaining: %d\n", name, testCases);
	}
	fclose(nameList);
	fclose(results);
	fclose(time);
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
