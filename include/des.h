#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

typedef struct message
{
	long chiffrerHexa;
	int chiffrerBinaire[64];
	int chiffrerBinairePermuter[64];
	int leftChiffrer[32];
	int rightChiffrer[32];
	int rightChiffrerExp[48];
	int sbox6Bits[6];
	int sbox6BitsXorer[6];
	int sbox4Bits[4];
} Message;

typedef struct clef
{
	int key48bit[48];
	int key56bit[56];
	int key64bitb[64];
	int key8bit[8];
} Key;

typedef struct des
{
	int claireBinaire[64];
	int key64Bit[64];
	int claireBinaireIp[64];
	int right32Bit[32];
	int left32Bit[32];
	int right32BitPlus1[32];
	int left32BitPlus1[32];
	int right48Bit[48];
	int subKey[16][48];
	int chiffrerBinaire[64];
} DES;

static const int ip[64] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7};

static const int ipMoin1[64] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25};

static const int e[48] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1};

static const int p[32] = {
	16, 7, 20, 21, 29, 12, 28, 17,
	1, 15, 23, 26, 5, 18, 31, 10,
	2, 8, 24, 14, 32, 27, 3, 9,
	19, 13, 30, 6, 22, 11, 4, 25};

static const int pMoin1[32] = {
	9, 17, 23, 31,
	13, 28, 2, 18,
	24, 16, 30, 6,
	26, 20, 10, 1,
	8, 14, 25, 3,
	4, 29, 11, 19,
	32, 12, 22, 7,
	5, 27, 15, 21};

static const int sbox[8][4][16] = {
	{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
	 {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
	 {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
	 {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
	{{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
	 {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
	 {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
	 {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
	{{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
	 {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
	 {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
	 {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
	{{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
	 {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
	 {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
	 {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
	{{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
	 {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
	 {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
	 {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
	{{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
	 {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
	 {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
	 {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
	{{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
	 {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
	 {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
	 {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
	{{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
	 {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
	 {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
	 {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

static const int pc1[56] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4};

static const int pc1Moin1[64] = {
	8, 16, 24, 56, 52, 44, 36, 0,
	7, 15, 23, 55, 51, 43, 35, 0,
	6, 14, 22, 54, 50, 42, 34, 0,
	5, 13, 21, 53, 49, 41, 33, 0,
	4, 12, 20, 28, 48, 40, 32, 0,
	3, 11, 19, 27, 47, 39, 31, 0,
	2, 10, 18, 26, 46, 38, 30, 0,
	1, 9, 17, 25, 45, 37, 29, 0};

static const int pc2[48] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32};

static const int pc2Moin1[56] = {
	5, 24, 7, 16, 6, 10, 20,
	18, 0, 12, 3, 15, 23, 1,
	9, 19, 2, 0, 14, 22, 11,
	0, 13, 4, 0, 17, 21, 8,
	47, 31, 27, 48, 35, 41, 0,
	46, 28, 0, 39, 32, 25, 44,
	0, 37, 34, 43, 29, 36, 38,
	45, 33, 26, 42, 0, 30, 40};

static const long messageClaire = 0xFDC16E3E3EB9A1C7;

static const long chiffrerJuste = 0x8FD0A0ED21824EF2;

static const long chiffrerFaux[32] = {
	0x8DD4A0E921834EF6,
	0x8FD2A0AD21834EF2,
	0x8FC0A2AD21824EF2,
	0x8F80A4AB31824EF2,
	0x8F80A4E933824EF2,
	0x8E90A4ED21804EF2,
	0x8FD0A4ED31824CF2,
	0x8ED0A0EC71824EF0,
	0x8790A4ED61964EF2,
	0x8FD8A0ED21964EF2,
	0x8FD0A8ED61924EF2,
	0x8FD0B0E421964EF3,
	0xCFD0B0ED69964EF3,
	0xCFD0B0ED218A4EF2,
	0xCFD0B0ED21C246F3,
	0xCFD0B0ED25C25EFA,
	0xEFD0B0ED25824FB3,
	0x8FF0A0ED25824FF2,
	0x8FD080ED25824EB2,
	0x9FD0E1CD21825FF2,
	0x9FD0E1ED05824FB2,
	0x9BD0A1ED21A24EF2,
	0x8FD0E1ED21826EF2,
	0x9BD0E1FD21820AD2,
	0x1BD0E0FD20820EF2,
	0x8F50A0ED20824AF2,
	0x8FD020FD21820EF2,
	0x8FD5A06D20820AF6,
	0x8FD5A0FDA0820AF2,
	0x8FD5A0ED21024EE2,
	0x8FD4A0ED2182CEF6,
	0x8FD1A0E921834E66};

void hexatobinary(int *tabResult, long hexa, int nbrHexa);

void decimaltobinary(int *tabResult, int decimal, int nbrBit);

long puissance(int a,int b);

int TabtoInt(int *tab,int nbrBit);

long TabtoLong(int *tab,int nbrBit);

void Permutation(int *resultat,int *aPermuter,const int *tablePermutation, int nbrBit);

void splitTab(int *completTab,int *leftTab,int *rightTab, int nbrBit);

void xor(int *tabResult, int *premierK, int *deuxiemeK, int nbrBit);

int bitFauter(int *tabJuste, int *tabFaux);

void Sboxfonc(int *resultat, int *entrer, int numSbox);

void obtenirR16L16(long hexa,Message *m);

void extraire6Bits(Message *m, int position);

int egale(int *tab1, int *tab2, int nbrBit);

long k16enHexa(int tabK16[8][64]);

long rechercheExostive(const long LechiffrerJuste, const long *LeschiffrerFaux);

void initTab(int *tab, int nbrBit);

void shiftGauche(int *resultat, int *tabAshifter, int nbrShift, int nbrBit);

void fusionTab(int *resultat, int *leftTab, int *rightTab, int nbrBit);

void generationSubKey(int Les16SubKey[][48], int *key64Bit);

void f(int *resultat, int *Ri, int *Ki);

void copieTab(int *resultat, int * aCopier, int nbrBit);

long fonctionDES(long claire, long k64);

long getK56bit(long claire, long chiffrer, long k16);

long getK64bitParite(long claire, long chiffrer, long k16);