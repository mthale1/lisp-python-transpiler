#ifndef TRANSPILER_H
#define TRANSPILER_H

#include <string>
#include "sexpr.h"

std::string transpile(SExpr* expr);
std::string transpileBinary(SExpr* expr, std::string op);
std::string transpileQuotedList(SExpr* list);
void transpileLoop();

#endif