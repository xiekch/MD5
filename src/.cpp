#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

unsigned int A(0x67452301);
unsigned int B(0xefcdab89);
unsigned int C(0x98badcfe);
unsigned int D(0x10325476);
unsigned int index[4] = {0x67452301 /*A0*/, 0xefcdab89 /*B1*/,
                         0x98badcfe /*C2*/, 0x10325476 /*D3*/};
bitset<32> M[16];
string Fill(string data);                                       //填充
unsigned int F(unsigned int x, unsigned int y, unsigned int z); //函数F
unsigned int G(unsigned int x, unsigned int y, unsigned int z); //函数G
unsigned int H(unsigned int x, unsigned int y, unsigned int z); //函数H
unsigned int I(unsigned int x, unsigned int y, unsigned int z); //函数I
void FF(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t); // FF函数
void GG(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t); // GG函数
void HH(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t); // HH函数
void II(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t); // II函数
static unsigned int ft[16] = {0xd76aa478, 0xe8c7b756,
                              0x242070db, 0xc1bdceee,
                              0xf57c0faf, 0x4787c62a,
                              0xa8304613, 0xfd469501,
                              0x698098d8, 0x8b44f7af,
                              0xffff5bb1, /*0xfff5bb1*/ 0x895cd7be,
                              0x6b901122, 0xfd987193 /*0xfb987193*/,
                              0xa679438e, 0x49b40821};
static unsigned int gt[16] = {0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                              0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                              0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                              0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a};
static unsigned int ht[16] = {0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                              0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                              0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                              0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665};
static unsigned int it[16] = {0xf4292244, /*0x411aff97*/ 0x432aff97,
                              0xab9423a7, 0xfc93a039,
                              0x655b59c3, 0x8f0ccc92,
                              0xffeff47d, 0x85845dd1,
                              0x6fa87e4f, 0xfe2ce6e0,
                              0xa3014314, 0x4e0811a1,
                              0xf7537e82, 0xbd3af235,
                              0x2ad7d2bb, 0xeb86d391};
void Cycle();
void dvblock(string block, unsigned int, unsigned int);

void main() {
    unsigned int p = 0;
    string data;
    string block;
    string temp;
    cin >> data;
    temp = Fill(data);
    unsigned int blocknum = temp.size() / 512;
    for (int bn = 0; bn < blocknum; bn++) {
        block = temp.substr(bn * 512, 512);
        dvblock(block, (blocknum - 1), (p++));
        Cycle();          //做循环
        A = index[0] + A; // A = a + A
        B = index[1] + B;
        C = index[2] + C;
        D = index[3] + D;
        index[0] = A; // a = A
        index[1] = B;
        index[2] = C;
        index[3] = D;
    }
    for (unsigned int i = 0; i < 4; i++)
        for (unsigned int j = 0; j < 4; j++) {
            cout << setw(2) << setfill('0') << hex
                 << ((index[i] >> (j * 8)) & 0xff);
        }
    cout << endl;
}

string Fill(string data) {
    unsigned int j = data.size() * 8;
    string temp = "";
    for (string::size_type i = 0; i != data.size(); i++) {
        bitset<8> at(data[i]);
        temp += at.to_string();
    }
    temp += "1";
    if (temp.size() % 512 < 448) {
        while (temp.size() % 512 < 448) //
        {
            temp += "0";
        }
    } else {
        while ((temp.size() + 64) % 512 != 0) {
            temp += "0";
        }
    }
    bitset<64> bt(j);
    temp += bt.to_string();
    return temp;
}

unsigned int F(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) | ((~x) & z);
}

unsigned int G(unsigned int x, unsigned int y, unsigned int z) {
    return (x & z) | (y & (~z));
}

unsigned int H(unsigned int x, unsigned int y, unsigned int z) {
    return x ^ y ^ z;
}

unsigned int I(unsigned int x, unsigned int y, unsigned int z) {
    return y ^ (x | (~z));
}

void FF(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t) {

    unsigned int Ftemp = F(B, C, D);
    unsigned int temp = A + (Ftemp + M.to_ulong() + t);
    temp = (temp << s) | (temp >> (32 - s));
    A = B + temp;
}

void GG(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t) {
    unsigned int Gtemp = G(B, C, D);
    unsigned int temp = A + (Gtemp + M.to_ulong() + t);
    temp = (temp << s) | (temp >> (32 - s));
    A = B + temp;
}

void HH(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t) {
    unsigned int Htemp = H(B, C, D);
    unsigned int temp = A + (Htemp + M.to_ulong() + t);
    temp = (temp << s) | (temp >> (32 - s));
    A = B + temp;
}

void II(unsigned int &A, unsigned int B, unsigned int C, unsigned int D,
        bitset<32> M, unsigned int s, unsigned int t) {

    unsigned int Itemp = I(B, C, D);
    unsigned int temp = A + (Itemp + M.to_ulong() + t);
    temp = (temp << s) | (temp >> (32 - s));
    A = B + temp;
}

void Cycle() {
    int i = 0,j;
    //第一轮
    unsigned int sfindex[4] = {7, 12, 17, 22};
    for (int j = 0; j < 16; j++) {
        FF(index[i], index[(i + 1) % 4], index[(i + 2) % 4], index[(i + 3) % 4],
           M[j], sfindex[j % 4], ft[j]);
        i = (i + 3) % 4;
    }

    //第二轮
    unsigned int sgindex[4] = {5, 9, 14, 20};
    unsigned int mgindex[16] = {1, 6,  11, 0, 5,  10, 15, 4,
                                9, 14, 3,  8, 13, 2,  7,  12};
    i = 0;
    for (j = 0; j < 16; j++) {
        GG(index[i], index[(i + 1) % 4], index[(i + 2) % 4], index[(i + 3) % 4],
           M[mgindex[j]], sgindex[j % 4], gt[j]);
        i = (i + 3) % 4;
    }

    //第三轮
    unsigned int shindex[4] = {4, 11, 16, 23};
    unsigned int mhindex[16] = {5,  8, 11, 14, 1, 4,  7,  10,
                                13, 0, 3,  6,  9, 12, 15, 2};
    i = 0;
    for (j = 0; j < 16; j++) {
        HH(index[i], index[(i + 1) % 4], index[(i + 2) % 4], index[(i + 3) % 4],
           M[mhindex[j]], shindex[j % 4], ht[j]);
        i = (i + 3) % 4;
    }

    //第四轮
    unsigned int siindex[4] = {6, 10, 15, 21};
    unsigned int miindex[16] = {0, 7,  14, 5,  12, 3,  10, 1,
                                8, 15, 6,  13, 4,  11, 2,  9};
    i = 0;
    for (j = 0; j < 16; j++) {
        II(index[i], index[(i + 1) % 4], index[(i + 2) % 4], index[(i + 3) % 4],
           M[miindex[j]], siindex[j % 4], it[j]);
        i = (i + 3) % 4;
    }
}

void dvblock(string block, unsigned int bn, unsigned int position) {
    string temp14, temp15;
    string temp;
    if (position == bn) {
        for (int n = 0; n < 16; n++) {
            temp = "";
            if (n < 14) {
                for (int i = 3; i != -1; i--) {
                    temp += block.substr(n * 32 + i * 8, 8);
                }
                bitset<32> bttemp(temp);
                M[n] = bttemp;
            } else if (n == 14)
                temp14 = block.substr(n * 32, 32);
            else if (n == 15)
                temp15 = block.substr(n * 32, 32);
        }
        bitset<32> t14(temp14);
        bitset<32> t15(temp15);
        M[14] = t15;
        M[15] = t14;
    } else {
        for (int n = 0; n < 16; n++) {
            temp = "";
            for (int i = 3; i != -1; i--) {
                temp += block.substr(n * 32 + i * 8, 8);
            }
            bitset<32> bttemp(temp);
            M[n] = bttemp;
        }
    }
}
