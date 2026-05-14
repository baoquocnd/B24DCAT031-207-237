#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

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
#ifdef _MSC_VER
        unsigned long leading_zero = 0;
        if (_BitScanReverse64(&leading_zero, poly)) return (int)leading_zero;
        else return -1;
#else
        return 63 - __builtin_clzll(poly);
#endif
    }
    PolynomialGF2 operator+(const PolynomialGF2& other) const { return PolynomialGF2(poly ^ other.poly); }
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
        if(b.poly == 0) throw invalid_argument("Loi: Chia cho 0");
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
    PolynomialGF2 operator/(const PolynomialGF2& other) const { return div_mod(*this, other).first; }
    PolynomialGF2 operator%(const PolynomialGF2& other) const { return div_mod(*this, other).second; }
    
    string to_string_fixed(int len) const {
        string s = "";
        for(int i = len - 1; i >= 0; --i) s += ((poly >> i) & 1) ? '1' : '0';
        return s;
    }
    string to_string() const {
        if(poly == 0) return "0";
        string s = "";
        int d = degree();
        for(int i=d; i>=0; --i) s += ((poly >> i) & 1) ? "1" : "0";
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
    void print() const { cout << to_string() << "\n"; }
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

class InfoTheoryApp {
private:
    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        cout << "\033[2J\033[1;1H";
#endif
    }
    
    void printHeader() {
        cout << "=================================================\n";
        cout << "         APP LY THUYET THONG TIN - GF(2)         \n";
        cout << "=================================================\n";
    }

    void processCommand(string type, istream& in, int outputMode, int& questionCount) {
        if(type == "ADD" || type == "MUL" || type == "DIV" || type == "MOD") {
            string a, b; in >> a >> b;
            PolynomialGF2 pA(a), pB(b), res;
            string opName, opSymbol;
            
            if (type == "ADD") { res = pA + pB; opName = "CONG"; opSymbol = " + "; }
            else if (type == "MUL") { res = pA * pB; opName = "NHAN"; opSymbol = " * "; }
            else if (type == "DIV") { res = pA / pB; opName = "CHIA LAY THUONG"; opSymbol = " / "; }
            else if (type == "MOD") { res = pA % pB; opName = "CHIA LAY DU"; opSymbol = " % "; }

            if (outputMode == 1) { 
                if (type == "ADD") cout << res.to_string_fixed(max(a.size(), b.size())) << "\n";
                else res.print();
            } 
            else if (outputMode == 2) { 
                cout << "--- [GIAI CHI TIET] PHEP " << opName << " ---\n";
                cout << "Buoc 1: Chuyen doi chuoi bit thanh da thuc tren GF(2):\n";
                cout << "  A = " << pA.to_math_string() << "\n";
                cout << "  B = " << pB.to_math_string() << "\n";
                cout << "Buoc 2: Thuc hien phep toan:\n";
                cout << "  Ket qua A" << opSymbol << "B = " << res.to_math_string() << "\n";
                cout << "Buoc 3: Ket qua he Nhi phan: " << (type=="ADD" ? res.to_string_fixed(max(a.size(), b.size())) : res.to_string()) << "\n\n";
            } 
            else if (outputMode == 3) { 
                cout << "Cau " << questionCount++ << ": Cho 2 da thuc A = " << pA.to_math_string() << " va B = " << pB.to_math_string() << " tren truong GF(2). ";
                cout << "Gia tri cua bieu thuc A" << opSymbol << "B la:\n";
                cout << " A. " << (res + PolynomialGF2(1)).to_math_string() << "\n"; 
                cout << " B. " << res.to_math_string() << "\n"; 
                cout << " C. " << (pA + pB + PolynomialGF2(2)).to_math_string() << "\n"; 
                cout << " D. Khong xac dinh\n";
                cout << " => DAP AN DUNG: B\n\n";
            }
        } 
        else if(type == "IRREDUCIBLE") {
            string a; in >> a;
            PolynomialGF2 pA(a);
            bool isIrr = isIrreducible(pA);
            
            if (outputMode == 1) {
                cout << (isIrr ? "YES" : "NO") << "\n";
            } 
            else if (outputMode == 2) {
                cout << "--- [GIAI CHI TIET] KIEM TRA DA THUC TOI GIAN ---\n";
                cout << "Da thuc can kiem tra: f(x) = " << pA.to_math_string() << "\n";
                cout << "Ap dung thuat toan Ben-Or test tren GF(2)...\n";
                cout << "Ket luan: f(x) " << (isIrr ? "LA" : "KHONG PHAI LA") << " da thuc bat kha quy.\n\n";
            } 
            else if (outputMode == 3) {
                cout << "Cau " << questionCount++ << ": Da thuc " << pA.to_math_string() << " co phai la da thuc toi gian (bat kha quy) tren GF(2) khong?\n";
                cout << " A. Co\n B. Khong\n";
                cout << " => DAP AN DUNG: " << (isIrr ? "A" : "B") << "\n\n";
            }
        } 
        else if(type == "GENERATE") {
            int d; in >> d;
            if (outputMode == 1) {
                uint64_t start = (1ULL << d) | 1;
                uint64_t end = (1ULL << (d + 1));
                for(uint64_t i = start; i < end; i += 2) {
                    PolynomialGF2 p(i);
                    if(isIrreducible(p)) cout << p.to_string() << " ";
                }
                cout << "\n";
            } 
            else if (outputMode == 2) {
                cout << "--- [GIAI CHI TIET] SINH DA THUC TOI GIAN BAC " << d << " ---\n";
                cout << "Cac da thuc thoa man (dang nhi phan va toan hoc):\n";
                uint64_t start = (1ULL << d) | 1;
                uint64_t end = (1ULL << (d + 1));
                for(uint64_t i = start; i < end; i += 2) {
                    PolynomialGF2 p(i);
                    if(isIrreducible(p)) cout << " + " << p.to_string() << " (" << p.to_math_string() << ")\n";
                }
                cout << "\n";
            }
            else if (outputMode == 3) {
                cout << "Cau " << questionCount++ << ": Chon mot da thuc toi gian bac " << d << " bat ky trong cac dap an sau:\n";
                uint64_t correct_poly = 0;
                for(uint64_t i = (1ULL << d) | 1; i < (1ULL << (d + 1)); i += 2) {
                    if(isIrreducible(PolynomialGF2(i))) { correct_poly = i; break; }
                }
                cout << " A. " << PolynomialGF2((1ULL << d)).to_math_string() << "\n";
                cout << " B. " << PolynomialGF2(correct_poly).to_math_string() << "\n";
                cout << " C. " << PolynomialGF2((1ULL << d) | 2).to_math_string() << "\n";
                cout << " D. Khong co da thuc nao\n";
                cout << " => DAP AN DUNG: B\n\n";
            }
        } else if (type == "BONUS_EXPLAIN_ADD" || type == "BONUS_MCQ_IRREDUCIBLE") {
            if (type == "BONUS_EXPLAIN_ADD") {
                string a, b; in >> a >> b;
            } else if (type == "BONUS_MCQ_IRREDUCIBLE") {
                string a; in >> a;
            }
        } else {
            cout << "Lenh khong hop le.\n";
            string dummy; getline(in, dummy);
        }
    }

public:
    void run() {
        int choice;
        while (true) {
            clearScreen();
            printHeader();
            cout << "1. Nhap input tu ban phim\n";
            cout << "2. Chay test case tu FILE (Chon file & Che do hien thi)\n";
            cout << "0. Thoat ung dung\n";
            cout << "-------------------------------------------------\n";
            cout << "Nhap lua chon: ";
            
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }

            if (choice == 0) break;
            
            if (choice == 1) {
                clearScreen();
                printHeader();
                cout << "--- NHAP INPUT TU BAN PHIM ---\n";
                cout << "Chon che do hien thi ket qua:\n";
                cout << "1. Binh thuong (Chi hien thi ket qua)\n";
                cout << "2. Tu luan (Giai thich tung buoc)\n";
                cout << "3. Trac nghiem (Sinh cau hoi & Dap an)\n";
                cout << "Lua chon cua ban (1/2/3): ";
                
                int mode;
                if (!(cin >> mode) || mode < 1 || mode > 3) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    mode = 1;
                    cout << "Lua chon khong hop le, mac dinh hien thi Binh thuong.\n";
                }

                cout << "\nCu phap: ADD a b | MUL a b | DIV a b | MOD a b | IRREDUCIBLE a | GENERATE d\n";
                cout << "Vi du: ADD 1011 1101\n";
                cout << "Nhap 'EXIT' de quay lai Menu.\n\n";
                
                string type;
                int questionCount = 1;
                while (true) {
                    cout << ">> ";
                    cin >> type;
                    if (type == "EXIT" || type == "exit") break;
                    processCommand(type, cin, mode, questionCount); 
                }
            }
            
            if (choice == 2) {
                clearScreen();
                printHeader();
                string filename;
                cout << "Nhap ten file (vd: input.txt): ";
                cin >> filename;
                ifstream fin(filename);
                if (!fin) {
                    cout << "Khong the mo file: " << filename << "\n";
                } else {
                    cout << "\nChon che do hien thi ket qua:\n";
                    cout << "1. Binh thuong (Chi hien thi ket qua)\n";
                    cout << "2. Tu luan (Giai thich tung buoc)\n";
                    cout << "3. Trac nghiem (Sinh cau hoi & Dap an)\n";
                    cout << "Lua chon cua ban (1/2/3): ";
                    
                    int mode;
                    if (!(cin >> mode) || mode < 1 || mode > 3) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        mode = 1;
                        cout << "Lua chon khong hop le, mac dinh hien thi Binh thuong.\n";
                    }

                    cout << "\n--- KET QUA CHAY TEST CASE ---\n\n";
                    string type;
                    int questionCount = 1;
                    
                    while(fin >> type) {
                        processCommand(type, fin, mode, questionCount);
                    }
                    
                    fin.close();
                    cout << "------------------------------\n";
                }
                cout << "\nNhan Enter de ve Menu..."; 
                cin.ignore(); cin.get();
            }
        }
    }
};

int main() {
    InfoTheoryApp app;
    app.run();
    return 0;
}