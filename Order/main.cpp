#include <iostream>
#include <vector>

using namespace std;

struct TreeNode 
{
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

void preOrder(TreeNode* root, vector<int>& result) 
{
    if (root == nullptr) return;

    result.push_back(root->val); 
    preOrder(root->left, result); 
    preOrder(root->right, result); 
}

void inOrder(TreeNode* root, vector<int>& result) 
{
    if (root == nullptr) return;

    inOrder(root->left, result); 
    result.push_back(root->val); 
    inOrder(root->right, result); 
}

void postOrder(TreeNode* root, vector<int>& result) 
{
    if (root == nullptr) return;

    postOrder(root->left, result); 
    postOrder(root->right, result); 
    result.push_back(root->val); 
}

int main() 
{
    // Пример дерева:
    //        1
    //       / \\
    //      2   3
    //     / \\
    //    4   5
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);

    vector<int> preOrderResult, inOrderResult, postOrderResult;

    preOrder(root, preOrderResult);
    inOrder(root, inOrderResult);
    postOrder(root, postOrderResult);

    cout << "Pre-order: ";
    for (int val : preOrderResult) cout << val << " ";
    cout << endl;

    cout << "In-order: ";
    for (int val : inOrderResult) cout << val << " ";
    cout << endl;

    cout << "Post-order: ";
    for (int val : postOrderResult) cout << val << " ";
    cout << endl;

    return 0;
}
