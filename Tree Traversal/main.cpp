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

class ReverseOrderTree
{
public:
    ReverseOrderTree(TreeNode* root)
    {
        m_parents.push(root);
        m_node = root;
    }
    
    bool hasNext()
    {
        return m_node != nullptr || !m_parents.empty();
    }
    
    TreeNode* next()
    {
        if (!hasNext()) return nullptr;

        // 1 roll down left
        if (m_node->left)
        {
            m_node = rollDownLeft(m_node);
            return m_node;
        }

        // 2 left up right shift -> roll down left | right shift if no left child
        m_node = leftUp();
        m_node = rightShift(m_node);
        
        m_node = rollDownLeft(m_node);
        return m_node;
    
        else if (m_node->right)
        {
            m_node = rightShift(m_node);
        }

        //right up if was right up | right shift no rdl

        if (!isDirectionLeftUp(m_node))
        {
            m_node = rightUp(m_node);
        }

        //backtracking:
        //1. parent memo: here: stack of parents




        //2. when come to parent: check direction right or left

    }
  
private:
    std::stack<TreeNode*> m_parents;
    TreeNode* m_node = nullptr;
    
    TreeNode* rollDownLeft(TreeNode* node)
    {
        TreeNode* current = node;
        
        while (current->left)
        {
            m_parents.push(current);
            current = current->left;
        }
        
        return current;
    }
    
    TreeNode* leftUp()
    {
        TreeNode* current = m_parents.top();
        m_parents.pop();
        
        return current;
    }
    
    TreeNode* rightUp(TreeNode* node)
    {
        while (!m_parents.empty())
        {
            TreeNode* parent = m_parents.top();
            m_parents.pop();

            if (parent->right == node)
            {
                node = parent; 
            }
            else
            {
                m_parents.push(parent);
                return parent;
            }
        }

        return nullptr; 
    }
    
    TreeNode* rightShift(TreeNode* node)
    {
        if (node == nullptr) return;

        if (node->right)
        {
            return node->right;
        }

        return nullptr;
    }
    
    bool isDirectionLeftUp(TreeNode* nodeChild)
    {
        if (nodeChild == m_parents.top()->left)
        {
            return true;
        }
        return false;
    }
};

void Traversal(ReverseOrderTree& tree) 
{
  
}

int main()
{
    TreeNode* tree = BuildTree();
    ReverseOrderTree orderTree(tree);

    while (orderTree.hasNext())
    {
        TreeNode* node = orderTree.next();
        std::cout << node->val << " ";
    }

    std::cout << std::endl;

    return 0;
}