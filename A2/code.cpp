#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>

class ArrayGenerator {
public:
    static std::vector<int> generateRandomArray() {
        std::vector<int> a(10000);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 6000);
        for (int& v : a) {
            v = dis(gen);
        }
        return a;
    }

    static std::vector<int> generateReverseSortedArray() {
        std::vector<int> a = generateRandomArray();
        std::sort(a.begin(), a.end());
        std::reverse(a.begin(), a.end());
        return a;
    }

    static std::vector<int> generateNearlySortedArray() {
        std::vector<int> a = generateRandomArray();
        std::sort(a.begin(), a.end());
        for (int i = 0; i < 5; i++) {
            int index1 = rand() % 10000;
            int index2 = rand() % 10000;
            std::swap(a[index1], a[index2]);
        }
        return a;
    }
};


class SortTester {
public:
    void mergeSort(std::vector<int>& arr, int l, int r) {
        if (l < r) {
            int m = l + (r - l) / 2;
            mergeSort(arr, l, m);
            mergeSort(arr, m + 1, r);
            merge(arr, l, m, r);
        }
    }

    void hybridSort(std::vector<int>& arr, int l, int r, int threshold) {
        if (r - l <= threshold) {
            insertionSort(arr, l, r);
        }
        else {
            int m = l + (r - l) / 2;
            hybridSort(arr, l, m, threshold);
            hybridSort(arr, m + 1, r, threshold);
            merge(arr, l, m, r);
        }
    }

    long long measureTimeMerge(std::vector<int>& arr, int l, int r) {
        auto start = std::chrono::high_resolution_clock::now();
        mergeSort(arr, l, r);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }
    long long measureTimeHybryd(std::vector<int>& arr, int l, int r, int threshold) {
        auto start = std::chrono::high_resolution_clock::now();
        hybridSort(arr, l, r, threshold);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }

    void saveResults(const std::string& filename, const std::vector<long long>& res) {
        std::ofstream file(filename);
        file << "[";
        for (const auto& result : res) {
            file << result << ",";
        }
        file << "]";
        file.close();
    }

private:
    void merge(std::vector<int>& a, int l, int mid, int r) {
        int n1 = mid - l + 1, n2 = r - mid;
        std::vector<int> L(n1), R(n2);
        for (int i = 0; i < n1; i++) {
            L[i] = a[l + i];
        }
        for (int j = 0; j < n2; j++) {
            R[j] = a[mid + 1 + j];
        }
        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) {
            if (L[i] <= R[j]) {
                a[k] = L[i];
                i++;
            }
            else {
                a[k] = R[j];
                j++;
            }
            k++;
        }
        while (i < n1) {
            a[k] = L[i];
            i++;
            k++;
        }
        while (j < n2) {
            a[k] = R[j];
            j++;
            k++;
        }
    }

    void insertionSort(std::vector<int>& a, int l, int r) {
        for (int i = l + 1; i <= r; i++) {
            int k = a[i], j = i - 1;
            while (j >= l && a[j] > k) {
                a[j + 1] = a[j];
                j--;
            }
            a[j + 1] = k;
        }
    }
};



int main() {
    int threshold = 10;
    SortTester st;
    ArrayGenerator ag;
    std::vector<int> randomA = ag.generateRandomArray();
    std::vector<int> revSortedA = ag.generateReverseSortedArray();
    std::vector<int> nearlySortedA = ag.generateNearlySortedArray();
    std::vector<long long> mergeSortResultsRandom;
    std::vector<long long> hybridSortResultsRandom;
    std::vector<long long> mergeSortResultsReversed;
    std::vector<long long> hybridSortResultsReversed;
    std::vector<long long> mergeSortResultsNearlyS;
    std::vector<long long> hybridSortResultsNearlyS;
    for (int i = 500; i <= 10000; i += 100) {
        long long totalMergeTimeRandom = 0;
        long long totalHybridTimeRandom = 0;
        long long totalMergeTimeReversed = 0;
        long long totalHybridTimeReversed = 0;
        long long totalMergeTimeNearlyS = 0;
        long long totalHybridTimeNearlyS = 0;
        const int numTests = 10;

        for (int test = 0; test < numTests; test++) {
            std::vector<int> randomA1(i), revSortedA1(i), nearlySortedA1(i);
            std::copy(randomA.begin(), randomA.begin() + i, randomA1.begin());
            std::copy(revSortedA.begin(), revSortedA.begin() + i, revSortedA1.begin());
            std::copy(nearlySortedA.begin(), nearlySortedA.begin() + i, nearlySortedA1.begin());
            std::vector<int> randomA2 = randomA1, revSortedA2 = revSortedA1, nearlySortedA2 = nearlySortedA1;

            totalMergeTimeRandom += st.measureTimeMerge(randomA1, 0, i - 1);
            totalHybridTimeRandom += st.measureTimeHybryd(randomA2, 0, i - 1, threshold);
            totalMergeTimeReversed += st.measureTimeMerge(revSortedA1, 0, i - 1);
            totalHybridTimeReversed += st.measureTimeHybryd(revSortedA2, 0, i - 1, threshold);
            totalMergeTimeNearlyS += st.measureTimeMerge(nearlySortedA1, 0, i - 1);
            totalHybridTimeNearlyS += st.measureTimeHybryd(nearlySortedA2, 0, i - 1, threshold);
        }
        mergeSortResultsRandom.push_back(totalMergeTimeRandom / numTests);
        hybridSortResultsRandom.push_back(totalHybridTimeRandom / numTests);
        mergeSortResultsReversed.push_back(totalMergeTimeReversed / numTests);
        hybridSortResultsReversed.push_back(totalHybridTimeReversed / numTests);
        mergeSortResultsNearlyS.push_back(totalMergeTimeNearlyS / numTests);
        hybridSortResultsNearlyS.push_back(totalHybridTimeNearlyS / numTests);

    }

    st.saveResults("RandomArray_merge_sort_results.txt", mergeSortResultsRandom);
    st.saveResults("RandomArray_hybrid_sort_results.txt", mergeSortResultsRandom);
    st.saveResults("ReversedArray_merge_sort_results.txt", mergeSortResultsRandom);
    st.saveResults("ReversedArray_hybrid_sort_results.txt", mergeSortResultsRandom);
    st.saveResults("NearlySortedArray_merge_sort_results.txt", mergeSortResultsRandom);
    st.saveResults("NearlySortedArray_hybrid_sort_results.txt", mergeSortResultsRandom);
    return 0;
}
