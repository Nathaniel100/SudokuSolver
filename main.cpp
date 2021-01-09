// ACCEPTED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <algorithm>
#include <queue>
#include <vector>

using namespace std;

using vi = vector<int>;
using vvi = vector<vi>;
using ii = pair<int, int>;

#define MAX_N 3

constexpr int LS_ONE(int x) { return x & (-x); }
constexpr int MASK(int x) { return ((1 << x) - 1); }
constexpr bool HAS_BIT(int M, int x) { return M & (1 << x); }
inline void SET_BIT(int &M, int x) { M |= (1 << x); }
inline void RESET_BIT(int &M, int x) { M &= ~(1 << x); }
constexpr int GRID(int i, int j, int n) {
  return ((i - 1) / n) * n + ((j - 1) / n) + 1;
}
inline int BIT_ONE_COUNT(int x) {
  int c = 0;
  while (x) {
    x -= LS_ONE(x);
    c++;
  }
  return c;
}

// 以下索引均从1作为起始

const int n = 3;
int square; // n*n
int sudoku_mask; // MASK(square+1)
vvi mat;
vi rows;
vi cols;
vi grids;

// 返回1表示找到下一个单元格，0表示全部单元格已填完，-1表示某个单元格存在冲突
int Next(int r, int c, int *next_r, int *next_c, int *next_k) {
  int r2 = 0, c2 = 0, k2 = 0;
  int min_ones = square + 2;

  for (int i = 1; i <= square; i++) {
    for (int j = 1; j <= square; j++) {
      if (i == r && j == c) continue;
      if (mat[i][j] != 0) continue;
      int g = GRID(i, j, n);
      int bitmask = sudoku_mask & ~(rows[i] | cols[j] | grids[g] | 1);
      if (bitmask == 0) return -1; // 冲突了
      int ones = BIT_ONE_COUNT(bitmask);
      if (ones == 1) {
        *next_r = i;
        *next_c = j;
        *next_k = g;
        return 1;
      }
      if (ones < min_ones) {
        min_ones = ones;
        r2 = i;
        c2 = j;
        k2 = g;
      }
    }
  }
  if (r2 == 0) return 0;
  *next_r = r2;
  *next_c = c2;
  *next_k = k2;
  return 1;
}

bool Backtrack(int r, int c, int k) {
  int next_r, next_c, next_k;
  for (int i = 1; i <= square; i++) {
    if (!HAS_BIT(rows[r], i) && !HAS_BIT(cols[c], i) && !HAS_BIT(grids[k], i)) {
      SET_BIT(rows[r], i);
      SET_BIT(cols[c], i);
      SET_BIT(grids[k], i);
      mat[r][c] = i;
      int status = Next(r, c, &next_r, &next_c, &next_k);
      if (status == 0) { 
        return true;
      } else if (status == -1) {
        RESET_BIT(rows[r], i);
        RESET_BIT(cols[c], i);
        RESET_BIT(grids[k], i);
        mat[r][c] = 0;
        continue;
      }
      if (Backtrack(next_r, next_c, next_k)) {
        return true;
      }
      RESET_BIT(rows[r], i);
      RESET_BIT(cols[c], i);
      RESET_BIT(grids[k], i);
      mat[r][c] = 0;
    }
  }
  return false;
}

int main() {
  char line[100] = {0}; // 每行81个字符
  int no = 1;
  while (fgets(line, sizeof(line), stdin)) {
    int line_len = strlen(line);
    if (line_len == 0) continue;
    while (isspace(line[line_len - 1])) line_len--;
    line[line_len] = 0;

    square = n * n;
    sudoku_mask = MASK(square + 1);
    mat.assign(square + 2, vi(square + 2, 0));
    rows.assign(square + 2, 1);
    cols.assign(square + 2, 1);
    grids.assign(square + 2, 1);
    int r = 0, c = 0, k = 0;
    for (int i = 1; i <= square; i++) {
      for (int j = 1; j <= square; j++) {
        int val = line[(i - 1) * square + j - 1] - '0';
        mat[i][j] = val;
        int g = GRID(i, j, n);
        SET_BIT(rows[i], val);
        SET_BIT(cols[j], val);
        SET_BIT(grids[g], val);
        if (val == 0 && r == 0) {
          r = i, c = j, k = g;
        }
      }
    }
    if (no > 1) printf("\n");
    if (Backtrack(r, c, k) || r == 0) {
      for (int i = 1; i <= square; i++) {
        for (int j = 1; j <= square; j++) {
          if (j == square) {
            printf("%d\n", mat[i][j]);
          } else {
            printf("%d ", mat[i][j]);
          }
        }
      }
    } else {
      printf("NO SOLUTION\n");
    }
    no++;
  }
}
