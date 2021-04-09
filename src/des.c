#include "../include/des.h"

long puissance(int a, int b)
{
   if (b == 0)
   {
      return 1;
   }
   else
   {
      return a * puissance(a, b - 1);
   }
}

int TabtoInt(int *tab, int nbrBit)
{
   int nombre = 0;
   int i = 0, j = nbrBit - 1;
   while (j >= 0)
   {
      if (tab[j] != 0)
      {
         nombre += puissance(2, i);
      }
      i++;
      j--;
   }
   return nombre;
}

long TabtoLong(int *tab, int nbrBit)
{
   long nombre = 0;
   int i = 0, j = nbrBit - 1;
   while (j >= 0)
   {
      if (tab[j] != 0)
      {
         nombre += puissance(2, i);
      }
      i++;
      j--;
   }
   return nombre;
}

void hexatobinary(int *tabResult, long hexa, int nbrHexa)
{
   long tmp = hexa;
   int entier;
   int compteur = nbrHexa * 4 - 1;
   for (int j = 0; j < nbrHexa; j++)
   {
      entier = tmp & 0xF;
      for (int i = 0; i < 4; i++)
      {
         tabResult[compteur] = entier % 2;
         entier = entier / 2;
         compteur--;
      }
      tmp = tmp >> 4;
   }
}

void decimaltobinary(int *tabResult, int decimal, int nbrBit)
{
   int entier = decimal;
   for (int i = nbrBit - 1; i >= 0; i--)
   {
      tabResult[i] = entier % 2;
      entier = entier / 2;
   }
}


void Permutation(int *resultat, int *aPermuter, const int *tablePermutation, int nbrBit)
{
   for (int i = 0; i < nbrBit; i++)
   {
      if (tablePermutation[i] != 0)
      {
         resultat[i] = aPermuter[tablePermutation[i] - 1];
      }
   }
}

void splitTab(int *completTab, int *leftTab, int *rightTab, int nbrBit)
{
   for (int i = 0; i < nbrBit; i++)
   {
      leftTab[i] = completTab[i];
      rightTab[i] = completTab[i + nbrBit];
   }
}

void xor (int *tabResult, int *premierK, int *deuxiemeK, int nbrBit) {
   for (int i = 0; i < nbrBit; i++)
   {
      tabResult[i] = premierK[i] ^ deuxiemeK[i];
   }
}

int bitFauter(int *tabJuste, int *tabFaux)
{
   int tabxor[33] = {0};
   xor(tabxor, tabJuste, tabFaux, 32);
   for (int j = 0; j < 32; j++)
   {
      if (tabxor[j] == 1)
      {
         return j;
      }
   }
   return -1;
}


void Sboxfonc(int *resultat, int *entrer, int numSbox)
{
   int row = 0;
   int column = 0;
   row = entrer[0] * 2 + entrer[5];
   int i = 0, j = 4;
   while (j > 0)
   {
      if (entrer[j] != 0)
      {
         column += puissance(2, i);
      }
      i++;
      j--;
   }
   long resultat4bit = sbox[numSbox][row][column];
   hexatobinary(resultat, resultat4bit, 1);
}

void obtenirR16L16(long hexa, Message *m)
{
   m->chiffrerHexa = hexa;
   hexatobinary(m->chiffrerBinaire, hexa, 16);
   Permutation(m->chiffrerBinairePermuter, m->chiffrerBinaire, ip, 64);
   splitTab(m->chiffrerBinairePermuter, m->leftChiffrer, m->rightChiffrer, 32);
}

void extraire6Bits(Message *m, int position)
{
   for (int i = 0; i < 6; i++)
   {
      m->sbox6Bits[i] = m->rightChiffrerExp[6 * position + i];
   }
}

int egale(int *tab1, int *tab2, int nbrBit)
{
   for (int i = 0; i < nbrBit; i++)
   {
      if (tab1[i] != tab2[i])
         return 0;
   }
   return 1;
}

long k16enHexa(int tabK16[8][64])
{
   long resultat = 0;
   int tab[8] = {0};
   int tabclef[6] = {0};
   int tabresult[64] = {0};
   for (int i = 0; i < 8; i++)
   {
      for (int j = 0; j < 64; j++)
      {
         if (tabK16[i][j] == 6)
            tab[i] = j;
      }
      printf("%d\n", tab[i]);
      decimaltobinary(tabclef, tab[i], 6);
      for (int j = 0; j < 6; j++)
      {
         tabresult[i * 6 + j] = tabclef[j];
      }
   }
   resultat = TabtoLong(tabresult, 48);
   return resultat;
}

long rechercheExostive(const long LechiffrerJuste, const long *LeschiffrerFaux)
{
   Message juste;
   Message faux;
   long aRetourner = 0;
   int resultat[8][64] = {0};
   obtenirR16L16(LechiffrerJuste, &juste);
   int leftPmoin1[32] = {0};
   int resultatxorLeft[32] = {0};
   for (int w = 0; w < 32; w++)
   {
      obtenirR16L16(LeschiffrerFaux[w], &faux);
      xor(resultatxorLeft, juste.leftChiffrer, faux.leftChiffrer, 32);
      Permutation(leftPmoin1, resultatxorLeft, pMoin1, 32);
      int bitfaux = bitFauter(juste.rightChiffrer, faux.rightChiffrer);
      printf("bit faux : %d\n", bitfaux);
      Permutation(juste.rightChiffrerExp, juste.rightChiffrer, e, 48);
      Permutation(faux.rightChiffrerExp, faux.rightChiffrer, e, 48);
      int resSbox[4] = {0};
      int resLeftJuste[4] = {0};
      int key[6] = {0};
      for (int i = 0; i < 48; i++)
      {
         if (e[i] == (bitfaux + 1))
         {
            extraire6Bits(&juste, i / 6);
            extraire6Bits(&faux, i / 6);
            for (int y = 0; y < 4; y++)
            {
               resLeftJuste[y] = leftPmoin1[4 * (i / 6) + y];
            }
            for (int j = 0; j < 64; j++)
            {
               decimaltobinary(key, j, 6);
               xor(juste.sbox6BitsXorer, juste.sbox6Bits, key, 6);
               decimaltobinary(key, j, 6);
               xor(faux.sbox6BitsXorer, faux.sbox6Bits, key, 6);
               Sboxfonc(juste.sbox4Bits, juste.sbox6BitsXorer, i / 6);
               Sboxfonc(faux.sbox4Bits, faux.sbox6BitsXorer, i / 6);
               xor(resSbox, juste.sbox4Bits, faux.sbox4Bits, 4);
               if (egale(resLeftJuste, resSbox, 4))
               {
                  resultat[i / 6][TabtoInt(key, 6)]++;
               }
            }
         }
      }
   }
   aRetourner = k16enHexa(resultat);
   return aRetourner;
}

void initTab(int *tab, int nbrBit)
{
   for (int i = 0; i < nbrBit; i++)
   {
      tab[i] = 0;
   }
}

void shiftGauche(int *resultat, int *tabAshifter, int nbrShift, int nbrBit)
{
   for (int i = -nbrShift; i < nbrBit - nbrShift; i++)
   {
      if (i < 0)
      {
         resultat[i + nbrBit] = tabAshifter[i + nbrShift];
      }
      else
      {
         resultat[i] = tabAshifter[i + nbrShift];
      }
   }
}

void fusionTab(int *resultat, int *leftTab, int *rightTab, int nbrBit)
{
   for (int i = 0; i < nbrBit; i++)
   {
      resultat[i] = leftTab[i];
      resultat[i + nbrBit] = rightTab[i];
   }
}

void generationSubKey(int Les16SubKey[][48], int *key64Bit)
{
   int v[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
   int key56bit[56] = {0};
   //int key48bit[48] = {0};
   int tabSplit[2][28] = {0};
   int tabSplitRes[2][28] = {0};
   Permutation(key56bit, key64Bit, pc1, 56);
   for (int i = 0; i < 16; i++)
   {
      splitTab(key56bit, tabSplit[0], tabSplit[1], 28);
      shiftGauche(tabSplitRes[0], tabSplit[0], v[i], 28);
      shiftGauche(tabSplitRes[1], tabSplit[1], v[i], 28);
      fusionTab(key56bit, tabSplitRes[0], tabSplitRes[1], 28);
      Permutation(Les16SubKey[i], key56bit, pc2, 48);
   }
}

void f(int *resultat, int *Ri, int *Ki)
{
   int right48b[48] = {0};
   int resultatXor[48] = {0};
   int entrerSbox[6] = {0};
   int sortiSbox[4] = {0};
   int sorti32bit[32] = {0};
   Permutation(right48b, Ri, e, 48);
   xor(resultatXor, right48b, Ki, 48);
   for (int j = 0; j < 8; j++)
   {
      for (int i = 0; i < 6; i++)
      {
         entrerSbox[i] = resultatXor[6 * j + i];
      }
      Sboxfonc(sortiSbox, entrerSbox, j);
      for (int i = 0; i < 4; i++)
      {
         sorti32bit[j * 4 + i] = sortiSbox[i];
      }
   }
   Permutation(resultat, sorti32bit, p, 32);
}

void copieTab(int *resultat, int *aCopier, int nbrBit)
{
   for (int i = 0; i < nbrBit; i++)
   {
      resultat[i] = aCopier[i];
   }
}

long fonctionDES(long claire, long k64)
{
   DES d;
   int resultatF[32] = {0};
   int resultatConcat[64] = {0};
   hexatobinary(d.claireBinaire, claire, 16);
   hexatobinary(d.key64Bit, k64, 16);
   Permutation(d.claireBinaireIp, d.claireBinaire, ip, 64);
   splitTab(d.claireBinaireIp, d.left32Bit, d.right32Bit, 32);
   generationSubKey(d.subKey, d.key64Bit);
   for (int i = 0; i < 16; i++)
   {
      copieTab(d.left32BitPlus1, d.right32Bit, 32);
      f(resultatF, d.right32Bit, d.subKey[i]);
      xor(d.right32BitPlus1, d.left32Bit, resultatF, 32);
      copieTab(d.left32Bit, d.left32BitPlus1, 32);
      copieTab(d.right32Bit, d.right32BitPlus1, 32);
   }
   fusionTab(resultatConcat, d.right32Bit, d.left32Bit, 32);
   Permutation(d.chiffrerBinaire, resultatConcat, ipMoin1, 64);
   return TabtoLong(d.chiffrerBinaire, 64);
}

long getK56bit(long claire, long chiffrer, long k16)
{
   Key k;
   initTab(k.key48bit, 48);
   initTab(k.key56bit, 56);
   initTab(k.key64bitb, 64);
   hexatobinary(k.key48bit, k16, 12);
   Permutation(k.key56bit, k.key48bit, pc2Moin1, 56);
   Permutation(k.key64bitb, k.key56bit, pc1Moin1, 64);
   int position8bit[8] = {14, 15, 19, 20, 51, 54, 58, 60};
   int i = 0;
   while (i < 256)
   {
      decimaltobinary(k.key8bit, i, 8);
      for (int j = 0; j < 8; j++)
      {
         k.key64bitb[position8bit[j] - 1] = k.key8bit[j];
      }
      long clef = TabtoLong(k.key64bitb, 64);
      if (chiffrer == fonctionDES(claire, clef))
      {
         return clef;
      }
      i++;
   }
   return 0;
}

long getK64bitParite(long claire, long chiffrer, long k16)
{
   int compteur = 0;
   int tabClefB[64] = {0};
   hexatobinary(tabClefB, getK56bit(claire, chiffrer, k16), 16);
   for (int i = 0; i < 64; i++)
   {
      printf("%d", tabClefB[i]);
      if ((i + 1) % 8 == 0)
         printf(" ");
   }
   printf("\n");
   for (int i = 1; i < 65; i++)
   {
      if ((i % 8) == 0)
      {
         if (compteur % 2 == 1)
         {
            tabClefB[i - 1] = 0;
         }
         else
         {
            tabClefB[i - 1] = 1;
         }
         compteur = 0;
      }
      else
      {
         compteur += tabClefB[i - 1];
      }
   }
   for (int i = 0; i < 64; i++)
   {
      printf("%d", tabClefB[i]);
      if ((i + 1) % 8 == 0)
         printf(" ");
   }
   printf("\n");
   return TabtoLong(tabClefB, 64);
}

int main()
{
   printf("[" GRN "start" RESET "]");
   printf("starting dfa-des\n");
   printf("[" YEL "lookingk16" RESET "]\n");
   long K_16 = rechercheExostive(chiffrerJuste, chiffrerFaux);
   printf("\n");
   printf("[" CYN "resultk16" RESET "]\n");
   printf("K_16 : %lx\n\n", K_16);
   printf("[" CYN "result64bits" RESET "]\n");
   printf("Clef k : %lx\n", getK64bitParite(messageClaire, chiffrerJuste, K_16));
   return 0;
}