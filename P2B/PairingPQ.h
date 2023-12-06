// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as pairing heap
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below),
            // initialize it here.
            explicit Node(const TYPE &val) : elt{ val }, child{ nullptr }, 
            sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to element at that Node's position.
			// There are two versions, getElt() and dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access private data members
            // of this Node class from within the PairingPQ class 
            // (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with 
    //an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{ nullptr }, count{ 0 } {
        // TODO: Implement this function.
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with 
    //an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, 
    COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        while(start != end) {
                push(*start);
                ++start;
            }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare } {
        std::deque<Node*> dq;
            Node * temp = other.root;
            dq.push_back(temp);
            while(!dq.empty()) {
                temp = dq.front();
                if(temp->child != nullptr) {
                    dq.push_back(temp->child);
                }
                if(temp->sibling != nullptr) {
                    dq.push_back(temp->sibling);
                }
                this->push(temp->elt);
                dq.pop_front();
            }
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);

        std::swap(count, temp.count);
        std::swap(root, temp.root);

        return *this;
        // HINT: Use the copy-swap method from the 
        //"Arrays and Containers" lecture.
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        if(count != 0) {
            std::deque<Node*> dq;
            Node * temp = root;
            dq.push_back(temp);
            while(!dq.empty()) {
                temp = dq.front();
                if(temp->child != nullptr) {
                    dq.push_back(temp->child);
                }
                if(temp->sibling != nullptr) {
                    dq.push_back(temp->sibling);
                }
                delete dq.front();
                dq.pop_front();
            }
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing 
    //heap are out of order and
    //              'rebuilds' the pairing heap by fixing the pairing
    // heap invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node*> dq;
        Node * temp = root;
        dq.push_back(temp);
        while(!dq.empty()) {
            temp = dq.front();
            dq.pop_front();
            if(temp->child != nullptr) {
                dq.push_back(temp->child);
            }
            if(temp->sibling != nullptr) {
                dq.push_back(temp->sibling);
            }
            temp->child = nullptr;
            temp->parent = nullptr;
            temp->sibling = nullptr;
            root = meld(root, temp);
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. 
    //This is already done.
    //              You should implement push functionality 
    //entirely in the addNode()
    //              function, and this function calls addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare')
    // element from
    //              the pairing heap.
    // Note: We will not run tests on your code that would require it 
    //to pop an
    // element when the pairing heap is empty. Though you are welcome to 
    //if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        std::deque<Node*> dq;
        Node * temp = root->child;
        // If only a root element exists
        if(temp == nullptr) {
            delete root;
        // Child of root has no siblings, then the child becomes new root
        } else if(temp->sibling == nullptr) {
            delete root;
            root = temp;
        // Child of root has at least one sibling
        } else {
            Node * meldA;
            Node * meldB;
            Node * melded;
            while(temp->sibling != nullptr) {
                dq.push_back(temp);
                temp = temp->sibling;
            }
            dq.push_back(temp);
            while(dq.size() > 1) {
                // Get two nodes
                meldA = dq.front();
                dq.pop_front();
                meldB = dq.front();
                dq.pop_front();
                // Break parent/sibling relationships
                meldA->parent = nullptr;
                meldA->sibling = nullptr;
                meldB->parent = nullptr;
                meldB->sibling = nullptr;
                // Meld and push
                melded = meld(meldA, meldB);
                dq.push_back(melded);
            }
            delete root;
            root = dq.front();
            dq.pop_front();
        }
        count = count - 1;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //   pairing heap.  This should be a reference for speed. It MUST
    //     be const because we cannot allow it to be modified, as that
    //    might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return count;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        if (count == 0) {
            return true;
        }
        return false;
    } // empty()


    // Description: Updates priority of an element in the pairing heap by
    //       replacing the element refered to by the Node with new_value.
    //              Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority,'new_value', must be more extreme
    //      (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        node->elt = new_value;
        // If root, exit
        if(node == root) {
            return;
        }
        Node * temp = node->parent;
        // Leftmost changed
        if(temp->child == node) {
            //sibling
            if(node->sibling) {
                root->child = node->sibling;
                node->sibling->parent = root;
                node->parent = nullptr;
                node->sibling = nullptr;
            //no siblings
            } else {
                node->parent = nullptr;
                root->child = nullptr;
            }
        // Otherwise
        } else {
            // Middle of sibling
            if(node->sibling) {
                temp = temp->child;
                while(temp->sibling != node) {
                    temp = temp->sibling;
                }
                temp->sibling = node->sibling;
                node->parent = nullptr;
                node->sibling = nullptr;
            // End of sibling
            } else {
                temp = temp->child;
                while(temp->sibling != node) {
                    temp = temp->sibling;
                }
                temp->sibling = nullptr;
                node->parent = nullptr;
            }
        }
        root = meld(root, node);
    } // updateElt()


    // Description: Add a new element to the pairing heap. 
    // Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, 
    //you must be sure to
    //       never move or copy/delete that node in the future, 
    //until it is eliminated
    //       by the user calling pop().  Remember this when you 
    //implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE &val) {
        Node * new_node = new Node{val};
        if(count == 0) {
            root = new_node;
            count += 1;
            return root;
        } else {
            root = meld(root, new_node);
            count += 1;
            return new_node;
        }
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions 
    //you require here.
    // TODO: We recommend creating a 'meld' function 
    //(see the Pairing Heap papers).

    // NOTE: For member variables, you are only allowed to add a 
    //"root pointer"
    //       and a "count" of the number of nodes.  
    //Anything else (such as a deque)
    //should be declared inside of member functions as needed.

    Node * root;
    size_t count;
    Node * meld(Node * a, Node * b) {
        if(a == b) {
            return a;
        }
        if(this->compare(a->elt, b->elt)) {
            a->parent = b;
            a->sibling = b->child;
            b->child = a;
            return b;
        } else {
            b->sibling = a->child;
            a->child = b;
            b->parent = a;
            return a;
        }
    }
};


#endif // PAIRINGPQ_H
