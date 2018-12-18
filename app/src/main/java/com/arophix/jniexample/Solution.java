package com.arophix.jniexample;

import java.util.Arrays;

class Solution {
    public int[] twoSum(final int[] nums, final int target) throws Exception {
        // Step 1: check arguments
        if(nums == null || nums.length < 2) {
            return null;
        }
        
        // Step 2: make a local copy and do sorting.
        int[] tempNums = new int[nums.length];
        System.arraycopy(nums, 0, tempNums, 0, nums.length);
        Arrays.sort(tempNums);
        
        // Step 3: compare target with array boundary acceptable range.
        if (tempNums[0] + tempNums[1] > target) {
            return null;
        }
        if (tempNums[tempNums.length - 1] + tempNums[tempNums.length - 2] < target) {
            return null;
        }
        
        // Step 4: loop.
        for (int i = 0; i < nums.length - 1; i++) {
            for (int j = i + 1; j < nums.length; j++) {
                if (nums[i] + nums[j] == target) {
                    return new int[] {i, j};
                }
            }
        }
        
        return null;
    }
}