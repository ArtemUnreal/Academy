#include <iostream>
#include <stack>
#include <unistd.h>

struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

TreeNode* BuildTree() 
{
    TreeNode* node1 = new TreeNode(1);
    TreeNode* node4 = new TreeNode(4);
    TreeNode* node3 = new TreeNode(3, node1, node4);

    TreeNode* node6 = new TreeNode(6);
    TreeNode* node8 = new TreeNode(8);
    TreeNode* node7 = new TreeNode(7, node6, node8);

    TreeNode* node5 = new TreeNode(5, node3, node7);

    TreeNode* node11 = new TreeNode(11);
    TreeNode* node13 = new TreeNode(13);
    TreeNode* node12 = new TreeNode(12, node11, node13);

    TreeNode* node17 = new TreeNode(17);
    TreeNode* node20 = new TreeNode(20);
    node17->right = node20;
    
    TreeNode* node15 = new TreeNode(15, node12, node17);
    
    TreeNode* root = new TreeNode(10, node5, node15);

    return root;
}

bool HasLeftNext(TreeNode* root)
{
    return root->left != nullptr;
}

bool HasRightNext(TreeNode* root)
{
    return root->right != nullptr;
}

void Traversal(TreeNode* root) {
    if (root == nullptr) return;

    std::stack<TreeNode*> parents;
    TreeNode* current = root;
    TreeNode* lastVisited = nullptr;
    int count = 0;
;
    while (current != nullptr || !parents.empty())
    {
        //1 rld
        while (HasLeftNext(current) && current->left != lastVisited)
        {
            parents.push(current);
            count++;
            current = current->left;
        }

        if (!current->right)
        {
            std::cout << "VAl  " << current->val << " " << std::endl;
            sleep(1);
        }

        // left up right shift ->rld
        current = parents.top();

        if (current->right && current->right != lastVisited)
        {
            current = current->right;
            //rld in left up right shift
            if (current->left)
            {
                while (HasLeftNext(current))
                {
                    parents.push(current);
                    current = current->left;
                }
            }
            else if (current->right) //right shift if no left child
            {
                current = current->right;
                continue;
            }

            lastVisited = current;

            //std::cout << "Last visited: " << lastVisited->val << std::endl;
        }
        else // right up
        {
            //std::cout << "ELse: " <<  current->val << " " << parents.top()->val << " ";
            std::cout << "Val: " << current->val << std::endl; 
            lastVisited = current;
            parents.pop();
            current = parents.top();
            //std::cout << "CURENT " << current->val << std::endl;
        }

        if (current->right == lastVisited) // right up if was right up
        {
            std::cout << parents.top()->val << std::endl;
            parents.pop();
            current = parents.top()->right;
        }


    }
}

int main()
{
    TreeNode* tree = BuildTree();

    Traversal(tree);

    std::cout << std::endl;

    return 0;
}