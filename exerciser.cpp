#include "exerciser.h"

#include <iostream>

#include "exerciser.h"
#include "query_funcs.h"

void exercise(pqxx::connection *C) {
  std::cout
      << "\n===================== EXERCISE() TESTS =====================\n";

  // Query1: example - show players who have MPG in [35, 40], no constraints for
  // other stats use_mpg = 1, min_mpg = 35, max_mpg = 40 use_ppg = 0, etc.
  std::cout << "\n--- QUERY1: Players with 35 <= MPG <= 40 ---\n";
  query1(C,
         /*use_mpg=*/1, /*min_mpg=*/35, /*max_mpg=*/40,
         /*use_ppg=*/0, 0, 0,
         /*use_rpg=*/0, 0, 0,
         /*use_apg=*/0, 0, 0,
         /*use_spg=*/0, 0.0, 0.0,
         /*use_bpg=*/0, 0.0, 0.0);

  // Query2: example - show teams with the color "Orange"
  // Adjust "Orange" to something you actually have in color.txt
  std::cout << "\n--- QUERY2: Teams that wear 'Orange' ---\n";
  query2(C, "Orange");

  // Query3: example - show players on team "Syracuse", sorted by PPG desc
  // Adjust "Syracuse" to match a team you have in team.txt
  std::cout << "\n--- QUERY3: Players on 'Syracuse', ordered by PPG desc ---\n";
  query3(C, "Syracuse");

  // Query4: example - players in state "NY" wearing color "Orange"
  // Adjust "NY"/"Orange" if your data uses different names
  std::cout << "\n--- QUERY4: Players in 'NY' wearing 'Orange' ---\n";
  query4(C, "NY", "Orange");

  // Query5: example - show players (and their teams) where the team has > 20
  // wins
  std::cout << "\n--- QUERY5: Players on teams with more than 20 wins ---\n";
  query5(C, 20);

  std::cout
      << "\n================== END EXERCISE() TESTS ====================\n\n";
}
