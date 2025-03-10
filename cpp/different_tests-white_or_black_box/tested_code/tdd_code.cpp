//======== Copyright (c) 2023, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - graph
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Dalibor Kalina <xkalin16@stud.fit.vutbr.cz>
// $Date:       $2023-03-07
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Martin Dočekal
 * @author Karel Ondřej
 * @author Dalibor Kalina
 *
 * @brief Implementace metod tridy reprezentujici graf.
 */

#include "tdd_code.h"


Graph::Graph(){}

Graph::~Graph(){
    clear();
}

std::vector<Node*> Graph::nodes() {
    return nodes_;
}

std::vector<Edge> Graph::edges() const{
    return edges_;
}

Node* Graph::addNode(size_t nodeId) {
    for (Node* n : nodes_){
        if (n->id == nodeId){
            return nullptr;
        }
    }

    Node* par = (Node*)malloc(sizeof(struct Node));
    if (par == NULL){
        return nullptr;
    }
    par->id = nodeId;
    par->color = 0;
    nodes_.push_back(par);
    return nodes_.back();
}

bool Graph::addEdge(const Edge& edge){
    if (edge.a == edge.b){
        return false;
    }
    if (containsEdge(edge)){
        return false;
    }
    if (getNode(edge.a) == NULL){
        addNode(edge.a);
    }
    if (getNode(edge.b) == NULL){
        addNode(edge.b);
    }

    edges_.push_back(edge);
    
    return true;

}

void Graph::addMultipleEdges(const std::vector<Edge>& edges) {
    for (Edge e : edges){
        addEdge(e);
    }
}

Node* Graph::getNode(size_t nodeId){
    for (Node* n : nodes_){
        if (n->id == nodeId){
            return n;
        }
    }
    return nullptr;
}

bool Graph::containsEdge(const Edge& edge) const{
    for (Edge e : edges_){
        if (e == edge){
            return true;
        }
    }
    return false;
}

void Graph::removeNode(size_t nodeId){
    Node* node = getNode(nodeId);
    if (node == NULL){
        throw std::out_of_range("Node neexistuje");
        return;
    }

    // smazat hrany s timto uzlem
    int pos = 0;
    while (nodeDegree(nodeId) != 0){
        pos = 0;
        for (Edge e : edges_){  
            if (e.a == nodeId || e.b == nodeId){
                edges_.erase(edges_.begin()+pos);
                break;
            }
            pos++;
        }
    }

    pos = 0;
    for (Node* n : nodes_){
        if (n->id == node->id){
            break;
        }
        pos++;
    }
    nodes_.erase(nodes_.begin()+pos);
    free(node);
}

void Graph::removeEdge(const Edge& edge){
    if (!containsEdge(edge)){
        throw std::out_of_range("Edge neexistuje");
        return;
    }
    int pos = 0;
    for (Edge e : edges_){
        if (e == edge){
            break;
        }
        pos++;
    }
    edges_.erase(edges_.begin()+pos);
}

size_t Graph::nodeCount() const{
    int ret = 0;
    for (Node* n : nodes_){
        ret++;
    }
    return ret;
}

size_t Graph::edgeCount() const{
    int ret = 0;
    for (Edge e : edges_){
        ret++;
    }
    return ret;
}

size_t Graph::nodeDegree(size_t nodeId) const{
    bool found = false;
    for (Node* n : nodes_){
        if (n->id == nodeId){
            found = true;
            break;
        }
    }
    if (!found){
        throw std::out_of_range("Edge neexistuje");
        return -1;
    }

    int ret = 0;
    for (Edge e : edges_){
        if (e.a == nodeId || e.b == nodeId){
            ret++;
        }
    }
    return ret;
}

size_t Graph::graphDegree() const{
    size_t max = 0;
    size_t temp;
    for (Node* n : nodes_){
        temp = nodeDegree(n->id);
        if (temp>max){
            max = temp;
        }
    }
    return max;
}

void Graph::coloring(){
    size_t max_color = 0;
    for (Node* n : nodes_){
        for (Edge e : edges_){
            if (e.a == n->id){
                Node* node = getNode(e.b);
                if (node->color > max_color){
                    max_color = node->color;
                }
            }
            else if (e.b == n->id){
                Node* node = getNode(e.a);
                if (node->color > max_color){
                    max_color = node->color;
                }
            }
        }
        n->color = max_color + 1;
        max_color = 0;
    }
}

void Graph::clear() {
    for (Node* n : nodes_){
        free(n);
    }
    nodes_.clear();
    edges_.clear();
}

/*** Konec souboru tdd_code.cpp ***/
