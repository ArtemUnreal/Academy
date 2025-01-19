#include <vector>
#include <iostream>

using namespace std;

class SegmentTree 
{
private:
    struct Node 
    {
        int left, right; 
        bool is_special; 
        int last_parity; 
    };

    vector<Node> tree; 
    vector<int> nums;  

    void build(int node, int left, int right) 
    {
        tree[node].left = left;
        tree[node].right = right;

        if (left == right) 
        {
            tree[node].is_special = true;
            tree[node].last_parity = nums[left] % 2;
        } 
        else 
        {
            int mid = (left + right) / 2;
            build(2 * node, left, mid);
            build(2 * node + 1, mid + 1, right);

            bool is_left_special = tree[2 * node].is_special;
            bool is_right_special = tree[2 * node + 1].is_special;
            int left_last_parity = tree[2 * node].last_parity;
            int right_first_parity = nums[mid + 1] % 2;

            tree[node].is_special = is_left_special && is_right_special && (left_last_parity != right_first_parity);
            tree[node].last_parity = tree[2 * node + 1].last_parity;
        }
    }

    bool query(int node, int left, int right) {
        if (tree[node].right < left || tree[node].left > right) 
        {
            return true; 
        }
        if (left <= tree[node].left && tree[node].right <= right) 
        {
            return tree[node].is_special;
        }

        bool is_left_special = query(2 * node, left, right);
        bool is_right_special = query(2 * node + 1, left, right);

        int mid = (tree[node].left + tree[node].right) / 2;
        int left_last_parity = nums[mid] % 2;
        int right_first_parity = nums[mid + 1] % 2;

        return is_left_special && is_right_special && (left_last_parity != right_first_parity);
    }

public:
    SegmentTree(vector<int>& nums) : nums(nums) 
    {
        int n = nums.size();
        tree.resize(4 * n); 
        build(1, 0, n - 1);
    }

    bool isSpecial(int left, int right) 
    {
        return query(1, left, right);
    }
};

class Solution {
public:
    vector<bool> isArraySpecial(vector<int>& nums, vector<vector<int>>& queries) {
        SegmentTree st(nums); 
        vector<bool> answer;

        for (auto& q : queries) 
        {
            int from = q[0], to = q[1];
            answer.push_back(st.isSpecial(from, to)); 
        }

        return answer;
    }
};

int main() 
{
    vector<int> nums1 = {3, 4, 1, 2, 6};
    vector<vector<int>> queries1 = {{0, 4}};
    Solution sol1;

    vector<bool> result1 = sol1.isArraySpecial(nums1, queries1);

    cout << "Example 1: ";

    for (bool res : result1) 
    {
        cout << (res ? "true" : "false") << " ";
    }

    cout << endl;

    vector<int> nums2 = {4, 3, 1, 6};
    vector<vector<int>> queries2 = {{0, 2}, {2, 3}};
    Solution sol2;

    vector<bool> result2 = sol2.isArraySpecial(nums2, queries2);

    cout << "Example 2: ";

    for (bool res : result2) 
    {
        cout << (res ? "true" : "false") << " ";
    }

    cout << endl;

    return 0;
}