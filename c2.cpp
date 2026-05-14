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
    if(d <= 0) return false;
    PolynomialGF2 x(2); 

    for(int i = 1; i <= d / 2; ++i) {
        PolynomialGF2 x2i = mod_pow(x, 1ULL << i, p);
        PolynomialGF2 f = x2i + x;
        PolynomialGF2 g = gcd(p, f);
        if(g.poly != 1) return false;
    }

    PolynomialGF2 final_check = mod_pow(x, 1ULL << d, p);
    if((final_check + x).poly != 0) return false;

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
    int choice;
    while(true) {
        cout << "\n=========================================\n";
        cout << "   UNG DUNG DAI SO DA THUC TREN GF(2)    \n";
        cout << "=========================================\n";
        cout << "1. Cong hai da thuc\n";
        cout << "2. Nhan hai da thuc\n";
        cout << "3. Chia hai da thuc (lay thuong)\n";
        cout << "4. Chia hai da thuc (lay du - Modulo)\n";
        cout << "5. Kiem tra da thuc bat kha quy (Irreducible)\n";
        cout << "6. Liet ke da thuc bat kha quy theo bac\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "Nhap lua chon cua ban (0-6): ";
        if (!(cin >> choice) || choice == 0) break;

        if (choice >= 1 && choice <= 4) {
            string a, b;
            cout << "Nhap da thuc A (dang bit): "; cin >> a;
            cout << "Nhap da thuc B (dang bit): "; cin >> b;
            PolynomialGF2 p1(a), p2(b);
            PolynomialGF2 res;
            string opName, opSymbol;

            if(choice == 1) { res = p1 + p2; opName = "Tong"; opSymbol = " + "; }
            else if(choice == 2) { res = p1 * p2; opName = "Tich"; opSymbol = " * "; }
            else if(choice == 3) { res = p1 / p2; opName = "Thuong"; opSymbol = " / "; }
            else if(choice == 4) { res = p1 % p2; opName = "Phan du"; opSymbol = " % "; }

            cout << "\nChon dinh dang Output:\n";
            cout << "1. Output chuan ICPC\n";
            cout << "2. Output Tu luan (BONUS)\n";
            cout << "3. Output Trac nghiem (BONUS)\n";
            cout << "Lua chon (1-3): ";
            int outFormat; cin >> outFormat;

            if (outFormat == 1) {
                if (choice == 1) {
                    int len = max(a.size(), b.size());
                    cout << res.to_string_fixed(len) << "\n";
                } else {
                    cout << res.to_string() << "\n";
                }
            } else if (outFormat == 2) {
                cout << "\n[LOI GIAI TU LUAN]\n";
                cout << "Buoc 1: A = " << p1.to_math_string() << "\n";
                cout << "Buoc 2: B = " << p2.to_math_string() << "\n";
                cout << "Buoc 3: A" << opSymbol << "B = " << res.to_math_string() << "\n";
            } else if (outFormat == 3) {
                cout << "\n[CAU HOI TRAC NGHIEM]\n";
                cout << "Cau hoi: " << opName << " cua (" << p1.to_math_string() << ") va (" << p2.to_math_string() << ") la?\n";
                cout << "Dap an: " << res.to_math_string() << "\n";
            }
        } else if (choice == 5) {
            string a;
            cout << "Nhap da thuc can kiem tra (dang bit): "; cin >> a;
            PolynomialGF2 p(a);

            cout << "\nChon dinh dang Output:\n";
            cout << "1. Output chuan ICPC\n";
            cout << "2. Output Tu luan (BONUS)\n";
            cout << "3. Output Trac nghiem (BONUS)\n";
            cout << "Lua chon (1-3): ";
            int outFormat; cin >> outFormat;

            bool isIrr = isIrreducible(p);
            if (outFormat == 1) {
                cout << (isIrr ? "YES" : "NO") << "\n";
            } else if (outFormat == 2) {
                cout << "\n[LOI GIAI TU LUAN]\n";
                cout << "Da thuc: P(x) = " << p.to_math_string() << "\n";
                cout << "Kiem tra bang thuat toan Ben-Or (tinh GCD voi cac da thuc x^(2^i) + x tren GF(2))...\n";
                cout << "Ket luan: P(x) " << (isIrr ? "LA" : "KHONG PHAI") << " da thuc bat kha quy.\n";
            } else if (outFormat == 3) {
                cout << "\n[CAU HOI TRAC NGHIEM]\n";
                cout << "Cau hoi: Da thuc " << p.to_math_string() << " co phai la da thuc bat kha quy khong?\n";
                cout << "Dap an: " << (isIrr ? "YES" : "NO") << "\n";
            }
        } else if (choice == 6) {
            int d;
            cout << "Nhap bac cua da thuc: "; cin >> d;
            cout << "Cac da thuc bat kha quy bac " << d << " tren GF(2) la:\n";
            generateIrreducible(d);
        } else {
            cout << "Lua chon khong hop le!\n";
        }
    }
    return 0;
}
