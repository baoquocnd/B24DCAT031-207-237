#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

using namespace std;

class Bit {
public:
    bool val;
    Bit(bool v = 0) : val(v) {}
    Bit operator+(const Bit& other) const { return Bit(val ^ other.val); }
    Bit operator*(const Bit& other) const { return Bit(val & other.val); }
    bool operator==(const Bit& other) const { return val == other.val; }
    bool operator!=(const Bit& other) const { return val != other.val; }
};

class BinaryVector {
public:
    vector<Bit> bits;
    BinaryVector() {}
    BinaryVector(string s) {
        for(char c : s) bits.push_back(Bit(c == '1'));
    }
    BinaryVector operator+(const BinaryVector& other) const {
        BinaryVector res;
        int n = max(bits.size(), other.bits.size());
        for(int i=0; i<n; ++i) {
            Bit a = i < bits.size() ? bits[i] : Bit(0);
            Bit b = i < other.bits.size() ? other.bits[i] : Bit(0);
            res.bits.push_back(a + b);
        }
        return res;
    }
    int weight() const {
        int w = 0;
        for(auto b : bits) w += b.val;
        return w;
    }
    int distance(const BinaryVector& other) const {
        return (*this + other).weight();
    }
    void print() const {
        for(auto b : bits) cout << b.val;
        cout << "\n";
    }
};

class PolynomialGF2 {
public:
    uint64_t poly; 
    PolynomialGF2(uint64_t p = 0) : poly(p) {}
    PolynomialGF2(string s) {
        poly = 0;
        int n = s.length();
        for(int i=0; i<n; ++i) {
            if(s[n-1-i] == '1') poly |= (1ULL << i);
        }
    }
    int degree() const {
        if(poly == 0) return -1;
        return 63 - __builtin_clzll(poly);
    }
    PolynomialGF2 operator+(const PolynomialGF2& other) const {
        return PolynomialGF2(poly ^ other.poly);
    }
    PolynomialGF2 operator*(const PolynomialGF2& other) const {
        uint64_t res = 0;
        uint64_t a = poly, b = other.poly;
        while(b > 0) {
            if(b & 1) res ^= a;
            a <<= 1;
            b >>= 1;
        }
        return PolynomialGF2(res);
    }
    static pair<PolynomialGF2, PolynomialGF2> div_mod(PolynomialGF2 a, PolynomialGF2 b) {
        if(b.poly == 0) return {0, a};
        int da = a.degree(), db = b.degree();
        if(da < db) return {0, a};
        uint64_t rem = a.poly;
        uint64_t q = 0;
        for(int i = da - db; i >= 0; --i) {
            if((rem >> (i + db)) & 1) {
                rem ^= (b.poly << i);
                q |= (1ULL << i);
            }
        }
        return {PolynomialGF2(q), PolynomialGF2(rem)};
    }
    PolynomialGF2 operator/(const PolynomialGF2& other) const {
        return div_mod(*this, other).first;
    }
    PolynomialGF2 operator%(const PolynomialGF2& other) const {
        return div_mod(*this, other).second;
    }
    
string to_string_fixed(int len) const {
    string s = "";

    for(int i = len - 1; i >= 0; --i) {
        s += ((poly >> i) & 1) ? '1' : '0';
    }

    return s;
}


    string to_string() const {
        if(poly == 0) return "0";
        string s = "";
        int d = degree();
        for(int i=d; i>=0; --i) {
            s += ((poly >> i) & 1) ? "1" : "0";
        }
        return s;
    }
    string to_math_string() const {
        if(poly == 0) return "0";
        string s = "";
        int d = degree();
        bool first = true;
        for(int i=d; i>=0; --i) {
            if((poly >> i) & 1) {
                if(!first) s += " + ";
                if(i == 0) s += "1";
                else if(i == 1) s += "x";
                else s += "x^" + std::to_string(i);
                first = false;
            }
        }
        return s;
    }
    void print() const {
        cout << to_string() << "\n";
    }
};

PolynomialGF2 gcd(PolynomialGF2 a, PolynomialGF2 b) {
    while(b.poly > 0) {
        PolynomialGF2 r = a % b;
        a = b;
        b = r;
    }
    return a;
}

PolynomialGF2 mod_pow(PolynomialGF2 base, uint64_t exp, PolynomialGF2 mod) {
    PolynomialGF2 res(1);
    base = base % mod;
    while(exp > 0) {
        if(exp & 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return res;
}


bool isIrreducible(PolynomialGF2 p) {
    int d = p.degree();

    // degree 0 hoặc polynomial 0/1 không phải bất khả quy
    if(d <= 0) return false;

    PolynomialGF2 x(2); // x

    // Ben-Or test:
    // gcd(p, x^(2^i) - x) = 1 với mọi i = 1..d/2
    for(int i = 1; i <= d / 2; ++i) {

        PolynomialGF2 x2i = mod_pow(x, 1ULL << i, p);

        // Trong GF(2): subtraction == addition
        PolynomialGF2 f = x2i + x;

        PolynomialGF2 g = gcd(p, f);

        if(g.poly != 1)
            return false;
    }

    // Điều kiện đầy đủ:
    // x^(2^d) mod p phải bằng x

    PolynomialGF2 final_check = mod_pow(x, 1ULL << d, p);

    if((final_check + x).poly != 0)
        return false;

    return true;
}


void generateIrreducible(int degree) {
    uint64_t start = (1ULL << degree) | 1;
    uint64_t end = (1ULL << (degree + 1));
    for(uint64_t i = start; i < end; i += 2) {
        PolynomialGF2 p(i);
        if(isIrreducible(p)) {
            cout << p.to_string() << " ";
        }
    }
    cout << "\n";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    string type;
    while(cin >> type) {
        if(type == "ADD") {
            string a, b; cin >> a >> b;
            PolynomialGF2 res = PolynomialGF2(a) + PolynomialGF2(b);
            int len = max(a.size(), b.size());
            cout << res.to_string_fixed(len) << "\n";
        } else if(type == "MUL") {
            string a, b; cin >> a >> b;
            (PolynomialGF2(a) * PolynomialGF2(b)).print();
        } else if(type == "DIV") {
            string a, b; cin >> a >> b;
            (PolynomialGF2(a) / PolynomialGF2(b)).print();
        } else if(type == "MOD") {
            string a, b; cin >> a >> b;
            (PolynomialGF2(a) % PolynomialGF2(b)).print();
        } else if(type == "IRREDUCIBLE") {
            string a; cin >> a;
            cout << (isIrreducible(PolynomialGF2(a)) ? "YES" : "NO") << "\n";
        } else if(type == "GENERATE") {
            int d; cin >> d;
            generateIrreducible(d);
        } else if(type == "BONUS_EXPLAIN_ADD") {
            string a, b; cin >> a >> b;
            PolynomialGF2 p1(a), p2(b);
            cout << "Step 1:\nA = " << p1.to_math_string() << "\n";
            cout << "Step 2:\nB = " << p2.to_math_string() << "\n";
            cout << "Step 3:\nA + B = " << (p1 + p2).to_math_string() << "\n";
        } else if(type == "BONUS_MCQ_IRREDUCIBLE") {
            string a; cin >> a;
            cout << "Question: Is polynomial " << PolynomialGF2(a).to_math_string() << " irreducible?\n";
            cout << "Answer: " << (isIrreducible(PolynomialGF2(a)) ? "YES" : "NO") << "\n";
        }
    }
    return 0;
}