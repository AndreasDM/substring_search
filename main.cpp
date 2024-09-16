#include <string>
#include <vector>
#include <utility>
#include <benchmark/benchmark.h>

using namespace std;

// Knuth-Morris-Pratt
template <int R>
struct KMP {
    using vvi = vector<vector<int>>;

    string input;
    vvi    dfa;

    KMP(string s)
    : input{std::move(s)}
    , dfa{R, {static_cast<int>(s.size()+1), 0}}
    {
        dfa[input[0]][0] = 1;
        for (int i = 1, x = 0; i < input.size(); ++i) {
            for (int c = 0; c < R; ++c)
                dfa[c][i] = dfa[c][x];
            dfa[input[i]][i] = i + 1;
            x = dfa[input[i]][x];
        }
    }

    int find_in(const string & txt)
    {
        int i = 0, j = 0;
        while (i < txt.size() && j < input.size())
            j = dfa[txt[i++]][j];
        return j == input.size() ? i - j : txt.size();
    }
};

// Boyer-Moore
template <int R>
struct BoyerMoore {
    string      input;
    vector<int> right;

    BoyerMoore(string s)
    : input{std::move(s)}
    , right(R, -1)
    {
        for (int i = 0; i < input.size(); ++i)
            right[input[i]] = i;
    }

    int find_in(const string & txt)
    {
        int skip;
        for (int i = 0; i < txt.size() - input.size(); i += skip) {
            skip = 0;

            for (int j = input.size() - 1; j >= 0; --j) {
                if (input[j] != txt[i + j]) {
                    skip = max(1, j - right[txt[i+j]]);
                    break;
                }
            }

            if (skip == 0)
                return i;
        }

        return txt.size();
    }
};

// Rabin-Karp
template <int R>
struct RabinKarp {
    string input;
    int    prime = 997; // Should be a random long prime
    int    input_hash;
    int    remove_factor = 1;

    RabinKarp(string s)
    : input{std::move(s)}
    {
        // Calculate Remove Factor
        for (int i = 1; i <= input.size() - 1; ++i)
            remove_factor = (remove_factor * R) % prime;

        input_hash = hash(input, input.size());
    }

    int hash(const string & s, int n)
    {
        int h{};
        for (int i = 0; i < n; ++i)
            h = (h * R + s[i]) % prime;
        return h;
    }

    int find_in(const string & txt)
    {
        int txt_hash = hash(txt, input.size());
        if (txt_hash == input_hash)
            return 0;

        for (int i = input.size(); i < txt.size(); ++i) {
            txt_hash = (txt_hash + prime - remove_factor * txt[i-input.size()] % prime) % prime;
            txt_hash = (txt_hash * R + txt[i]) % prime;
            if (txt_hash == input_hash)
                return i - input.size() + 1;
        }

        return txt.size();
    }
};

void knuth_morris_pratt(benchmark::State & state)
{
    const string haystack = "findaneedleinhaystack";
    const string needle   = "needle";
    constexpr int radix   = 256;

    KMP<radix> kmp{needle};

    for (auto _ : state)
        benchmark::DoNotOptimize(kmp.find_in(haystack));
}

void boyer_moore(benchmark::State & state)
{
    const string haystack = "findaneedleinhaystack";
    const string needle   = "needle";
    constexpr int radix   = 256;

    BoyerMoore<radix> boyer_moore{needle};

    for (auto _ : state)
        benchmark::DoNotOptimize(boyer_moore.find_in(haystack));
}

void rabin_karp(benchmark::State & state)
{
    const string haystack = "findaneedleinhaystack";
    const string needle   = "needle";
    constexpr int radix   = 256;

    RabinKarp<radix> rabin_karp{needle};

    for (auto _ : state)
        benchmark::DoNotOptimize(rabin_karp.find_in(haystack));
}

BENCHMARK(knuth_morris_pratt);
BENCHMARK(boyer_moore);
BENCHMARK(rabin_karp);
BENCHMARK_MAIN();
