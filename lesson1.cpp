#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void part_sum(const int* arr, size_t a, size_t b, long long* res) {
    long long s = 0;
    for (size_t i = a; i < b; ++i) s += arr[i];
    *res = s;
}

int main() {
    const size_t n = 1'000'000;
    const int th_n = 4;

    std::vector<int> v(n);
    unsigned r = 123456789u;
    for (size_t i = 0; i < n; ++i) {
        r = r * 1103515245u + 12345u;
        v[i] = int(r % 100u) + 1;
    }

    // --- single thread ---
    auto t1s = std::chrono::high_resolution_clock::now();
    long long s1 = 0;
    for (auto x : v) s1 += x;
    auto t1e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt1 = t1e - t1s;

    std::cout << "1 Thread:\n";
    std::cout << "-----------------------------\n";
    std::cout << "Sum: " << s1 << "\n";
    std::cout << "Time: " << dt1.count() << " s\n";
    std::cout << "-----------------------------\n\n";

    // --- multi-thread ---
    auto t2s = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> th(th_n);
    std::vector<long long> ps(th_n, 0);

    size_t seg = n / th_n;
    for (int i = 0; i < th_n; ++i) {
        size_t a = i * seg;
        size_t b = (i == th_n - 1) ? n : a + seg;
        th[i] = std::thread(part_sum, v.data(), a, b, &ps[i]);
    }
    for (auto& t : th) t.join();

    long long s2 = 0;
    for (auto p : ps) s2 += p;

    auto t2e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt2 = t2e - t2s;

    std::cout << th_n << " Threads:\n";
    std::cout << "-----------------------------\n";
    std::cout << "Sum: " << s2 << "\n";
    std::cout << "Time: " << dt2.count() << " s\n";
    std::cout << "-----------------------------\n";
    std::cout << "Speedup: " << (dt1.count() / dt2.count()) << "x\n";

    return 0;
}
