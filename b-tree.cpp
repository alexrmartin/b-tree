#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "ArgumentManager.h"

using namespace std;

string outputString;

class BNode {
private:
    //BNode **children;
    int *data;
    int degree;
    int num;
    bool isLeaf;

public: 
    BNode **children;

    BNode(int d, bool leaf) {
	num = 0;
	degree = d;
	isLeaf = leaf;

	data = new int[2*degree-1];
	children = new BNode *[2*degree];
    }

    int getNum() {
        return num;
    }

    int getData(int idx) {
        return data[idx];
    }

    bool getIsLeaf() {
        return isLeaf;
    }
    
    void notFullInsert(int a) {
	int i = num - 1; 

	if (isLeaf) {
            while (i >= 0 && data[i] > a) { 
                data[i+1] = data[i];
                i--; 
            }

            data[i+1] = a;
            num++;
	}

	else {
            while (i >= 0 && data[i] > a) {
                    i--;
            }

            if (children[i+1]->num == 2*degree - 1) {
                    split(i+1, children[i+1]); 

                    if (data[i+1] < a) {
                        i++;
                    }
            }

            children[i+1]->notFullInsert(a); 
        } 
    } 

    void split(int i, BNode *c1) {
	BNode *c2 = new BNode(c1->degree, c1->isLeaf); 
	c2->num = degree - 1; 

	for (int j = 0; j < degree-1; j++) {
	    c2->data[j] = c1->data[j+degree];
        }

	if (!(c1->isLeaf)) {
	    for (int j = 0; j < degree; j++) {
		c2->children[j] = c1->children[j+degree];
            }
	} 

	c1->num = degree - 1;

	for (int j = num; j >= i+1; j--) {
	    children[j+1] = children[j];
        }

	children[i+1] = c2; 

	for (int j = num-1; j >= i; j--) {
	    data[j+1] = data[j];
        }

	data[i] = c1->data[degree-1]; 
        num++;
    } 

    void setNum(int n) {
        num = n;
    }

    void setData(int da, int idx) {
        data[idx] = da;
    }

    BNode *find(int k) {
	int i = 0; 
	while (i < num && k > data[i]) {
	    i++;
        }

	if (data[i] == k) {
	    return this; 
        }

	if (isLeaf) {
	    return NULL;
        }

	return children[i]->find(k); 
    } 

    void traverse(int currentLevel, int level) {
	int i;

	for (i = 0; i < num; i++) {
            if (!isLeaf) {
                children[i]->traverse(currentLevel+1, level); 
            }

            int levelTemp = level;
            if (degree % 2 != 0) {
                levelTemp = 2*level - 1;
            }
            if (currentLevel == levelTemp || level == -1) {
                outputString += to_string(data[i]) + " ";
                cout << "(" << data[i] << ", " << currentLevel << ") ";
            }
	}

	if (!isLeaf) {
	    children[i]->traverse(currentLevel+1, level);
        }
    }
}; 

class BTree {
private:
    BNode *head;
    int degree;

public: 
    BTree() {
        head = NULL;
        degree = 0;
    }

    void setDegree(int d) {
        degree = d;
    }

    void traverse(int level) {
        if (head != NULL) {
            head->traverse(1, level);
        }
    }

    void insert(int k) {
	if (head == NULL) {
            head = new BNode(degree, true); 
            head->setData(k, 0);
            head->setNum(1);

            return;
	}

        if (head->find(k) != NULL) {
            return;
        }

        if (head->getNum() == 2*degree - 1) {
            BNode *c = new BNode(degree, false); 

            c->children[0] = head; 

            c->split(0, head); 

            int i = 0; 
            if (c->getData(0) < k) {
                i++; 
            }
            c->children[i]->notFullInsert(k); 

            head = c; 
        } 
        else {
            head->notFullInsert(k);
        }
    }
};

void parseFiles(string insertFileName, string commandFileName, string outFileName)  {
    ifstream insertFile(insertFileName);
    ifstream commandFile(commandFileName);
    ofstream outFile(outFileName);

    BTree *bt = new BTree();
    int *insertNums = new int[1000];

    for (int i = 0; i < 1000; i++) {
        insertNums[i] = -1;
    }

    string line;
    int idx = 0;
    while (getline(insertFile, line)) {
        if (line.empty()) {
            continue;
        }

        istringstream is(line);
        int n;
        while(is >> n) {
            insertNums[idx] = n;
            idx++;
        }
    }

    while (getline(commandFile, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.find("=") != -1) {
            int degree = stoi(line.substr(7));
            cout << degree << endl;
            if (degree % 2 == 0) {
                bt->setDegree(degree/2);
                //bt->setDegree(degree);
            }
            else {
                bt->setDegree(degree/2);
                //bt->setDegree(degree);
            }

            for (int i = 0; i < 1000; i++) {
                if (insertNums[i] == -1) {
                    break;
                }

                bt->insert(insertNums[i]);
                //cout << insertNums[i] << endl;
            }
        }

        if (line.compare("Inorder Traversal") == 0) {
            outputString = "";
            bt->traverse(-1);
            outFile << outputString << endl;
        }

        if (line.find("Level") != -1) {
            outputString = "";
            int level = stoi(line.substr(6));
            bt->traverse(level);
            if (outputString.compare("") == 0) {
                outFile << "empty" << endl;
            }
            else {
                outFile << outputString << endl;
            }
        }
    }
}



int main(int argc, char *argv[]) {
    ArgumentManager am(argc, argv);
    string insertFileName = am.get("input");
    string commandFileName = am.get("command");
    string outFileName = am.get("output");

    parseFiles(insertFileName, commandFileName, outFileName);
    return 0;
}
