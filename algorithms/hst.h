
#ifndef BOTTLENECK_HST_H
#define BOTTLENECK_HST_H

#include <vector>
#include <string>

using namespace std;

typedef struct GridNode {
    int         x;
    int         y;

    GridNode();
    GridNode(int, int);
} GridNode;

typedef struct HSTNode {
    int                 index;  // index in sequence
    int                 level;  // level in tree
    HSTNode*            parent;
    vector<HSTNode*>    children;

    HSTNode(int, int);
} HSTNode;

class HSTree {
public:
    HSTNode*                    root;
    vector<vector<HSTNode*> >    map;
    vector<GridNode>            sequence;
    vector<double>              radii;
    int                         len, wid;
    int                         tot_level;

    HSTree();
    bool get_HSTree_btt(int, int);
    void store_hst_to_file(string);
    void get_hst_from_file(string);

private:
    void get_subhst_from_file(fstream&, HSTNode*, int);
    void store_hst_to_file(fstream&, HSTNode*);
};




#endif //BOTTLENECK_HST_H
