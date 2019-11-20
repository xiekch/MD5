#include <bitset>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

typedef unsigned int uint32;
#define BLOCKBYTES 512 / 8
#define MD5BYTES 128 / 8
#define F(B, C, D) (((B) & (C)) | ((~B) & (D)))
#define G(B, C, D) (((B) & (D)) | ((C) & (~D)))
#define H(B, C, D) ((B) ^ (C) ^ (D))
#define I(B, C, D) ((C) ^ ((B) | (~D)))
#define CYCLE(func, A, B, C, D, X, S, T)                                       \
    do {                                                                       \
        A += func(B, C, D) + X + T;                                            \
        A = ((A << S) | (A >> (32 - S)));                                      \
        A += B;                                                                \
    } while (0)

const unsigned int IV[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};

const unsigned int S[64] = {7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 7,
                            12, 17, 22, 5,  9,  14, 20, 5,  9,  14, 20, 5,  9,
                            14, 20, 5,  9,  14, 20, 4,  11, 16, 23, 4,  11, 16,
                            23, 4,  11, 16, 23, 4,  11, 16, 23, 6,  10, 15, 21,
                            6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21};

const unsigned int K[64] = {0,  1,  2,  3, 4, 5,  6,  7,  8,  9,  10, 11, 12,
                            13, 14, 15, 1, 6, 11, 0,  5,  10, 15, 4,  9,  14,
                            3,  8,  13, 2, 7, 12, 5,  8,  11, 14, 1,  4,  7,
                            10, 13, 0,  3, 6, 9,  12, 15, 2,  0,  7,  14, 5,
                            12, 3,  10, 1, 8, 15, 6,  13, 4,  11, 2,  9};

static unsigned int T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
    0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
    0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
    0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
    0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
    0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

class MD5 {
  private:
    ofstream outFile;
    ifstream inFile;
    unsigned int CV[4];
    unsigned int X[20];
    char M[80];
    unsigned long long length;

  public:
    MD5() {
        length = 0;
        memset(M, 0, sizeof(M));
    }

    ~MD5() {
        if (inFile != NULL)
            inFile.close();
        if (outFile != NULL)
            outFile.close();
    }

    bool md5(const char *inFilename, const char *outFilename) {
        memcpy(CV, IV, 4 * 4);

        inFile.open(inFilename);
        outFile.open(outFilename);
        while (inFile.peek() != EOF) {
            memset(M, 0, sizeof(M));
            inFile.read(M, BLOCKBYTES);
            printf("%s\n", M);
            int size = inFile.gcount();
            if (size == BLOCKBYTES) {
                process();
                length += size * 8;
            } else {
                for (int j = 0; j < 64; j++)
                    printf("%x ", M[j]);
                printf("\n");
                length += size * 8;
                finalProcess(size);
            }
        }
        outFile.write((char *)CV, MD5BYTES);
        outFile.close();
        inFile.close();
    }

    void process() {
        memcpy(X, M, BLOCKBYTES);

        unsigned int temp[4];
        memcpy(temp, CV, MD5BYTES);
        for (int i = 0; i < 64; i++) {
            if (i < 16) {
                CYCLE(F, CV[(4 - i % 4) % 4], CV[(4 - (i + 3) % 4) % 4],
                      CV[(4 - (i + 2) % 4) % 4], CV[(4 - (i + 1) % 4) % 4],
                      X[K[i]], S[i], T[i]);

            } else if (i < 32) {
                CYCLE(G, CV[(4 - i % 4) % 4], CV[(4 - (i + 3) % 4) % 4],
                      CV[(4 - (i + 2) % 4) % 4], CV[(4 - (i + 1) % 4) % 4],
                      X[K[i]], S[i], T[i]);

            } else if (i < 48) {
                CYCLE(H, CV[(4 - i % 4) % 4], CV[(4 - (i + 3) % 4) % 4],
                      CV[(4 - (i + 2) % 4) % 4], CV[(4 - (i + 1) % 4) % 4],
                      X[K[i]], S[i], T[i]);
            } else if (i < 64) {
                CYCLE(I, CV[(4 - i % 4) % 4], CV[(4 - (i + 3) % 4) % 4],
                      CV[(4 - (i + 2) % 4) % 4], CV[(4 - (i + 1) % 4) % 4],
                      X[K[i]], S[i], T[i]);
                printf("%d ", i);
            }
        }
        for (int i = 0; i < 4; i++) {
            CV[i] += temp[i];
        }
    }

    void finalProcess(unsigned int size) {
        unsigned int paddingLen = 0;
        if (size < 56) {
            paddingLen = 56 - size;
            padding(size);
            llToChar(length, M + 56);
            process();
        } else {
            paddingLen = 120 - size;
            padding(size);
            process();
            llToChar(length, M + 56);
            process();
        }
    }

    void padding(int size) { M[size] = 0x80u; }

    void llToChar(unsigned long long num, char *output) {
        for (int i = 0; i < 8; i++) {
            output[i] = (num >> i * 8) & 0xff;
        }
    }
};
// printf("a:%x ", A);                                                    \
//         printf("b:%x ", B);                                                    \
//         printf("c:%x ", C);                                                    \
//         printf("d:%x ", D);                                                    \
//         printf("x:%x ", X);                                                    \
//         printf("s:%x ", S);                                                    \
//         printf("t:%x ", T);                                                    \
