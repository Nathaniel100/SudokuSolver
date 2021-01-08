# 数独

代码是基于C++实现的数独解答程序。

## 数据结构

将 9X9 的单元格分为3类，分别对应行，宽以及 3x3格。我们可以使用位去表示这3类的状态, 比如第一行已填充 1, 2, 5, 即第一行对应的状态值的二进制数据为 `0000100111 = (1<<1)|(1<<2)|(1<<5)|(1<<0)`, 第0位默认填1。我们可以使用3个整型数组去表示这3类的状态。每行、每列、每格的有效位最多9位, 加上1填充位，使用整型 `int` 即可。我们还需要把当前的每个小格数值记录下来, 一共 9X9 个。为方便计算，所有的索引均从1开始, 因此我们值的二维数组大小至少就是 10 X 10, 数据机构如下所示。

```c++
#include <vector>

using vi = vector<int>;
using vvi = vector<vi>;

const int n = 3; // n的平方表示一行，一列，一格的数量
vi rows, cols, grids; // 索引从1开始，数组大小至少为 n*n+1
vvi mat; // 数独数值，0表示未填数值, 1-9表示已填充数值, 大小至少为 (n*n+1) * (n*n+1)
```

## 算法实现

利用回溯法进行遍历, 由于效率问题，该方法遇到部分数组可能会较慢。还需要额外优化。

```c++
// 参数表示当前单元格所在行，列，格的索引
bool Backtrack(int r, int c, int k) {
  int next_r, next_c, next_k;
  for (int i = 1; i <= n * n; i++) {
    if (!HAS_BIT(rows[r], i) && !HAS_BIT(cols[c], i) && !HAS_BIT(grids[k], i)) {
      SET_BIT(rows[r], i);
      SET_BIT(cols[c], i);
      SET_BIT(grids[k], i);
      mat[r][c] = i;
      // 如果Next返回false，说明所有单元格均为填充完毕，返回true表示成功
      if (!Next(r, c, k, &next_r, &next_c, &next_k)) return true;
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

// 找到下一个待填充的单元格(值为0)
// TODO 待优化
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
```
