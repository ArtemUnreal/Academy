#pragma once

#include <string>

using namespace std;

string longestPalindrome(string s) 
    {
        int n = s.length();
        if (n <= 1) return s; 

        int start = 0, max_len = 0;

        auto expandAroundCenter = [&](int left, int right) 
        {
            while (left >= 0 && right < n && s[left] == s[right]) 
            {
                --left;
                ++right;
            }

            int len = right - left - 1;
            if (len > max_len) 
            {
                max_len = len;
                start = left + 1;
            }
        };

        for (int i = 0; i < n; ++i) 
        {
            expandAroundCenter(i, i);
            expandAroundCenter(i, i + 1);
        }

        return s.substr(start, max_len);
    }