//
// Created by 吴凡 on 2018/8/13.
//

#ifndef SUDOKUSOLVER_SODOKU_H
#define SUDOKUSOLVER_SODOKU_H

#include <cstring>
#include <cstdio>
#include <sstream>
#include <stack>

namespace sudoku {

// if possible value is 6 (0110), the possible value is 1, 2
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

enum SolveState {
  CONFLICTED = -1,
  NO = 0,
  SOLVED = 1
};

class Sudoku {
 public:
  constexpr static int kRows = 9;
  constexpr static int kCols = 9;
  constexpr static int kBlocks = 9;
  constexpr static int kBlockRows = 3;
  constexpr static int kBlockCols = 3;
  constexpr static int kNums = kRows * kCols;
 public:
  // 530070000600195000098000060800060003400803001700020006060000280000319005000080079
  explicit Sudoku(const char *str) {
    int values[kNums] = {0};
    for (int i = 0; i < kNums; i++) {
      if (str[i] >= '1' && str[i] <= '9') {
        values[i] = str[i] - '0';
      } else {
        values[i] = 0;
      }
    }
    Init(values);
  }

  explicit Sudoku(const int *values) {
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

  std::string FormatOriginString() {
    return Format(origin_values_matrix_);
  }

  std::string FormatString() {
    return Format(values_matrix_);
  }

  std::string FormatPossibleString() {
    return Format(possible_values_matrix_);
  }

  void PrintOrigin() {
    printf("%s", FormatOriginString().c_str());
  }

  void Print() {
    printf("%s", FormatString().c_str());
//    printf("#################\n");
//    printf("%s", FormatPossibleString().c_str());
  }

  void Solve() {
    SolveState solve_state = SolveByPossible();
    if (solve_state == SOLVED) { // 已解决
      return;
    } else if (solve_state == CONFLICTED) { // 出现冲突，说明数独题有问题
      return;
    }
    // 以上基本可以解决简单的

    // 使用回溯法解决
    SolveByBacktrack();
  }

 private:
  // 从最小可能possbile_value开始
  void SolveByBacktrack() {
    if (IsSolved()) {
      return;
    }
    int backtrack_value = GetBacktrackStart();
    while (true) {
      SaveSnapshot(backtrack_value);
      int index = SnapshotIndex(backtrack_value);
      int guess = SnapshotGuessValue(backtrack_value);
      values_[index] = guess;
      SolveState state = SolveByPossible();
      if (state == SOLVED) {
        ClearSnapshot();
        break;
      } else if (state == CONFLICTED) {
        do {
          RestoreSnapshot(&backtrack_value);
          backtrack_value = AnotherPossibleBacktrack(backtrack_value);
        } while (backtrack_value == -1);
      } else {
        backtrack_value = GetBacktrackStart();
      }
    }
  }

  // 我们从可能的值中选一个可能值最少的
  //       16  15  10  9        0
  // XXXXXXXX  XXXXXX  XXXXXXXXXX
  // index  guess  possible_value
  int GetBacktrackStart() {
    int min_ones_count = kRows;
    int min_index = 0;
    for (int i = 0; i < kNums; i++) {
      if (possible_values_[i] == 0) {
        continue;
      }
      int ones_count = Ones(possible_values_[i], nullptr);
      if (min_ones_count > ones_count) {
        min_ones_count = ones_count;
        min_index = i;
      }
    }
    int value = 0;
    int first_one_bit = FirstOneBit(possible_values_[min_index]);
    value |= ClearBit(possible_values_[min_index], first_one_bit);
    value |= (first_one_bit << 10);
    value |= (min_index << 16);
    return value;
  }

  inline int AnotherPossibleBacktrack(int value) {
    int possible_value = (value & kMask);
    int guess = (value >> 10) & 0x1F;
    int index = (value >> 16) & 0xFF;
    value = 0;
    int new_possible_value = ClearBit(possible_value, guess);
    if (new_possible_value == 0) {
      return -1;
    }
    int first_one_bit = FirstOneBit(new_possible_value);
    value |= ClearBit(new_possible_value, first_one_bit);
    value |= (first_one_bit << 10);
    value |= (index << 16);
    return value;
  }

  inline int SnapshotIndex(int value) {
    return (value >> 16) & 0xFF;
  }

  inline int SnapshotGuessValue(int value) {
    return (value >> 10) & 0x1F;
  }

  void SaveSnapshot(int backtrack_value) {
    snapshot_stack_.push(Snapshot(values_, possible_values_, backtrack_value));
  }

  void ClearSnapshot() {
    while (!snapshot_stack_.empty()) {
      snapshot_stack_.pop();
    }
  }

  void RestoreSnapshot(int *backtrack_value) {
    snapshot_stack_.top().Restore(values_, possible_values_, backtrack_value);
    snapshot_stack_.pop();
  }

  // 以下方法基本可以解决Easy的问题
  SolveState SolveByPossible() {
    int iteration = GetIteration();
    for (int i = 0; i < iteration; i++) {
      if (!UpdateValues()) { // 出现冲突
        return CONFLICTED;
      }
      if (!UpdatePossibleValues()) { // 出现冲突
        return CONFLICTED;
      }
    }
    return IsSolved() ? SOLVED : NO;
  }

  static std::string Format(int (*v)[kCols]) {
    std::ostringstream oss;
    for (int i = 0; i < kRows; i++) {
      for (int j = 0; j < kCols; j++) {
        oss << v[i][j];
        if (j != kCols - 1) {
          oss << ' ';
        }
      }
      oss << '\n';
    }
    return oss.str();
  }

  void Init(const int *values) {
    origin_values_ = reinterpret_cast<int *>(origin_values_matrix_);
    values_ = reinterpret_cast<int *>(values_matrix_);
    possible_values_ = reinterpret_cast<int *>(possible_values_matrix_);
    memcpy(origin_values_, values, sizeof(int) * kNums);
    memcpy(values_, values, sizeof(int) * kNums);
    UpdatePossibleValues();
  }

  int GetIteration() {
    int iteration = 0;
    for (int i = 0; i < kNums; i++) {
      if (values_[i] != 0) {
        iteration++;
      }
    }
    return iteration;
  }

  bool UpdateValues() {
    for (int i = 0; i < kRows; i++) {
      for (int j = 0; j < kCols; j++) {
        int bit;
        if (Ones(possible_values_matrix_[i][j], &bit) == 1) {
          if (!CheckRowValue(i, bit)) {
            return false;
          }
          if (!CheckColValue(j, bit)) {
            return false;
          }
          int block_row_col = BlockRowCol(i, j);
          int block = Block(block_row_col);
          if (!CheckBlockValue(block, bit)) {
            return false;
          }
          values_matrix_[i][j] = bit;
        }
      }
    }
    return true;
  }

  bool CheckRowValue(int row, int value) {
    for (int j = 0; j < kCols; j++) {
      if (values_matrix_[row][j] == value) {
        return false;
      }
    }
    return true;
  }

  bool CheckColValue(int col, int value) {
    for (int i = 0; i < kRows; i++) {
      if (values_matrix_[i][col] == value) {
        return false;
      }
    }
    return true;
  }

  bool CheckBlockValue(int block, int value) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if (value == Value(block, i, j)) {
          return false;
        }
      }
    }
    return true;
  }

  bool UpdatePossibleValues() {
    for (int i = 0; i < kRows; i++) {
      for (int j = 0; j < kCols; j++) {
        int possible_value = CalculatePossibleValue(i, j);
        if (possible_value == -1) {
          return false;
        }
        possible_values_matrix_[i][j] = possible_value;
      }
    }
    return true;
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
    int block_row_col = BlockRowCol(row, col);
    int block = Block(block_row_col);

    int possible_value = kMask;
    CalculateRowPossibleValue(row, &possible_value);
    CalculateColPossibleValue(col, &possible_value);
    CalculateBlockPossibleValue(block, &possible_value);

    if (possible_value == 0) { // Conflict
      return -1;
    }

    return possible_value;
  }

  void CalculateRowPossibleValue(int row, int *possible_value) {
    for (int j = 0; j < kCols; j++) {
      if (values_matrix_[row][j] != 0) {
        *possible_value = ClearBit(*possible_value, values_matrix_[row][j]);
      }
    }
  }

  void CalculateColPossibleValue(int col, int *possible_value) {
    for (int i = 0; i < kRows; i++) {
      if (values_matrix_[i][col] != 0) {
        *possible_value = ClearBit(*possible_value, values_matrix_[i][col]);
      }
    }
  }

  void CalculateBlockPossibleValue(int block, int *possible_value) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        int value = Value(block, i, j);
        if (value != 0) {
          *possible_value = ClearBit(*possible_value, value);
        }
      }
    }
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

  inline int FirstOneBit(int value) {
    int bit = kRows;
    do {
      if (HasBit(value, bit)) {
        break;
      }
    } while (--bit != 0);
    return bit;
  }

 private:
  class Snapshot {
   private:
    int snap_values_[kRows * kCols];
    int snap_possible_values_[kRows * kCols];
    int snap_backtrack_value_;
   public:
    Snapshot(int *values, int *possible_values, int backtrack_value) {
      memcpy(snap_values_, values, sizeof(snap_values_));
      memcpy(snap_possible_values_, possible_values, sizeof(snap_possible_values_));
      snap_backtrack_value_ = backtrack_value;
    }

    void Restore(int *values, int *possible_values, int *backtrack_value) {
      memcpy(values, snap_values_, sizeof(snap_values_));
      memcpy(possible_values, snap_possible_values_, sizeof(snap_possible_values_));
      *backtrack_value = snap_backtrack_value_;
    }
  };
 private:
  int *origin_values_;
  int *values_;
  int *possible_values_;
  int origin_values_matrix_[kRows][kCols];
  int values_matrix_[kRows][kCols];
  int possible_values_matrix_[kRows][kCols];
  std::stack<Snapshot> snapshot_stack_;
};

}

#endif //SUDOKUSOLVER_SODOKU_H
