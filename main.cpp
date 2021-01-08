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

#define MAX_N 3

constexpr int MASK(int x) { return ((1 << x) - 1); }
constexpr bool HAS_BIT(int M, int x) { return M & (1 << x); }
inline void SET_BIT(int &M, int x) { M |= (1 << x); }
inline void RESET_BIT(int &M, int x) { M &= ~(1 << x); }
constexpr int GRID(int i, int j, int n) {
  return ((i - 1) / n) * n + ((j - 1) / n) + 1;
}

// 以下索引均从1作为起始

const int n = 3;
vvi mat;
vi rows;
vi cols;
vi grids;

bool Next(int r, int c, int k, int *next_r, int *next_c, int *next_k) {
  for (int i = 1; i <= n * n; i++) {
    for (int j = 1; j <= n * n; j++) {
      if (i == r && j == c) continue;
      if (mat[i][j] == 0) {
        *next_r = i;
        *next_c = j;
        *next_k = GRID(i, j, n);
        return true;
      }
    }
  }
  return false;
}

bool Backtrack(int r, int c, int k) {
  int next_r, next_c, next_k;
  for (int i = 1; i <= n * n; i++) {
    if (!HAS_BIT(rows[r], i) && !HAS_BIT(cols[c], i) && !HAS_BIT(grids[k], i)) {
      SET_BIT(rows[r], i);
      SET_BIT(cols[c], i);
      SET_BIT(grids[k], i);
      mat[r][c] = i;
      if (!Next(r, c, k, &next_r, &next_c, &next_k)) return true;
      // printf("# %d %d %d, %d %d %d: %d\n", r, c, k, next_r, next_c, next_k, i);
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

    int square = n * n;
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
