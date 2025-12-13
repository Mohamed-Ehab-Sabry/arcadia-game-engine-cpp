// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable
{
private:
    // TODO: Define your data structures here
    // Hint: You'll need a hash table with double hashing collision resolution

public:
    ConcretePlayerTable()
    {
        // TODO: Initialize your hash table
    }

    void insert(int playerID, string name) override
    {
        // TODO: Implement double hashing insert
        // Remember to handle collisions using h1(key) + i * h2(key)
    }

    string search(int playerID) override
    {
        // TODO: Implement double hashing search
        // Return "" if player not found
        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard
{
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers
    struct Node
    {
        int playerID;
        int score;
        vector<Node *> forward;
        Node(int id, int s, int level)
        {
            this->playerID = id;
            this->score = s;
            this->forward = vector<Node *>(level, nullptr);
        }
    };

    Node *header;
    int max_level;
    int current_level;
    double probability;
    vector<Node *> update;

public:
    ConcreteLeaderboard()
    {
        // TODO: Initialize your skip list
        max_level = 16;
        current_level = 0;
        probability = 0.5;
        header = new Node(-1, INT_MAX, max_level);
        update = vector<Node *>(max_level + 1, nullptr);
    }

    int randomLevel()
    {
        int level = 0;
        while (level < max_level && (rand() % 100) < (probability * 100))
        {
            level++;
        }
        return level;
    }

    void addScore(int playerID, int score) override
    {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
        removePlayer(playerID);

        int level = randomLevel();

        Node *current = header;
        for (int i = level; i >= 0; i--)
        {
            while (current->forward[i] != nullptr && current->forward[i]->score > score)
            {
                current = current->forward[i];
            }
            if (current->forward[i] != nullptr && current->forward[i]->score == score && current->forward[i]->playerID < playerID)
            {
                current = current->forward[i];
            }
            update[i] = current;
        }

        Node *newNode = new Node(playerID, score, level + 1);
        for (int i = 0; i <= level; i++)
        {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }

        if (level > current_level)
        {
            current_level = level;
        }
    }

    void removePlayer(int playerID) override
    {
        // TODO: Implement skip list deletion
        Node *current = header;
        for (int i = max_level; i >= 0; i--)
        {
            while (current->forward[i] != nullptr && current->forward[i]->playerID != playerID)
            {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = update[0]->forward[0];
        if (current == nullptr || current->playerID != playerID)
        {
            cout << "Player not found" << endl;
            return;
        }

        // Deleting the node from every level
        for (int i = 0; i <= max_level; i++)
        {
            if (update[i]->forward[i] != nullptr && update[i]->forward[i]->playerID == playerID)
            {
                update[i]->forward[i] = current->forward[i];
            }
        }

        delete current;
    }

    vector<int> getTopN(int n) override
    {
        // TODO: Return top N player IDs in descending score order
        vector<int> result;
        Node *current = header->forward[0];

        while (current != nullptr && result.size() < n)
        {
            result.push_back(current->playerID);
            current = current->forward[0];
        }

        return result;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree
{
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers
    enum Color{
        red, black
    }; 

    class Node
    {
    public:
        int id;
        int price; 
        Color color;
        bool Dblack; 
        Node *parent;
        Node *left;
        Node *right;

        Node()
        {
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            color = red;
            Dblack = false;
            price = INT_MIN;
            id = INT_MIN;   
        }
        Node(int id, int price)
        {
            this->price = price;
            this->id = id; 
            color = red; 
            Dblack = false; 
            parent = nullptr;
            left = nullptr;
            right = nullptr;
        }
        Node(bool isDoubleBlack)
        {
            Dblack = isDoubleBlack;
            color = black; 
            parent = nullptr;
            left = nullptr;
            right = nullptr;
            price = INT_MIN;
            id = INT_MIN;   
        }
    };

    void leftRotation(Node* node) 
    {
        //check if the node or it's child is null 
        if(node == nullptr || node->right == nullptr)
            return; 
        // saving the right child and it's left subtree
        Node* rightchild = node->right; 
        Node* leftgrandchild = rightchild->left;
        //moving the subtree to it's new parent (the main node)
        node->right = leftgrandchild; 
        if(leftgrandchild != nullptr)
            leftgrandchild->parent = node; 
        //making the rotation and handling the new root of the subtree
        rightchild->left = node; 
        rightchild->parent = node->parent; 
        if(node->parent == nullptr)
            root = rightchild; 
        else if(node == node->parent->right)
            node->parent->right = rightchild; 
        else 
            node->parent->left = rightchild; 
        //changing the parent of the old root 
        node->parent = rightchild;  
    }
    void rightRotation(Node* node)
    {
        //check if the node or it's child is null 
        if(node == nullptr || node->left == nullptr)
            return; 
        // saving the right child and it's right subtree
        Node* leftchild = node->left; 
        Node* rightgrandchild = leftchild->right;
        //moving the subtree to it's new parent (the main node)
        node->left = rightgrandchild; 
        if(rightgrandchild != nullptr)
            rightgrandchild->parent = node; 
        //making the rotation and handling the new root of the subtree
        leftchild->right = node; 
        leftchild->parent = node->parent; 
        if(node->parent == nullptr)
            root = leftchild; 
        else if(node == node->parent->left)
            node->parent->left = leftchild; 
        else 
            node->parent->right = leftchild; 
        //changing the parent of the old root 
        node->parent = leftchild;  
    }

    void fixInsert(Node* node)
    {
        //Base Case 
        if(node == nullptr)
            return; 
        if(node == root)
        {
            node->color = black; 
            return; 
        }
        //no violation 
        if(node->parent->color == black)
            return; 
        Node* parent = node->parent; 
        Node* grandparent = node->parent->parent;
        //parent is the root 
        if(grandparent == nullptr)
        {
            parent->color = black; 
            return; 
        }
        if(parent == grandparent->left)
        {
            Node* uncle = grandparent->right; 
            if(uncle != nullptr && uncle->color == red)
            {
                //case 1 & 2 : uncle is red. just recoloring and call fix on grandfather 
                parent->color = black; 
                uncle->color = black; 
                grandparent->color = red; 
                fixInsert(grandparent); 
            }
            else 
            {
                // recoloring. same result in case 3 and 4
                grandparent->color = red; 
                parent->color = black; 
                if(node == parent->right) // case 3 : the problem is left right. changing it to case 4
                    leftRotation(parent); 
                rightRotation(grandparent); //case 4 : final rotaion 
                if (root) root->color = black;
                return;
            }
        }
        else 
        {
            Node* uncle = grandparent->left; 
            if(uncle != nullptr && uncle->color == red)
            {
                //case 1 & 2 : uncle is red. just recoloring and call fix on grandfather
                parent->color = black; 
                uncle->color = black; 
                grandparent->color = red; 
                fixInsert(grandparent); 
            }
            else 
            {
                // recoloring. same result in case 3 and 4
                grandparent->color = red; 
                parent->color = black; 
                if(node == parent->left) // case 3 : the problem is left right. changing it to case 4
                    rightRotation(parent); 
                leftRotation(grandparent); //case 4 : final rotaion 
                if (root) root->color = black;
                return; 
            }
        }
        if(root) root->color = black; 
    }


    void fixDelete(Node* node)
    {
        if(node == nullptr)
        {
            cout << "CAN'T FIX A NULL NODE" << endl; 
            return; 
        }
        if(node == root)
        {
            node->color = black; 
            node->Dblack = false;
            cleanNil(node); 
            return;  
        }
        if(!node->Dblack)
        {
            cleanNil(node); 
            return;
        }
        if(node->Dblack && node->color == red)
        {
            node->color = black; 
            node->Dblack = false; 
            cleanNil(node); 
            return; 
        }
        if(node->parent->left == node)
        {
            Node* sibling = node->parent->right; 
            if(sibling && sibling->color == red) // case 1 : the sibling is red
            {
                //recolor the new parent and grandparent (the old parent and sibling)
                sibling->color = black; 
                node->parent->color = red; 
                leftRotation(node->parent); //left rotate around the parent
                fixDelete(node); 
                return; 
            }
            else 
            {
                if(sibling && sibling->right && sibling->right->color == red)//case 4 : the far nephew is red
                {
                    sibling->right->color = black; 
                    Color temp = sibling->color; 
                    sibling->color = node->parent->color; 
                    node->parent->color = temp; 
                    leftRotation(node->parent); 
                    node->Dblack = false; 
                    cleanNil(node); 
                    return; 
                }
                else if(sibling && sibling->left && sibling->left->color == red)//case 3 : the near nephew is red
                {
                    //recolor new sibling and far nephew(near nephew before rotation) (changing it into case 4)
                    sibling->color = red; 
                    sibling->left->color = black; 
                    rightRotation(sibling); //right rotate around the sibling 
                    fixDelete(node); //call recursively so it'll solve the violation after it became case 4
                    return; 
                }
                else //case 2 : sibling exists and his childrens are black 
                {
                    node->Dblack = false; 
                    node->parent->Dblack = true;
                    if(sibling) 
                        sibling->color = red; 
                    cleanNil(node); 
                    fixDelete(node->parent); 
                    return; 
                }
            }
        }
        else 
        {
            Node* sibling = node->parent->left; 
            if(sibling && sibling->color == red) // case 1 : the sibling is red
            {
                //recolor the new parent and grandparent (the old parent and sibling)
                sibling->color = black; 
                node->parent->color = red; 
                rightRotation(node->parent); //right rotate around the parent
                fixDelete(node); 
                return; 
            }
            else 
            {
                if(sibling && sibling->left && sibling->left->color == red)//case 4 : the far nephew is red
                {
                    sibling->left->color = black;
                    Color temp = sibling->color; 
                    sibling->color = node->parent->color; 
                    node->parent->color = temp;  
                    rightRotation(node->parent); 
                    node->Dblack = false; 
                    cleanNil(node); 
                    return; 
                }
                else if(sibling && sibling->right && sibling->right->color == red)//case 3 : the near nephew is red
                {
                    //recolor new sibling and far nephew(near nephew before rotation) (changing it into case 4)
                    sibling->color = red; 
                    sibling->right->color = black; 
                    leftRotation(sibling); //left rotate around the sibling 
                    fixDelete(node); //call recursively so it'll solve the violation after it became case 4
                    return; 
                }
                else //case 2 : sibling exists and his childrens are black 
                {
                    node->Dblack = false; 
                    node->parent->Dblack = true;
                    if(sibling) 
                        sibling->color = red; 
                    cleanNil(node); 
                    fixDelete(node->parent); 
                    return; 
                }
            }
        }
        if(root)
        {
            root->color = black; 
            root->Dblack = false; 
        }
        cleanNil(node); 
    }


    void cleanNil(Node* node)
    {
        if(node->id == INT_MIN && !node->Dblack)
        {
            relinkNode(node, nullptr); 
            delete node; 
        }
    }


    Node* predecessor(Node* node)
    {
        while(node && node->right)
            node = node->right; 
        return node; 
    } 

    void relinkNode(Node* oldNode, Node* newNode)
    {
        if(oldNode->parent == nullptr)
            root = newNode; 
        else if (oldNode->parent->right == oldNode)
            oldNode->parent->right = newNode; 
        else 
            oldNode->parent->left = newNode; 
        if(newNode) 
            newNode->parent = oldNode->parent;
    }

    Node* searchElement(int id, Node* node)
    {
        if(node == nullptr)
            return nullptr;
        if(node->id == id)
            return node; 
        Node* leftResult = searchElement(id, node->left);
        if(leftResult != nullptr)
            return leftResult;
        return searchElement(id, node->right);  
    }

    void clear(Node* node)
    {
        if(!node)
            return; 
        clear(node->left); 
        clear(node->right); 
        delete node; 
    }

    Node* root;
    
public:
    ConcreteAuctionTree()
    {
        // TODO: Initialize your Red-Black Tree
        root = nullptr; 
    }

    void insertItem(int itemID, int price) override
    {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
        Node* node = new Node(itemID, price); 
        if(root == nullptr)
        {
            node->color = black; 
            root = node; 
            return; 
        }
        Node* current = root; 
        Node* parent = nullptr; 
        while(current != nullptr)
        {
            parent = current; 
            if (price > current->price) {
                current = current->right; 
            } else {
                current = current->left; 
            }
        }
        node->parent = parent; 
        if(node->price > parent->price)
            parent->right = node; 
        else
            parent->left = node; 
        if(parent->color == red)
            fixInsert(node); 
    }

    void deleteItem(int itemID) override
    {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
        Node* node = searchElement(itemID, root); 
        //if the value doesn't exist in the tree 
        if(node == nullptr)
        {
            cout << "THIS ITEM DOES NOT EXISTS!!!!" << endl; 
            return; 
        }
        //finding the predecessor of the node and save it in newNode, if the node has one child the newNode will be the node itself 
        Node* newNode = node; 
        if(node->left && node->right)
            newNode = predecessor(node->left); 
        //copying the value of the predecessor into the node, now i need to delete the newNode
        node->price = newNode->price; 
        node->id = newNode->id; 
        //if it has one child (it can have at most one child since it's the presuccessor)
        if(newNode->left || newNode->right)
        {
            Node* newChild = newNode->left? newNode->left : newNode->right; //saving the child to link it with the newNode's parent
            relinkNode(newNode, newChild); //deleting the newNode from the tree
            if(newNode->color == black)
            {
                newChild->Dblack = true; 
                fixDelete(newChild); 
            }
            delete newNode; 
        }
        else //if it has no children 
        {
            if(newNode->color == black)//if it's color is black, make a delusional node (a NIL node) that is double balck and link it in the node's place 
            {
                Node* newChild = new Node(true);
                relinkNode(newNode, newChild);  
                delete newNode; 
                fixDelete(newChild); 
            }
            else //if it's color is red (note that it's a leaf), just reassign the parent pointer and delete the node
            {
                relinkNode(newNode, nullptr);  
                delete newNode; 
                return; 
            }
        }
    }

    ~ConcreteAuctionTree()
    {
        clear(root); 
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int> &coins)
{
    // TODO: Implement partition problem using DP
    // Goal: Minimize |sum(subset1) - sum(subset2)|
    // Hint: Use subset sum DP to find closest sum to total/2
    return 0;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>> &items)
{
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

long long InventorySystem::countStringPossibilities(string s)
{
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>> &edges, int source, int dest)
{
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional

    vector<vector<int>> neighbors(n);

    for (auto &road : edges)
    {
        int first_city = road[0];
        int second_city = road[1];

        neighbors[first_city].push_back(second_city);
        neighbors[second_city].push_back(first_city);
    }

    vector<bool> visited(n, 0);
    queue<int> visiting;
    visiting.push(source);
    visited[source] = true;
    int current_city = 0;

    while (!visiting.empty())
    {
        current_city = visiting.front();
        visiting.pop();

        if (dest == current_city)
        {
            return true;
        }

        for (auto &neighbor : neighbors[current_city])
        {
            if (visited[neighbor] == true)
            {
                continue;
            }
            visited[neighbor] = true;
            visiting.push(neighbor);
        }
    }

    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>> &roadData)
{
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>> &roads)
{
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully

    vector<vector<long long>> shortest_path(n, vector<long long>(n, LLONG_MAX));
    for (int i = 0; i < n; i++)
    {
        shortest_path[i][i] = 0;
    }

    for (auto &road : roads)
    {

        shortest_path[road[0]][road[1]] = min((long long)road[2], shortest_path[road[0]][road[1]]);
    }

    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (shortest_path[i][k] == LLONG_MAX || shortest_path[k][j] == LLONG_MAX)
                {
                    continue;
                }

                if (shortest_path[i][j] > shortest_path[i][k] + shortest_path[k][j])
                {
                    shortest_path[i][j] = shortest_path[i][k] + shortest_path[k][j];
                }
            }
        }
    }

    long long sum = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (shortest_path[i][j] == LLONG_MAX)
            {
                continue;
            }
            sum += shortest_path[i][j];
        }
    }

    string sum_string = "";

    if (sum == 0)
    {
        sum_string = "0";
    }

    while (sum > 0)
    {
        if (sum & 1)
        {
            sum_string += "1";
        }
        else
        {
            sum_string += "0";
        }
        sum >>= 1;
    }
    reverse(sum_string.begin(), sum_string.end());

    return sum_string;
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char> &tasks, int n)
{
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C"
{
    PlayerTable *createPlayerTable()
    {
        return new ConcretePlayerTable();
    }

    Leaderboard *createLeaderboard()
    {
        return new ConcreteLeaderboard();
    }

    AuctionTree *createAuctionTree()
    {
        return new ConcreteAuctionTree();
    }
}
