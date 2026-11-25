#ifndef TRANSPILER_H
#define TRANSPILER_H

#include <string>
#include "sexpr.h"

std::string transpile(SExpr* expr);
std::string transpileBinary(SExpr* expr, std::string op);
std::string transpileQuotedList(SExpr* list);
std::string transpileCond(SExpr* clauses);
std::string transpileParameters(SExpr* params);
int executePythonFile(std::string pythonFile, std::string resultFile);
void transpileLoop();
bool transpileFile(std::string inputFile, std::string outputFile);

#endif