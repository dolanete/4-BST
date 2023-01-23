#pragma once

#include <functional> // std::less
#include <iostream>
#include <queue> // std::queue
#include <utility> // std::pair

template <typename K, typename V, typename Comparator = std::less<K>>
class BinarySearchTree
{
  public:
    using key_type        = K;
    using value_type      = V;
    using key_compare     = Comparator;
    using pair            = std::pair<key_type, value_type>;
    using pointer         = pair*;
    using const_pointer   = const pair*;
    using reference       = pair&;
    using const_reference = const pair&;
    using difference_type = ptrdiff_t;
    using size_type       = size_t;

  private:
    struct BinaryNode
    {
        pair element;
        BinaryNode *left;
        BinaryNode *right;

        BinaryNode( const_reference theElement, BinaryNode *lt, BinaryNode *rt )
          : element{ theElement }, left{ lt }, right{ rt } { }
        
        BinaryNode( pair && theElement, BinaryNode *lt, BinaryNode *rt )
          : element{ std::move( theElement ) }, left{ lt }, right{ rt } { }
    };

    using node           = BinaryNode;
    using node_ptr       = node*;
    using const_node_ptr = const node*;

    node_ptr _root;
    size_type _size;
    key_compare comp;

  public:
    BinarySearchTree(): _root(0), _size(0) {
        _root = nullptr;
    }
    BinarySearchTree( const BinarySearchTree & rhs ): _root(0), _size(0) {
        _root = clone(rhs._root);
        _size = rhs._size;
    }
    BinarySearchTree( BinarySearchTree && rhs ) {
        _root = rhs._root;
        _size = rhs._size; //set sizes

        rhs._root = nullptr;
        rhs._size = 0;
    }
    ~BinarySearchTree() {
        clear();
    }

    const_reference min() const { return min( _root )->element; }
    const_reference max() const { return max( _root )->element; }
    const_reference root() const {
        return _root->element;
    }
    bool contains( const key_type & x ) const { return contains( x, _root ); }
    value_type & find( const key_type & key ) { return find( key, _root )->element.second; }
    const value_type & find( const key_type & key ) const { return find( key, _root )->element.second; }
    bool empty() const {
        if (_size == 0) {
            return true;
        }
        return false;
    }
    size_type size() const {
        return _size;
    }

    void clear() {
        clear( _root );
        _size = 0;
    }
    void insert( const_reference x ) { insert( x, _root ); }
    void insert( pair && x ) { insert( std::move( x ), _root ); }
    void erase( const key_type & x ) { erase(x, _root); }

    BinarySearchTree & operator=( const BinarySearchTree & rhs ) {
        
        if (this != &rhs) {
            clear();
            _root = clone(rhs._root);
            _size = rhs._size;
        }

        return *this;
    }
    BinarySearchTree & operator=( BinarySearchTree && rhs ) {

        if (this != &rhs) {
            clear();
            _root = rhs._root;
            _size = rhs._size; //set sizes

            rhs._root = nullptr;
            rhs._size = 0;
        }
        return *this;
        
    }

  private:
    void insert( const_reference x, node_ptr & t ) {
        if (t == nullptr) { //base case/ This will create the node when the empty position is found
            _size++;
            node_ptr temp = new node(x, nullptr, nullptr);
            t = temp;
        }
            if (x.first == t->element.first) { // If the element that was going to be inserted is found, do nothing basically
                t->element=x;
                return;
            } else if (comp(x.first, t->element.first)) { // If the value is less than the current node
                insert(x, t->left); // Recur to the left side node
            } else if (comp(t->element.first, x.first)) { // If the value is less than the current node
                insert(x, t->right); // Recur to the right side node
            }
        
    }
    void insert( pair && x, node_ptr & t ) {
        if (t == nullptr) { //base case
            _size++;
            node_ptr temp = new node(std::move(x), nullptr, nullptr);
            t = temp;
        } else if (x.first == t->element.first) {
            t->element=std::move(x);
            return;
        } else if (comp(x.first, t->element.first)) {
            insert(std::move(x), t->left);
        } else if (comp(t->element.first, x.first)) {
            insert(std::move(x), t->right);
        }
    }

    void erase( const key_type & x, node_ptr & t ) {
    
        
        if (t == nullptr) { //base / This happens when the value that should have been erase doesn't exist
            return;
        } else if (comp(x, t->element.first)) { //Iterate to find node to want to be deleted
                erase(x, t->left);
        } else if (comp(t->element.first, x)) {
                erase(x, t->right);
        } else {
            // When the element is found, check the three removal cases
            if (t->left == nullptr && t->right == nullptr) { // No children: If there are no children, you can just delete the node
                _size--; // Decrease size
                delete t; // delete the current root node
                t = nullptr; // set the deleted node to a nullptr
            } else if (t->left == nullptr) { //When there is only one child on the right side, you can balance by switching the parent and the child then delete.
                /* t->element = t->right->element; // One child on right side. Replace the root node with the right child
                erase(x, t->right); */
                _size--;
                node_ptr temp = t;
                t = t->right;
                delete temp;

            } else if (t->right == nullptr) { // oNe child on left side. Preplace the root node with the left child
                /* t->element = t->left->element;
                erase(x, t->left); */
                _size--;
                node_ptr temp = t;
                t = t->left;
                delete temp;

            } else { // If there is two children: When there are two children, swap the element with the smallest element on the right tree and recur again to delete the last element
                t->element = min(t->right)->element;
                erase(min(t->right)->element.first, t->right);
            }

        }

    }

    const_node_ptr min( const_node_ptr t ) const { //Recur all the way to the left side of the BST
        if (t->left == nullptr) {
            return t;
        }
        min(t->left);
    }
    const_node_ptr max( const_node_ptr t ) const { //Recur all the way to the right side of the BST
        if (t->right == nullptr) {
            return t;
        }
        max(t->right);
    }

    bool contains( const key_type & x, const_node_ptr t ) const {
        if (find(x, t) == nullptr) {
            return false;
        }
        return true;
    }
    node_ptr find( const key_type & key, node_ptr t ) {
        if (t == nullptr) {
            return nullptr;
        }
        if(comp(key, t->element.first)) {
            return find(key, t->left);
        } else if(comp(t->element.first, key)) {
            return find(key, t->right);
        }
        return t;
    }
    const_node_ptr find( const key_type & key, const_node_ptr t ) const {
        if (t == nullptr) {
            return nullptr;
        }
        if(comp(key, t->element.first)) {
            return find(key, t->left);
        } else if(comp(t->element.first, key)) {
            return find(key, t->right);
        }
        return t;
    }

    void clear( node_ptr & t ) {
        if (t != nullptr) {
            clear(t->left);
            clear(t->right);
            delete t;
            t = nullptr;
        }
        
    }
    
    node_ptr clone ( const_node_ptr t ) const {
        if (t == nullptr) {
            return nullptr;
        }
        node_ptr temp = new node(t->element, clone(t->left), clone(t->right));
        

        return temp;
    }

  public:
    template <typename KK, typename VV, typename CC>
    friend void printLevelByLevel( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out );

    template <typename KK, typename VV, typename CC>
    friend std::ostream& printNode(std::ostream& o, const typename BinarySearchTree<KK, VV, CC>::node& bn);

    template <typename KK, typename VV, typename CC>
    friend void printTree( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out );

    template <typename KK, typename VV, typename CC>
    friend void printTree(typename BinarySearchTree<KK, VV, CC>::const_node_ptr t, std::ostream & out, unsigned depth );

    template <typename KK, typename VV, typename CC>
    friend void vizTree(
        typename BinarySearchTree<KK, VV, CC>::const_node_ptr node, 
        std::ostream & out,
        typename BinarySearchTree<KK, VV, CC>::const_node_ptr prev
    );

    template <typename KK, typename VV, typename CC>
    friend void vizTree(
        const BinarySearchTree<KK, VV, CC> & bst, 
        std::ostream & out
    );
};

template <typename KK, typename VV, typename CC>
std::ostream& printNode(std::ostream & o, const typename BinarySearchTree<KK, VV, CC>::node & bn) {
    return o << '(' << bn.element.first << ", " << bn.element.second << ')';
}

template <typename KK, typename VV, typename CC>
void printLevelByLevel( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out = std::cout ) {
    using node = typename BinarySearchTree<KK, VV, CC>::node;
    using node_ptr = typename BinarySearchTree<KK, VV, CC>::node_ptr;
    using const_node_ptr = typename BinarySearchTree<KK, VV, CC>::const_node_ptr;
    
    bool hasNode = true; //Checks until a line has only null
    int levelCount = 1;
    int secondCount = 0;
    int nullCounter = 0;
    if (bst._root == nullptr) { //empty BST
        return;
    }
    std::queue<node_ptr> q;
    q.push(bst._root);
    while(nullCounter != levelCount) {
        hasNode = false; //Sets the check to false. If sees a node turns to true to activate the next loop
        nullCounter = 0;
        for (int i = 0; i < levelCount; i++) {
            node_ptr temp = q.front(); //get value of node

            if (temp == nullptr) {
                out << "null ";
                q.push(nullptr);
                q.push(nullptr);
                nullCounter += 2;
            } else {
                out << "(" << temp->element.first << ", " << temp->element.second << ") ";
                if (temp->left == nullptr) { //If there is a non-nullptr then continue the loop
                    nullCounter++;
                } 
                if (temp->right == nullptr) { //If there is a non-nullptr then continue the loop
                    nullCounter++; 
                }
                q.push(temp->left);
                q.push(temp->right);
            } 

            
            


            q.pop(); //pop the node
           
        }
        out << std::endl; //When a line ends indent
        
        levelCount *= 2; // Doubles the level count
        

        
    }
}

template <typename KK, typename VV, typename CC>
void printTree( const BinarySearchTree<KK, VV, CC> & bst, std::ostream & out = std::cout ) { printTree<KK, VV, CC>(bst._root, out ); }

template <typename KK, typename VV, typename CC>
void printTree(typename BinarySearchTree<KK, VV, CC>::const_node_ptr t, std::ostream & out, unsigned depth = 0 ) {
    if (t != nullptr) {
        printTree<KK, VV, CC>(t->right, out, depth + 1);
        for (unsigned i = 0; i < depth; ++i)
            out << '\t';
        printNode<KK, VV, CC>(out, *t) << '\n';
        printTree<KK, VV, CC>(t->left, out, depth + 1);
    }
}

template <typename KK, typename VV, typename CC>
void vizTree(
    typename BinarySearchTree<KK, VV, CC>::const_node_ptr node, 
    std::ostream & out,
    typename BinarySearchTree<KK, VV, CC>::const_node_ptr prev = nullptr
) {
    if(node) {
        std::hash<KK> khash{};

        out << "\t" "node_" << (uint32_t) khash(node->element.first)
            << "[label=\"" << node->element.first 
            << " [" << node->element.second << "]\"];" << std::endl;
        
        if(prev)
            out << "\tnode_" << (uint32_t) khash(prev->element.first) <<  " -> ";
        else
            out << "\t";
        
        out << "node_" << (uint32_t) khash(node->element.first) << ";" << std::endl;
    
        vizTree<KK, VV, CC>(node->left, out, node);
        vizTree<KK, VV, CC>(node->right, out, node);
    }
}

template <typename KK, typename VV, typename CC>
void vizTree(
    const BinarySearchTree<KK, VV, CC> & bst, 
    std::ostream & out = std::cout
) {
    out << "digraph Tree {" << std::endl;
    vizTree<KK, VV, CC>(bst._root, out);
    out << "}" << std::endl;
}
