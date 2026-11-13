
#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO

protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
};

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* right = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = right->getLeft();

    right->setParent(parent);

    if (parent == NULL) {
        this->root_ = right;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(right);
    }
    else {
        parent->setRight(right);
    }

    node->setRight(middle);

    if (middle != NULL) {
        middle->setParent(node);
    }

    right->setLeft(node);
    node->setParent(right);

    int oldNodeBalance = node->getBalance();
    int oldRightBalance = right->getBalance();

    int newNodeBalance = oldNodeBalance - 1 - std::max(oldRightBalance, 0);
    int newRightBalance = oldRightBalance - 1 + std::min(newNodeBalance, 0);

    node->setBalance(static_cast<int8_t>(newNodeBalance));
    right->setBalance(static_cast<int8_t>(newRightBalance));

    return right;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* left = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* middle = left->getRight();

    left->setParent(parent);

    if (parent == NULL) {
        this->root_ = left;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(left);
    }
    else {
        parent->setRight(left);
    }

    node->setLeft(middle);

    if (middle != NULL) {
        middle->setParent(node);
    }

    left->setRight(node);
    node->setParent(left);

    int oldNodeBalance = node->getBalance();
    int oldLeftBalance = left->getBalance();

    int newNodeBalance = oldNodeBalance + 1 - std::min(oldLeftBalance, 0);
    int newLeftBalance = oldLeftBalance + 1 + std::max(newNodeBalance, 0);

    node->setBalance(static_cast<int8_t>(newNodeBalance));
    left->setBalance(static_cast<int8_t>(newLeftBalance));

    return left;
}

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(
            new_item.first, new_item.second, NULL
        );
        return;
    }

    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->root_);

    AVLNode<Key, Value>* parent = NULL;

    while (current != NULL) {
        parent = current;

        if (new_item.first == current->getKey()) {
            current->setValue(new_item.second);
            return;
        }

        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        }
        else {
            current = current->getRight();
        }
    }

    AVLNode<Key, Value>* added = new AVLNode<Key, Value>(
        new_item.first, new_item.second, parent
    );

    if (new_item.first < parent->getKey()) {
        parent->setLeft(added);
    }
    else {
        parent->setRight(added);
    }

    AVLNode<Key, Value>* child = added;

    while (parent != NULL) {
        if (parent->getLeft() == child) {
            parent->updateBalance(-1);
        }
        else {
            parent->updateBalance(1);
        }

        if (parent->getBalance() == 0) {
            break;
        }

        if (parent->getBalance() == -2) {
            if (parent->getLeft()->getBalance() > 0) {
                rotateLeft(parent->getLeft());
            }

            rotateRight(parent);
            break;
        }

        if (parent->getBalance() == 2) {
            if (parent->getRight()->getBalance() < 0) {
                rotateRight(parent->getRight());
            }

            rotateLeft(parent);
            break;
        }

        child = parent;
        parent = parent->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* target =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (target == NULL) {
        return;
    }

    if (target->getLeft() != NULL && target->getRight() != NULL) {
        AVLNode<Key, Value>* pred = target->getLeft();

        while (pred->getRight() != NULL) {
            pred = pred->getRight();
        }

        nodeSwap(target, pred);
    }

    AVLNode<Key, Value>* child =
        (target->getLeft() != NULL)
        ? target->getLeft()
        : target->getRight();

    AVLNode<Key, Value>* parent = target->getParent();

    bool removedFromLeft =
        (parent != NULL && parent->getLeft() == target);

    if (child != NULL) {
        child->setParent(parent);
    }

    if (parent == NULL) {
        this->root_ = child;
    }
    else if (removedFromLeft) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete target;

    while (parent != NULL) {
        parent->updateBalance(removedFromLeft ? 1 : -1);

        AVLNode<Key, Value>* subtreeRoot = parent;

        if (parent->getBalance() == -2) {
            if (parent->getLeft()->getBalance() > 0) {
                rotateLeft(parent->getLeft());
            }

            subtreeRoot = rotateRight(parent);
        }
        else if (parent->getBalance() == 2) {
            if (parent->getRight()->getBalance() < 0) {
                rotateRight(parent->getRight());
            }

            subtreeRoot = rotateLeft(parent);
        }

        if (subtreeRoot->getBalance() != 0) {
            break;
        }

        parent = subtreeRoot->getParent();

        if (parent != NULL) {
            removedFromLeft = (parent->getLeft() == subtreeRoot);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
