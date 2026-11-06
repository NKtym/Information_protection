#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <array>
#include <numeric>
#include <random>
#include <cctype>

unsigned long long modMul(unsigned long long a, unsigned long long b, unsigned long long mod) {
    unsigned __int128 t = (unsigned __int128)a * (unsigned __int128)b;
    t %= (unsigned __int128)mod;
    return (unsigned long long)t;
}

unsigned long long modPow(unsigned long long a, unsigned long long e, unsigned long long mod) {
    unsigned long long result = 1 % mod;
    unsigned long long base = a % mod;
    while (e > 0) {
        if ((e & 1ULL) == 1ULL) {
            result = modMul(result, base, mod);
        }
        base = modMul(base, base, mod);
        e >>= 1;
    }
    return result;
}

bool isPrime64(unsigned long long n) {
    if (n < 2ULL) return false;
    const unsigned long long smallPrimes[] = {2ULL,3ULL,5ULL,7ULL,11ULL,13ULL,17ULL,19ULL,23ULL,29ULL,31ULL,37ULL};
    for (unsigned long long p : smallPrimes) {
        if (n % p == 0ULL) 
            return n == p;
    }
    unsigned long long d = n - 1ULL;
    unsigned int s = 0;
    while ((d & 1ULL) == 0ULL) { 
        d >>= 1;
        s++;
    }
    const unsigned long long bases[] = {2ULL,3ULL,5ULL,7ULL,11ULL,13ULL,17ULL,19ULL,23ULL};
    for (unsigned long long a : bases) {
        if (a % n == 0ULL) 
            continue;
        unsigned long long x = modPow(a, d, n);
        if (x == 1ULL || x == n - 1ULL) 
            continue;
        bool composite = true;
        for (unsigned int r = 1; r < s; r++) {
            x = modMul(x, x, n);
            if (x == n - 1ULL) { 
                composite = false;
                break; 
            }
        }
        if (composite) 
            return false;
    }
    return true;
}

static std::mt19937_64 global_rng(static_cast<unsigned long long>(std::time(0)));

unsigned long long genRandom31BitPrime() {
    std::uniform_int_distribution<unsigned int> dist((1U << 30), (1U << 31) - 1U);
    while (true) {
        unsigned int x32 = dist(global_rng);
        unsigned long long x = static_cast<unsigned long long>(x32);
        x |= 1ULL;
        if (isPrime64(x)) 
            return x;
    }
}

long long extendedGCD(long long a, long long b, long long &x, long long &y) {
    long long u1 = a, u2 = 1, u3 = 0;
    long long v1 = b, v2 = 0, v3 = 1;
    while (v1 != 0) {
        long long q = u1 / v1;
        long long t1 = u1 % v1;
        long long t2 = u2 - q * v2;
        long long t3 = u3 - q * v3;
        u1 = v1; 
        u2 = v2; 
        u3 = v3;
        v1 = t1; 
        v2 = t2; 
        v3 = t3;
    }
    x = u2;
    y = u3;
    return u1;
}

long long modInvLong(long long a, long long m) {
    long long x = 0, y = 0;
    long long g = extendedGCD(a, m, x, y);
    if (g != 1) 
        return -1;
    x %= m;
    if (x < 0) 
        x += m;
    return x;
}

struct Edge { 
    int u; 
    int v; 
};

int colorCharToIdx(char c) {
    if (c == 'R' || c == 'r' || c == '0') 
        return 0;
    if (c == 'B' || c == 'b' || c == '1') 
        return 1;
    if (c == 'Y' || c == 'y' || c == '2') 
        return 2;
    return -1;
}

unsigned long long setColorBits(unsigned long long rnd, int color) {
    rnd &= ~3ULL;
    unsigned long long bits = 0ULL;
    if (color == 0) 
        bits = 0ULL;
    else if (color == 1) 
        bits = 1ULL;
    else 
        bits = 2ULL;
    return rnd | bits;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Неверные входные данные. Использование: ./coloring файл a (a - кол-во тестов * |E|)" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    int a_param = 5;
    if (argc >= 3) {
        try { 
            a_param = std::stoi(argv[2]); 
        } 
        catch (...) { 
            a_param = 5; 
        }
        if (a_param <= 0) 
            a_param = 5;
    }
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cout << "Файл не открыт" << std::endl;
        return 1;
    }
    int n = 0;
    long long m = 0;
    fin >> n >> m;
    if (n <= 0 || n > 1000) {
        std::cout << "Неверный n" << std::endl;
        return 1;
    }
    std::vector<Edge> edges;
    edges.reserve(static_cast<size_t>(m));
    for (long long i = 0; i < m; i++) {
        int u = 0, v = 0;
        fin >> u >> v;
        Edge e;
        e.u = u - 1;
        e.v = v - 1;
        edges.push_back(e);
    }
    std::string rest;
    std::getline(fin, rest);
    std::string colorLine;
    bool gotColors = false;
    while (std::getline(fin, colorLine)) {
        if (colorLine.empty()) 
            continue;
        bool allspace = true;
        for (char ch : colorLine) {
            if (!std::isspace(static_cast<unsigned char>(ch))) { 
                allspace = false; 
                break; 
            }
        }
        if (!allspace) { 
            gotColors = true; 
            break; 
        }
    }
    if (!gotColors) {
        std::cout << "Цвета не найдены" << std::endl;
        return 1;
    }
    std::vector<int> color;
    color.resize(static_cast<size_t>(n));
    {
        std::stringstream ss(colorLine);
        for (int i = 0; i < n; i++) {
            std::string token;
            if (!(ss >> token)) { 
                std::cout << "Мало цветов" << std::endl; 
                return 1; 
            }
            int c = -1;
            if (token.size() == 1) 
                c = colorCharToIdx(token[0]);
            else {
                if (std::isdigit(static_cast<unsigned char>(token[0]))) {
                    int val = std::stoi(token);
                    c = val % 3;
                } else {
                    c = colorCharToIdx(token[0]);
                }
            }
            if (c < 0) { 
                std::cout << "Неверный токен цвета" << std::endl; 
                return 1; 
            }
            color[static_cast<size_t>(i)] = c;
        }
    }
    std::cout << "Граф: n=" << n << " m=" << m << std::endl;
    std::cout << "Цвета: ";
    for (int i = 0; i < n; i++) {
        std::cout << color[static_cast<size_t>(i)];
        if (i + 1 < n) 
            std::cout << ' ';
        else 
            std::cout << std::endl;
    }
    struct VertexCommit { 
        unsigned long long p; 
        unsigned long long q; 
        unsigned long long N; 
        unsigned long long d; 
        unsigned long long e; 
        unsigned long long Z; 
        unsigned long long r; 
    };
    std::vector<VertexCommit> V;
    V.resize(static_cast<size_t>(n));
    const unsigned long long PUB_E = 65537ULL;
    for (int i = 0; i < n; i++) {
        while (true) {
            unsigned long long p = genRandom31BitPrime();
            unsigned long long q = genRandom31BitPrime();
            if (p == q) 
                continue;
            unsigned long long N = p * q;
            unsigned long long phi = (p - 1ULL) * (q - 1ULL);
            if (std::gcd(PUB_E, phi) != 1ULL) 
                continue;
            long long phi_ll = static_cast<long long>(phi);
            long long inv = modInvLong(static_cast<long long>(PUB_E), phi_ll);
            if (inv == -1) 
                continue;
            V[static_cast<size_t>(i)].p = p;
            V[static_cast<size_t>(i)].q = q;
            V[static_cast<size_t>(i)].N = N;
            V[static_cast<size_t>(i)].d = static_cast<unsigned long long>(inv);
            V[static_cast<size_t>(i)].e = PUB_E;
            break;
        }
    }
    int rounds = a_param * static_cast<int>(std::max(1LL, m));
    std::mt19937_64 rng(static_cast<unsigned long long>(std::time(0)));
    std::uniform_int_distribution<int> edge_dist(0, static_cast<int>(edges.size() - 1));
    int caught = 0;
    for (int round = 1; round <= rounds; round++) {
        std::array<int,3> perm;
        perm[0] = 0; 
        perm[1] = 1; 
        perm[2] = 2;
        std::shuffle(perm.begin(), perm.end(), rng);
        for (int v = 0; v < n; v++) {
            unsigned long long r = 0;
            unsigned long long Ncurr = V[static_cast<size_t>(v)].N;
            if (Ncurr < 4ULL) {
                r = 2ULL;
            } else {
                std::uniform_int_distribution<unsigned long long> dist_r(2ULL, Ncurr - 2ULL);
                for (;;) {
                    unsigned long long rnd_v = dist_r(rng);
                    r = setColorBits(rnd_v, perm[static_cast<size_t>(color[static_cast<size_t>(v)])]);
                    if (std::gcd(r, Ncurr) == 1ULL) 
                        break;
                }
            }
            V[static_cast<size_t>(v)].r = r;
            V[static_cast<size_t>(v)].Z = modPow(r, V[static_cast<size_t>(v)].d, V[static_cast<size_t>(v)].N);
        }
        int ei = edge_dist(rng);
        Edge e = edges[static_cast<size_t>(ei)];
        int u = e.u;
        int w = e.v;
        unsigned long long cu = V[static_cast<size_t>(u)].e;
        unsigned long long cw = V[static_cast<size_t>(w)].e;
        unsigned long long ru_hat = modPow(V[static_cast<size_t>(u)].Z, cu, V[static_cast<size_t>(u)].N);
        unsigned long long rw_hat = modPow(V[static_cast<size_t>(w)].Z, cw, V[static_cast<size_t>(w)].N);
        int bu = static_cast<int>(ru_hat & 3ULL);
        int bw = static_cast<int>(rw_hat & 3ULL);
        if (bu == bw) {
            caught++;
            std::cout << "Раунд " << round << ": ребро " << (u + 1) << "-" << (w + 1) << " Обман" << std::endl;
        } else {
            std::cout << "Раунд " << round << ": ребро " << (u + 1) << "-" << (w + 1) << " Всё хорошо" << std::endl;
        }
    }
    std::cout << "Поймано " << caught << " из " << rounds << std::endl;
    return 0;
}
