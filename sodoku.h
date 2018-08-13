//
// Created by 吴凡 on 2018/8/13.
//

#ifndef SODOKUSOLVER_SODOKU_H
#define SODOKUSOLVER_SODOKU_H

#include <cstring>
#include <cstdio>
namespace sodoku_solver {

// value is 0, 1, 2, 3, 4, 5, 6, 7, 8

// Binary presentation of a possible value is value
// if possible value is 7 (0111), the possible value is 0, 1, 2
enum PossibleValue {
  kNo = 0,
  kOne = 1 << 1,
  kTwo = 1 << 2,
  kThree = 1 << 3,
  kFour = 1 << 4,
  kFive = 1 << 5,
  kSix = 1 << 6,
  kSeven = 1 << 7,
  kEight = 1 << 8,
  kNight = 1 << 9,
  kMask = (1 << 10) - 2,
};

class Sodoku {
 public:
  constexpr static int kRows = 9;
  constexpr static int kCols = 9;
  constexpr static int kBlocks = 9;
  constexpr static int kBlockRows = 3;
  constexpr static int kBlockCols = 3;
  constexpr static int kNums = kRows * kCols;
 public:
  // 530070000600195000098000060800060003400803001700020006060000280000319005000080079
  explicit Sodoku(const char *str) {
    int values[kNums] = {0};
    for (int i = 0; i < kNums; i++) {
      values[i] = str[i] - '0';
    }
    Init(values);
  }
  explicit Sodoku(const int *values) {
    Init(values);
  }
  bool IsSolved() {
    for (int i = 0; i < kNums; i++) {
      if (values_[i] == 0) {
        return false;
      }
    }
    return true;
  }
  void PrintOrigin() {
    for (int i = 0; i < kRows; i++) {
      for (int j = 0; j < kCols; j++) {
        if (j != kCols - 1) {
          printf("%d ", origin_values_matrix_[i][j]);
        } else {
          printf("%d", origin_values_matrix_[i][j]);
        }
      }
      printf("\n");
    }

  }
  void Print() {
    for (int i = 0; i < kRows; i++) {
      for (int j = 0; j < kCols; j++) {
        if (j != kCols - 1) {
          printf("%d ", values_matrix_[i][j]);
        } else {
          printf("%d", values_matrix_[i][j]);
        }
      }
      printf("\n");
    }

    for (int i = 0; i < kRows; i++) {
      for (int j = 0; j < kCols; j++) {
        printf("possible[%d][%d]: ", i, j);
        for (int k = 1; k <= kRows; k++) {
          if (HasBit(possible_values_matrix_[i][j], k)) {
            printf("%d ", k);
          }
        }
        printf("\n");
      }
    }
  }

  void Solve() {
    for (int i = 0; i < kNums; i++) {// 假设每次都能解决一个空格
      for (int k = 0; k < kNums; k++) {
        int bit;
        if (Ones(possible_values_[k], &bit) == 1) {
          values_[k] = bit;
        }
      }
      if (IsSolved()) {
        break;
      }
      UpdatePossibleValues();
    }
    // 以上方法基本可以解决Easy的问题

    // 使用回溯法解决
  }

  void SolveByBackstrack() {
  }
 private:
  void Init(const int *values) {
    origin_values_ = reinterpret_cast<int *>(origin_values_matrix_);
    values_ = reinterpret_cast<int *>(values_matrix_);
    possible_values_ = reinterpret_cast<int *>(possible_values_matrix_);
    memcpy(origin_values_, values, sizeof(int) * kNums);
    memcpy(values_, values, sizeof(int) * kNums);
    UpdatePossibleValues();
  }
  void UpdatePossibleValues() {
    for (int i = 0; i < kRows; i++) {
      for (int j = 0; j < kCols; j++) {
        possible_values_matrix_[i][j] = CalculatePossibleValue(i, j);
      }
    }
  }

  // 3, 3 -> block 3, row 0, col 0
  inline int BlockRowCol(int row, int col) {
    int block = (row / kBlockRows) * kBlockCols + col / 3;
    int block_row = row % kBlockRows;
    int block_col = col % kBlockCols;
    return (block << 8) | (block_row << 4) | (block_col);
  }

  inline int Block(int block_row_col) {
    return (block_row_col >> 8) & 0x0F;
  }

  inline int BlockRow(int block_row_col) {
    return (block_row_col >> 4) & 0x0F;
  }

  inline int BlockCol(int block_row_col) {
    return (block_row_col & 0x0F);
  }

  inline int BlockToRow(int block, int block_row) {
    return (block / kBlockCols) * kBlockRows + block_row;
  }

  inline int BlockToCol(int block, int block_col) {
    return (block % kBlockCols) * kBlockCols + block_col;
  }


  inline int Value(int block, int block_row, int block_col) {
    int row = BlockToRow(block, block_row);
    int col = BlockToCol(block, block_col);
    return values_matrix_[row][col];
  }

  inline int PossibleValue(int block, int block_row, int block_col) {
    int row = BlockToRow(block, block_row);
    int col = BlockToCol(block, block_col);
    return possible_values_matrix_[row][col];
  }

  int CalculatePossibleValue(int row, int col) {
    if (values_matrix_[row][col] != 0) {
      return 0;
    }

    int possible_value = kMask;
    for (int j = 0; j < kCols; j++) {
      if (values_matrix_[row][j] != 0) {
        possible_value = ClearBit(possible_value, values_matrix_[row][j]);
      }
    }
    for (int i = 0; i < kRows; i++) {
      if (values_matrix_[i][col] != 0) {
        possible_value = ClearBit(possible_value, values_matrix_[i][col]);
      }
    }
    int block_row_col = BlockRowCol(row, col);
    int block = Block(block_row_col);
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int value = Value(block, i, j);
        if (value != 0) {
          possible_value = ClearBit(possible_value, value);
        }
      }
    }

    return possible_value;
  }

  inline int ClearBit(int value, int bit) {
    return (~(1 << bit) & value) & kMask;
  }

  inline int SetBit(int value, int bit) {
    return ((1 << bit) | value) & kMask;
  }

  inline bool HasBit(int value, int bit) {
    return ((1 << bit) & value) != 0;
  }

  inline int Ones(int value, int *bit_ptr) {
    int count = 0;
    int bit = kRows;
    do {
      if (HasBit(value, bit)) {
        count++;
        if (bit_ptr) {
          *bit_ptr = bit;
        }
      }
    } while (--bit != 0);
    return count;
  }

 private:
  int *origin_values_;
  int *values_;
  int *possible_values_;
  int origin_values_matrix_[kRows][kCols];
  int values_matrix_[kRows][kCols];
  int possible_values_matrix_[kRows][kCols];
};

}

#endif //SODOKUSOLVER_SODOKU_H
