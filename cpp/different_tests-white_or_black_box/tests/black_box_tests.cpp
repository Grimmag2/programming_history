//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Dalibor Kalina <xkalin16@stud.fit.vutbr.cz>
// $Date:       $2017-01-04
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Dalibor Kalina
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

using namespace ::testing;
//
// Testovani prazdneho stromu
//

// Vytvoreni prazdneho stromu
class EmptyTree : public Test{
    protected:
        BinaryTree tree; 
};

TEST_F(EmptyTree, InsertNode){
    
    /*
        Postupne pridavani prvku v nasledujicim poradi: 0, 1, -1
        Ocekavany strom:

                            {0,black}
                                |
                {-1,red}-----------------{1,red}
                    |                       |
         {-, black}---{-, black} {-, black}---{-, black}        <--- listy

    */



    // Testovani prazdneho stromu po pridani jednoho prvku

    auto root0 = tree.InsertNode(0);
    auto *node0 = root0.second;
    ASSERT_TRUE(node0 != NULL);
    EXPECT_TRUE(root0.first);

    EXPECT_TRUE(node0->pParent == NULL);
    EXPECT_FALSE(node0->pLeft == NULL);
    EXPECT_FALSE(node0->pRight == NULL);
    EXPECT_EQ(node0->key, 0);
    EXPECT_EQ(node0->color, BinaryTree::BLACK);

    auto *lnode = node0->pLeft;
    ASSERT_TRUE(lnode != NULL);
    EXPECT_TRUE(lnode->pParent == node0);
    EXPECT_TRUE(lnode->pLeft == NULL);
    EXPECT_TRUE(lnode->pRight == NULL);
    EXPECT_EQ(lnode->color, BinaryTree::BLACK);
    EXPECT_EQ(lnode->pParent->key, 0);
    EXPECT_EQ(lnode->pParent->color, BinaryTree::BLACK);

    auto *rnode = node0->pRight;
    ASSERT_TRUE(rnode != NULL);
    EXPECT_TRUE(rnode->pParent == node0);
    EXPECT_TRUE(rnode->pLeft == NULL);
    EXPECT_TRUE(rnode->pRight == NULL);
    EXPECT_EQ(rnode->color, BinaryTree::BLACK);
    EXPECT_EQ(rnode->pParent->key, 0);
    EXPECT_EQ(rnode->pParent->color, BinaryTree::BLACK);

    // Testovani prazdneho stromu po pridani dalsiho prvku

    auto root1 = tree.InsertNode(1);
    auto node1 = root1.second;
    ASSERT_TRUE(node1 != NULL);
    EXPECT_TRUE(root1.first);

    EXPECT_TRUE(node1->pParent == node0);
    EXPECT_FALSE(node1->pLeft == NULL);
    EXPECT_FALSE(node1->pRight == NULL);
    EXPECT_EQ(node1->key, 1);
    EXPECT_EQ(node1->color, BinaryTree::RED);
    EXPECT_EQ(node1->pParent->key, 0);
    EXPECT_EQ(node1->pParent->color, BinaryTree::BLACK);

    lnode = node1->pLeft;
    ASSERT_TRUE(lnode != NULL);
    EXPECT_TRUE(lnode->pParent == node1);
    EXPECT_TRUE(lnode->pLeft == NULL);
    EXPECT_TRUE(lnode->pRight == NULL);
    EXPECT_EQ(lnode->color, BinaryTree::BLACK);
    EXPECT_EQ(lnode->pParent->key, 1);
    EXPECT_EQ(lnode->pParent->color, BinaryTree::RED);

    rnode = node1->pRight;
    ASSERT_TRUE(rnode != NULL);
    EXPECT_TRUE(rnode->pParent == node1);
    EXPECT_TRUE(rnode->pLeft == NULL);
    EXPECT_TRUE(rnode->pRight == NULL);
    EXPECT_EQ(rnode->color, BinaryTree::BLACK);
    EXPECT_EQ(rnode->pParent->key, 1);
    EXPECT_EQ(rnode->pParent->color, BinaryTree::RED);

    ASSERT_TRUE(node0->pRight != NULL);
    EXPECT_EQ(node0->pRight->key, 1);

    EXPECT_EQ(node0->pLeft->color, BinaryTree::BLACK);
    EXPECT_FALSE(node0->pLeft == NULL);
    EXPECT_TRUE(node0->pLeft->pLeft == NULL);
    EXPECT_TRUE(node0->pLeft->pRight == NULL);

    // Vlozeni existujiciho uzlu

    auto rootSame = tree.InsertNode(0);
    auto nodeSame = rootSame.second;
    ASSERT_TRUE(nodeSame != NULL);
    EXPECT_FALSE(rootSame.first);
    EXPECT_EQ(nodeSame->key, 0);

    //Vlozeni zaporneho uzlu

    auto root_1 = tree.InsertNode(-1);
    auto node_1 = root_1.second;
    ASSERT_TRUE(node_1 != NULL);
    EXPECT_TRUE(root_1.first);

    EXPECT_TRUE(node_1->pParent != NULL);
    EXPECT_EQ(node_1->pParent->key, 0);
    EXPECT_FALSE(node_1->pLeft == NULL);
    EXPECT_FALSE(node_1->pRight == NULL);
    EXPECT_EQ(node_1->key, -1);
    EXPECT_EQ(node_1->color, BinaryTree::RED);

}

TEST_F(EmptyTree, DeleteNode){
    EXPECT_FALSE(tree.DeleteNode(0));
    EXPECT_FALSE(tree.DeleteNode(1));
    EXPECT_FALSE(tree.DeleteNode(-1));
}

TEST_F(EmptyTree, FindNode){
    EXPECT_FALSE(tree.FindNode(0));
    EXPECT_FALSE(tree.FindNode(1));
    EXPECT_FALSE(tree.FindNode(-1));
}



//
// Testovani predem naplneneho stromu
//

// Naplneni hodnotami -3 az 2, 10, 5
class NonEmptyTree : public Test{
    void SetUp(){
        for (int i = -3; i<=2; i++){
            tree.InsertNode(i);
        }
        tree.InsertNode(10);
        tree.InsertNode(5);
    }
    protected:
        BinaryTree tree;
};

TEST_F(NonEmptyTree, InsertNode){

    // Vlozeni existujiciho uzlu
    
    auto root0 = tree.InsertNode(2);
    auto *node0 = root0.second;
    ASSERT_TRUE(node0 != NULL);
    EXPECT_FALSE(root0.first);
    
    EXPECT_TRUE(node0->pParent != NULL);
    EXPECT_TRUE(node0->pLeft != NULL);
    EXPECT_TRUE(node0->pRight != NULL);
    EXPECT_EQ(node0->color, BinaryTree::RED);
    EXPECT_EQ(node0->key, 2);

    auto *pnode = node0->pParent;
    ASSERT_TRUE(pnode != NULL);
    EXPECT_TRUE(pnode->pParent == NULL);
    EXPECT_EQ(pnode->color, BinaryTree::BLACK);
    EXPECT_EQ(pnode->key, 0);
    EXPECT_EQ(pnode->pLeft->color, BinaryTree::RED);
    EXPECT_EQ(pnode->pRight->color, BinaryTree::RED);

    auto *rnode = node0->pRight;
    ASSERT_TRUE(rnode != NULL);
    EXPECT_TRUE(rnode->pParent != NULL);
    EXPECT_EQ(rnode->color, BinaryTree::BLACK);
    EXPECT_EQ(rnode->key, 10);
    EXPECT_EQ(rnode->pRight->color, BinaryTree::BLACK);
    EXPECT_EQ(rnode->pLeft->color, BinaryTree::RED);

    auto *lnode = node0->pLeft;
    ASSERT_TRUE(lnode != NULL);
    EXPECT_TRUE(lnode->pParent != NULL);
    EXPECT_EQ(lnode->color, BinaryTree::BLACK);
    EXPECT_EQ(lnode->key, 1);
    EXPECT_EQ(lnode->pRight->color, BinaryTree::BLACK);
    EXPECT_EQ(lnode->pLeft->color, BinaryTree::BLACK);


    // Vlozeni uzlu s hodnotou 7

    auto root8 = tree.InsertNode(7);
    auto node8 = root8.second;
    ASSERT_TRUE(node8 != NULL);
    EXPECT_TRUE(root8.first);

    EXPECT_TRUE(node8->pParent != NULL);
    EXPECT_TRUE(node8->pLeft != NULL);
    EXPECT_TRUE(node8->pRight != NULL);
    EXPECT_EQ(node8->color, BinaryTree::BLACK);
    EXPECT_EQ(node8->key, 7);

    pnode = node8->pParent;
    ASSERT_TRUE(pnode != NULL);
    EXPECT_TRUE(pnode->pParent != NULL);
    EXPECT_EQ(pnode->color, BinaryTree::RED);
    EXPECT_EQ(pnode->key, 2);
    EXPECT_EQ(pnode->pLeft->color, BinaryTree::BLACK);
    EXPECT_EQ(pnode->pRight->color, BinaryTree::BLACK);

    rnode = node8->pRight;
    ASSERT_TRUE(rnode != NULL);
    EXPECT_TRUE(rnode->pParent != NULL);
    EXPECT_EQ(rnode->color, BinaryTree::RED);
    EXPECT_EQ(rnode->key, 10);
    EXPECT_EQ(rnode->pLeft->color, BinaryTree::BLACK);
    EXPECT_EQ(rnode->pRight->color, BinaryTree::BLACK);

    lnode = node8->pLeft;
    ASSERT_TRUE(lnode != NULL);
    EXPECT_TRUE(lnode->pParent != NULL);
    EXPECT_EQ(lnode->color, BinaryTree::RED);
    EXPECT_EQ(lnode->key, 5);
    EXPECT_EQ(lnode->pLeft->color, BinaryTree::BLACK);
    EXPECT_EQ(lnode->pRight->color, BinaryTree::BLACK);
}

TEST_F(NonEmptyTree, DeleteNode){
    EXPECT_TRUE(tree.DeleteNode(10));
    EXPECT_FALSE(tree.DeleteNode(10));

    for (int i = -3; i<=2; i++){
            EXPECT_TRUE(tree.DeleteNode(i));
            EXPECT_FALSE(tree.DeleteNode(i));
    }

    EXPECT_TRUE(tree.DeleteNode(5));
    EXPECT_FALSE(tree.DeleteNode(5));

    EXPECT_FALSE(tree.DeleteNode(-100));
    EXPECT_FALSE(tree.DeleteNode(100));
}

TEST_F(NonEmptyTree, FindNode){
    EXPECT_TRUE(tree.FindNode(10));
    for (int i = -3; i<=2; i++){
            EXPECT_TRUE(tree.FindNode(i));
    }
    EXPECT_TRUE(tree.FindNode(5));

    EXPECT_FALSE(tree.FindNode(-100));
    EXPECT_FALSE(tree.FindNode(100));
}


//
// Testovani Axiomu
//

// Strom pro kontrolu axiomu sestaveny z prvku -5 az 5
class TreeAxioms : public Test{
    void SetUp(){
        for (int i = -5; i<=5; i++){
            tree.InsertNode(i);
        }
    }
    protected:
        BinaryTree tree;
};

// Axiom 1 = vsechny listy musi byt cerne
TEST_F(TreeAxioms, Axiom1){
    std::vector<BinaryTree::Node_t *> leafNodes;
	tree.GetLeafNodes(leafNodes);

    size_t cout = sizeof(leafNodes) / sizeof(leafNodes[0]);

	for (int i = 0; i<cout; i++){
		EXPECT_EQ(leafNodes[i]->color, BinaryTree::BLACK);
	}
}

// Axiom 2 = pokud je uzel cerveny, jeho synove musi byt cerni
TEST_F(TreeAxioms, Axiom2){
    std::vector<BinaryTree::Node_t *> nonLeafNodes;
	tree.GetNonLeafNodes(nonLeafNodes);
    
    size_t cout = sizeof(nonLeafNodes) / sizeof(nonLeafNodes[0]);

    for (int i = 0; i<cout; i++){
        if (nonLeafNodes[i]->color == BinaryTree::RED){
            EXPECT_EQ(nonLeafNodes[i]->pLeft->color, BinaryTree::BLACK);
            EXPECT_EQ(nonLeafNodes[i]->pRight->color, BinaryTree::BLACK);
        }
    }
}

// Axiom 3 = pocet cernych uzlu na ceste z listu do korene je pro vsechny listy stejny
TEST_F(TreeAxioms, Axiom3){
    std::vector<BinaryTree::Node_t *> leafNodes;
    tree.GetLeafNodes(leafNodes);

    size_t cout = sizeof(leafNodes) / sizeof(leafNodes[0]);
    int black_count[cout] = { };

    for (int i = 0; i<cout; i++){       //spocitani cernych uzlu pro jednotlive listy
        auto *node = leafNodes[i];
        node = node->pParent;
        while (node != NULL){
            if (node->color == BinaryTree::BLACK){
                black_count[i]++;
            }
            node = node->pParent;
        }
    }

    bool match = true;

    for (int i = 1; i<cout; i++){       //porovnani poctu cernych uzlu
        if (black_count[i-1] != black_count[i]){
            match = false;
            break;
        }
    }

    EXPECT_TRUE(match);
}

/*** Konec souboru black_box_tests.cpp ***/
