#include "transpiler.h"

#include <iostream>

using namespace std;

string transpileBinary(SExpr* expr, string op){
    SExpr* left = car(cdr(expr));
    SExpr* right = car(cdr(cdr(expr)));

    return "(" + transpile(left) + " " + op + " " + transpile(right) + ")";
}

string transpileQuotedList(SExpr* list){
    string result = "[";

    while (!isNil(list)){
        SExpr* value = car(list);

        if (isAtom(value)){
            result += "\"" + value->atom + "\"";
        }
        else {
            result += transpileQuotedList(value);
        }

        list = cdr(list);

        if (!isNil(list)){
            result += ", ";
        }
    }

    result += "]";

    return result;
}

string transpile(SExpr* expr){
    if (isAtom(expr)){
        return expr->atom;
    }

    SExpr* op = car(expr);

    if (op->atom == "add"){
        return transpileBinary(expr, "+");
    }

    if (op->atom == "sub"){
        return transpileBinary(expr, "-");
    }

    if (op->atom == "mul"){
        return transpileBinary(expr, "*");
    }

    if (op->atom == "div"){
        return transpileBinary(expr, "//");
    }

    if (op->atom == "rem"){
        return transpileBinary(expr, "%");
    }

    if (op->atom == "lt"){
        return transpileBinary(expr, "<");
    }

    if (op->atom == "eq?"){
        return transpileBinary(expr, "==");
    }

    if (op->atom == "and?"){
        return transpileBinary(expr, "and");
    }

    if (op->atom == "or?"){
        return transpileBinary(expr, "or");
    }

    if (op->atom == "not?"){
        SExpr* arg = car(cdr(expr));

        return "(not " + transpile(arg) + ")";
    }

    if (op->atom == "if"){
        SExpr* condition = car(cdr(expr));
        SExpr* trueValue = car(cdr(cdr(expr)));
        SExpr* falseValue = car(cdr(cdr(cdr(expr))));

        return "(" + transpile(trueValue) + " if " + transpile(condition) +
            " else " + transpile(falseValue) + ")";
    }

    if (op->atom == "set"){
        SExpr* name = car(cdr(expr));
        SExpr* value = car(cdr(cdr(expr)));

        return name->atom + " = " + transpile(value);
    }

    if (op->atom == "quote"){
        SExpr* value = car(cdr(expr));

        if (isAtom(value)){
            return "\"" + value->atom + "\"";
        }
        
        return transpileQuotedList(value);
    }

    if (op->atom == "car"){
        SExpr* list = car(cdr(expr));

        return transpile(list) + "[0]";
    }

    if (op->atom == "cdr"){
        SExpr* list = car(cdr(expr));

        return transpile(list) + "[1:]";
    }

    if (op->atom == "cons"){
        SExpr* first = car(cdr(expr));
        SExpr* rest = car(cdr(cdr(expr)));

        return "([" + transpile(first) + "] + " + transpile(rest) + ")";
    }

        return "";
    }

void transpileLoop(){

    while (cin)
    {
        skipWhitespace();

        if (cin.peek() == EOF)
        {
            break;
        }

        SExpr* expr = readExpr();

        cout << transpile(expr) << endl;
    }
}
