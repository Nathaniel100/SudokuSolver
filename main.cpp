#include <iostream>
#include <fstream>
#include "sudoku.h"

void SolvePuzzles(const std::string &in_file, const std::string &out_file) {
  std::ifstream in(in_file.c_str(), std::ios_base::in);
  if (!in.good()) {
    fprintf(stderr, "In file path is invalid: %s\n", in_file.c_str());
    return;
  }
  std::ofstream out(out_file.c_str(), std::ios_base::out);
  if (!out.good()) {
    fprintf(stderr, "Out file path is invalid: %s\n", out_file.c_str());
    return;
  }
  char buffer[82] = {0};
  while (in.getline(buffer, 82)) {
    sudoku::Sudoku sudoku(buffer);
    sudoku.Solve();
    std::string format_string = sudoku.FormatString();
    out.write(format_string.data(), format_string.size());
    out.write("\n\n", 2);
  }
  out.flush();
}

void SolveSimplePuzzle() {
// easy
//  sudoku::Sudoku sudoku("530070000"
//                               "600195000"
//                               "098000060"
//                               "800060003"
//                               "400803001"
//                               "700020006"
//                               "060000280"
//                               "000419005"
//                               "000080079");

// easy
//  sudoku::Sudoku sudoku("501943000"
//                               "200008600"
//                               "090006500"
//                               "300000472"
//                               "620804039"
//                               "419000006"
//                               "002100060"
//                               "008200004"
//                               "000489705");
// medium
//  sudoku::Sudoku sudoku("006100002"
//                               "000035100"
//                               "591000030"
//                               "700003001"
//                               "080706000"
//                               "010000074"
//                               "000020690"
//                               "408060700"
//                               "063501000");
// Very Very Difficult (From wiki)
//  sudoku::Sudoku sudoku("000000000"
//                               "000003085"
//                               "001020000"
//                               "000507000"
//                               "004000100"
//                               "090000000"
//                               "500000073"
//                               "002010000"
//                               "000040009");

//  sudoku::Sudoku sudoku("000000000"
//                               "700001602"
//                               "006470100"
//                               "500004300"
//                               "300807006"
//                               "002300009"
//                               "003082900"
//                               "104600005"
//                               "000000000");
//  sudoku::Sudoku sudoku("400000805030000000000700000020000060000080400000010000000603070500200000104000000");
//  sudoku::Sudoku sudoku("520006000000000701300000000000400800600000050000000000041800000000030020008700000");
//  sudoku::Sudoku sudoku("300080000000700005100000000000000360002004000070000000000060130045200000000000800");
  sudoku::Sudoku sudoku("000070020800000006010205000905400008000000000300008501000302080400000009070060000");
  sudoku.PrintOrigin();
  printf("=================\n");
  sudoku.Solve();
  sudoku.Print();
}

int main() {
  SolveSimplePuzzle();
  SolvePuzzles("../hardest-11.txt", "../hardest-11-solution.txt");
  // difficult-95 is slow, it will cost about 3 minutes.
//  SolvePuzzles("../difficult-95.txt", "../difficult-95-solution.txt");
  return 0;
}


