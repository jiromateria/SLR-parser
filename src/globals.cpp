#include "globals.hpp"

SymbTable symbtable;

TokenValue yylval;

unsigned int symbID = 0;

void resetGlobals() {
    symbtable.clear();
    symbID = 0;
}
