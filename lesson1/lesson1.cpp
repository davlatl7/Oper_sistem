#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void part_sum(const int* arr, size_t a, size_t b, long long* res) {
    long long s = 0;
    for (size_t i = a; i < b; ++i) s += arr[i];
    *res = s;
}


std::vector<int> make_data(size_t n) {
    std::vector<int> v(n);
    unsigned r = 123456789u;

    for (size_t i = 0; i < n; ++i) {
        r = r * 1103515245u + 12345u;
        v[i] = int(r % 100u) + 1;
    }
    return v;
}

long long single_thread_sum(const std::vector<int>& v, double& out_time) {
    auto t1s = std::chrono::high_resolution_clock::now();

    long long s = 0;
    for (auto x : v) s += x;

    auto t1e = std::chrono::high_resolution_clock::now();
    out_time = std::chrono::duration<double>(t1e - t1s).count();

    return s;
}

long long multi_thread_sum(
        const std::vector<int>& v,
        int th_n,
        double& out_time)
{
    auto t2s = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> th(th_n);
    std::vector<long long> ps(th_n, 0);

    size_t n = v.size();
    size_t seg = n / th_n;

    for (int i = 0; i < th_n; ++i) {
        size_t a = i * seg;
        size_t b = (i == th_n - 1) ? n : a + seg;
        th[i] = std::thread(part_sum, v.data(), a, b, &ps[i]);
    }
    for (auto& t : th) t.join();

    long long sum = 0;
    for (auto s : ps) sum += s;

    auto t2e = std::chrono::high_resolution_clock::now();
    out_time = std::chrono::duration<double>(t2e - t2s).count();

    return sum;
}


int main() {
    const size_t n = 1'000'000;
    const int th_n = 4;

    std::vector<int> v = make_data(n);

    double t1, t2;

    long long s1 = single_thread_sum(v, t1);
    long long s2 = multi_thread_sum(v, th_n, t2);

    std::cout << "1 Thread:\n"
              << "-----------------------------\n"
              << "Sum: " << s1 << "\n"
              << "Time: " << t1 << " s\n\n";

    std::cout << th_n << " Threads:\n"
              << "-----------------------------\n"
              << "Sum: " << s2 << "\n"
              << "Time: " << t2 << " s\n"
              << "Speedup: " << (t1 / t2) << "x\n";

    return 0;
}

