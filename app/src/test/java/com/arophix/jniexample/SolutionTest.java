package com.arophix.jniexample;

import org.junit.Test;

import java.util.Arrays;

import static org.junit.Assert.*;

public class SolutionTest {
    
    Solution solution = new Solution();
    
    @Test
    public void twoSum() {
        
        twoSumary(new int[]{2,7,11,15}, 9);
        twoSumary(new int[]{2,7,11,15}, 17);
        twoSumary(new int[]{2,7,11,15}, 2);
        twoSumary(new int[]{2,7,11,15}, 5);
    }
    
    private void twoSumary(int[] testArr, int target) {
        int[] resultArr = null;
        
        try {
            resultArr = solution.twoSum(testArr, target);
        } catch (Exception e) {
            e.printStackTrace();
        }
    
        System.out.println("resultArr: " + Arrays.toString(resultArr));
    }
}