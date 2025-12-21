// Meaningful variable & function names
// No magic indexing (NQ[0])
// Business logic separated from I/O
// Easy to test CalculateSubarrayMean()
// Cleaner and interview-ready

using System;

class SubarrayMeanCalculator
{
    static void Main(string[] args)
    {
        int[] inputMeta = ReadIntegers();
        int numberOfElements = inputMeta[0];
        int numberOfQueries = inputMeta[1];

        long[] arrayValues = ReadLongArray();
        long[] prefixSum = BuildPrefixSum(arrayValues);

        ProcessQueries(numberOfQueries, prefixSum);
    }


    static int[] ReadIntegers()
    {
        return Array.ConvertAll(Console.ReadLine().Split(' '), int.Parse);
    }

    static long[] ReadLongArray()
    {
        return Array.ConvertAll(Console.ReadLine().Split(' '), long.Parse);
    }

    static long[] BuildPrefixSum(long[] values)
    {
        long[] prefixSum = new long[values.Length + 1];
        prefixSum[0] = 0;

        for (int i = 1; i <= values.Length; i++)
        {
            prefixSum[i] = prefixSum[i - 1] + values[i - 1];
        }

        return prefixSum;
    }

    static void ProcessQueries(int queryCount, long[] prefixSum)
    {
        for (int i = 0; i < queryCount; i++)
        {
            int[] range = ReadIntegers();
            int leftIndex = range[0];
            int rightIndex = range[1];

            long mean = CalculateSubarrayMean(prefixSum, leftIndex, rightIndex);
            Console.WriteLine(mean);
        }
    }

    static long CalculateSubarrayMean(long[] prefixSum, int left, int right)
    {
        long subarraySum = prefixSum[right] - prefixSum[left - 1];
        int length = right - left + 1;

        return subarraySum / length;
    }
}
