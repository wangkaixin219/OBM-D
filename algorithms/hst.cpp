
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <cmath>
#include "hst.h"

using namespace std;

double get_Euclidean_distance(GridNode node1, GridNode node2) {
    int x_diff = node1.x - node2.x, y_diff = node1.y - node2.y;
    return sqrt(x_diff * x_diff + y_diff * y_diff);
}

GridNode::GridNode() {
    this->x = 0;
    this->y = 0;
}

GridNode::GridNode(int x, int y) {
    this->x = x;
    this->y = y;
}

HSTNode::HSTNode(int i, int l) {
    this->index = i;
    this->level = l;
    this->parent = nullptr;
    this->children = vector<HSTNode*>();
}

HSTree::HSTree() {
    this->root = nullptr;
}

bool HSTree::get_HSTree_btt(int len, int wid) {
    random_device rd;
    uniform_real_distribution<> urd(0, 1);

    double max_dis = sqrt(len * len + wid * wid), rad = urd(rd) / 2.0 + 0.5;
    int power = (int) ceil(log2(max_dis / rad)), size = (len + 1) * (wid + 1);      // HST is [0--power]
    fstream frecord;
    frecord.open("../indexData/record.txt", ios::out);
    this->len = len;
    this->wid = wid;
    this->sequence = vector<GridNode>();
    for (int i = 0; i <= len; i++)
        for (int j = 0; j <= wid; j++)
            this->sequence.push_back(GridNode(i, j));
    shuffle(this->sequence.begin(), this->sequence.end(), rd);
    this->tot_level = power;
    vector<vector<HSTNode*> > pointers = vector<vector<HSTNode*> >(power + 1, vector<HSTNode*>(size, nullptr));
    vector<vector<int> > record = vector<vector<int> >(power + 1, vector<int>());
    for (int i = 0; i < size; i++)
        record[0].push_back(i);
    radii = vector<double>(power + 1);
    radii[0] = rad;
    for (int i = 1; i <= power; i++)
        radii[i] = radii[i - 1] * 2;
    for (int i = 0; i < size; i++)
        pointers[0][i] = new HSTNode(i, 0);
    for (int i = 0; i <= power - 1; i++) {
        vector<bool> checked = vector<bool>(record[i].size(), false);
        cout << record[i].size() << endl;
        for (int j = 0; j < record[i].size(); j++) {
            if (checked[j]) continue;
            vector<int> choosen = vector<int>();
            for (int k = j; k < record[i].size(); k++) {
                if (checked[k]) continue;
                if (get_Euclidean_distance(this->sequence[record[i][j]], this->sequence[record[i][k]]) <= radii[i + 1]) {
                    checked[k] = true;
                    choosen.push_back(record[i][k]);
                }
            }
            cout << "(" << i << ", " << j << ")" << endl;
            if (!choosen.empty()) {
                record[i + 1].push_back(record[i][j]);
                HSTNode *hstnode = new HSTNode(record[i][j], i + 1);
                pointers[i + 1][record[i][j]] = hstnode;
                for (int k = 0; k < choosen.size(); k++) {
                    int index = choosen[k];
                    HSTNode *node = pointers[i][index];
                    hstnode->children.push_back(node);
                    node->parent = hstnode;
                }
                if (i == power - 1)
                    root = hstnode;
            }
        }
        frecord << "level=" << i << ", record=" << record[i].size() << endl;
    }
    map = vector<vector<HSTNode*> >(len + 1, vector<HSTNode*>(wid + 1));
    for (int i = 0; i < size; i++) {
        int seq_i = pointers[0][i]->index;
        map[this->sequence[seq_i].x][this->sequence[seq_i].y] = pointers[0][i];
    }
    return true;
}

void HSTree::store_hst_to_file(string path)
{
    fstream fout(path, ios::out);
    fout << len << " " << wid << " " << tot_level << endl;
    for (int i = 0; i < radii.size(); i++)
        fout << radii[i] << " ";
    fout << endl;
    for (int i = 0; i < sequence.size(); i++) {
        fout << sequence[i].x << " " << sequence[i].y << " ";
    }
    fout << endl;
    store_hst_to_file(fout, root);
    fout.close();
}

void HSTree::store_hst_to_file(fstream& fin, HSTNode* node)
{
    vector<HSTNode*> chil = node->children;
    fin << node->index << " ";
    if (!chil.empty()) {
        fin << chil.size() << " ";
        for (int i = 0; i < chil.size(); i++) {
            store_hst_to_file(fin, chil[i]);
        }
    }
}

void HSTree::get_hst_from_file(string path)
{
    fstream fin(path, ios::in);
    fin >> len >> wid >> tot_level;
    map = vector<vector<HSTNode*> >(len + 1, vector<HSTNode*>(wid + 1));
    radii.clear();
    radii = vector<double>(tot_level + 1);
    for (int i = 0; i <= tot_level; i++)
        fin >> radii[i];

    int seq_size = (len + 1) * (wid + 1);
    sequence.clear();
    sequence = vector<GridNode>(seq_size);
    for (int i = 0; i < sequence.size(); i++) fin >> sequence[i].x >> sequence[i].y;
    int r_index, chil_num;
    fin >> r_index >> chil_num;
    root = new HSTNode(r_index, tot_level);
    for (int i = 0; i < chil_num; i++)
        get_subhst_from_file(fin, root, tot_level - 1);

}

void HSTree::get_subhst_from_file(fstream& fin, HSTNode* node, int level)
{
    int index, chil_num = 0;
    if (level == 0)
        fin >> index;
    else
        fin >> index >> chil_num;
    HSTNode *new_node = new HSTNode(index, level);
    new_node->parent = node;
    if (level == 0) {
        map[sequence[index].x][sequence[index].y] = new_node;
    }
    node->children.push_back(new_node);
    for (int i = 0; i < chil_num; i++) {
        get_subhst_from_file(fin, new_node, level - 1);
    }
}
