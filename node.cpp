/***************************************************************************
 *   Copyright (C) 2006 by BUI Quang Minh, Steffen Klaere, Arndt von Haeseler   *
 *   minh.bui@univie.ac.at   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "node.h"

//#include <sys/time.h>
//#include <time.h>
#include <math.h>

//#define INFINITY 1000000000

/*********************************************
        class Node
 *********************************************/

Node::Node(int aid) {
    id = aid;
    //name = NULL;
    height = -1;
}

Node::Node(int aid, int aname) {
    id = aid;
    char str[20];
    sprintf(str, "%d", aname);
    name = str;
    height = -1;
}

Node::Node(int aid, const char *aname) {
    id = aid;
    if (aname)
        name = aname;
    height = -1;
}

bool Node::isLeaf() {
    return neighbors.size() <= 1;
}

bool Node::isInCherry() {
	if (this->isLeaf()) {
		if (neighbors[0]->node->isCherry()) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Node::isCherry() {
    int num_leaves = 0;
    for (NeighborVec::iterator it = neighbors.begin(); it != neighbors.end(); it++)
        if ((*it)->node->isLeaf()) num_leaves++;
    return (num_leaves > 1);
}

int Node::degree() {
    return neighbors.size();
}

/** calculate the height of the subtree rooted at this node,
        given the dad. Also return the lowestLeaf.
        @param dad the dad of this node
        @return the leaf at the lowest level. Also modify the height, highestNei of this class.
 */
Node *Node::calcHeight(Node *dad) {
    if (isLeaf() && dad != NULL) {
        // if a leaf, but not the root
        height = 0;
        highestNei = NULL;
        return this;
    }
    // scan through all children
    height = -INFINITY;
    Node *lowestLeaf = NULL;
    for (NeighborVec::iterator it = neighbors.begin(); it != neighbors.end(); it++)
        if ((*it)->node != dad) {
            Node *leaf = (*it)->node->calcHeight(this);
            if ((*it)->node->height + (*it)->length > height) {
                height = (*it)->node->height + (*it)->length;
                highestNei = (*it);
                lowestLeaf = leaf;
            }
        }
    return lowestLeaf;
}

int Node::calDist(Node* partner, Node* dad, int curLen) {
    if ( this->isLeaf() && this != partner && dad != NULL )
        return 0;
    if ( this->isLeaf() && dad == NULL ) {
        return this->neighbors[0]->node->calDist(partner, this, 1);
    } else {
        Node* left = NULL;
        Node* right = NULL;
        for (NeighborVec::iterator it = neighbors.begin(); it != neighbors.end(); it++) {
            if ((*it)->node != dad) {
                if (left == NULL)
                    left = (*it)->node;
                else
                    right = (*it)->node;
            }
        }
        curLen++;
//        cout << left->id << endl;
//        cout << right->id << endl;
        int sumLeft = 0;
        int sumRight = 0;
        if ( left->isLeaf() ) {
            if ( left == partner)
            {
                //cout << " I found you baby" << endl;
                return curLen;
            }
        }
        else {
            sumLeft = left->calDist(partner, this, curLen);
        }
        if ( right->isLeaf() ) {
            if ( right == partner) {
                //cout << " I found you baby" << endl;
                return curLen;
            }
        }
        else {
            sumRight = right->calDist(partner, this, curLen);
        }
        return sumRight + sumLeft;
    }

}


/**
        efficient longest path algorithm
 */
double Node::longestPath2(Node* &node1, Node* &node2) {
    // step 1: find the farthest leaf from this node (as a leaf)
    assert(isLeaf());
    node1 = calcHeight();
    // step 2: find the farthest leaf from node1
    node2 = node1->calcHeight();
    return node1->height;
}

Neighbor *Node::findNeighbor(Node *node) {
	int size = neighbors.size();
    for (int i = 0; i < size; i++)
        if (neighbors[i]->node == node) return neighbors[i];
    /*
    for (NeighborVec::iterator it = neighbors.begin(); it != neighbors.end(); it ++)
            if ((*it)->node == node)
                    return (*it);*/
    cout << "ERROR : Could not find neighbors of node " << node->id << endl;
    assert(0);
    return NULL;
}

bool Node::isNeighbor(Node* node) {
    int size = neighbors.size();
    for (int i = 0; i < size; i++)
        if (neighbors[i]->node == node) return true;
    return false;
}

NeighborVec::iterator Node::findNeighborIt(Node *node) {
    for (NeighborVec::iterator it = neighbors.begin(); it != neighbors.end(); it++)
        if ((*it)->node == node)
            return it;
    assert(0);
    return neighbors.end();
}

void Node::addNeighbor(Node *node, double length, int id) {
    neighbors.push_back(new Neighbor(node, length, id));
}

void Node::updateNeighbor(NeighborVec::iterator nei_it, Neighbor *newnei) {
    assert(nei_it != neighbors.end());
    *nei_it = newnei;
}

void Node::updateNeighbor(NeighborVec::iterator nei_it, Neighbor *newnei, double newlen) {
    assert(nei_it != neighbors.end());
    *nei_it = newnei;
    newnei->length = newlen;
}

void Node::updateNeighbor(Node *node, Neighbor *newnei) {
    NeighborVec::iterator nei_it = findNeighborIt(node);
    assert(nei_it != neighbors.end());
    *nei_it = newnei;
}

void Node::updateNeighbor(Node *node, Neighbor *newnei, double newlen) {
    NeighborVec::iterator nei_it = findNeighborIt(node);
    assert(nei_it != neighbors.end());
    *nei_it = newnei;
    newnei->length = newlen;
}

void Node::updateNeighbor(Node* node, Node *newnode, double newlen) {
    for (NeighborVec::iterator it = neighbors.begin(); it != neighbors.end(); it++)
        if ((*it)->node == node) {
            (*it)->node = newnode;
            (*it)->length = newlen;
            break;
        }
}

double Node::updateNeighbor(Node* node, Node *newnode) {
    for (NeighborVec::iterator it = neighbors.begin(); it != neighbors.end(); it++)
        if ((*it)->node == node) {
            (*it)->node = newnode;
            return (*it)->length;
        }
    return -1;
}

void Node::deleteNode() {
    NeighborVec::reverse_iterator it;
    for (it = neighbors.rbegin(); it != neighbors.rend(); it++)
        delete (*it);
    neighbors.clear();
}

Node::~Node() {
    deleteNode();
}

