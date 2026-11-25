#include "transpiler.h"

#include <iostream>
#include <fstream>

using namespace std;

int executePythonFile(string pythonFile, string resultFile){
    string command = "python3 \"" + pythonFile + "\" > \"" + resultFile + "\" 2>&1";

    return system(command.c_str());
}

bool transpileFile(string inputFile, string outputFile){
    ifstream input(inputFile);

    if (!input.is_open()){
        return false;
    }

    ofstream output(outputFile);

    if (!output.is_open()){
        return false;
    }

    streambuf* originalCin = cin.rdbuf(input.rdbuf());

    while (cin){
        skipWhitespace();

        if (cin.peek() == EOF){
            break;
        }

        SExpr* expr = readExpr();
        string translated = transpile(expr);

        if (isAtom(expr) || isNil(expr)){
            output << "print(" << translated << ")" << endl << endl;
        }
        else if (isAtom(car(expr))){
            string op = car(expr)->atom;

            if (op == "set" || op == "def"){
                output << translated << endl << endl;
            }
            else {
                output << "print(" << translated << ")" << endl << endl;
            }
        }
    }

    cin.rdbuf(originalCin);

    return true;
}

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
            if (isNumber(value)){
                result += value->atom;
            }
            else {
                result += "\"" + value->atom + "\"";
            }
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

string transpileCond(SExpr* clauses){
    SExpr* condition = car(clauses);
    SExpr* result = car(cdr(clauses));
    SExpr* rest = cdr(cdr(clauses));

    if (isNil(rest)){
        return "(" + transpile(result) + " if " +
            transpile(condition) + " != [] else [])";
    }

    return "(" + transpile(result) + " if " + transpile(condition) +
        " != [] else " + transpileCond(rest) + ")";
}

string transpileParameters(SExpr* params){
    string result = "";

    while (!isNil(params)){
        SExpr* param = car(params);

        result += param->atom;
        params = cdr(params);

        if (!isNil(params)){
            result += ", ";
        }
    }

    return result;
}

string transpile(SExpr* expr){
    if (isNil(expr)){
        return "[]";
    }

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
        SExpr* left = car(cdr(expr));
        SExpr* right = car(cdr(cdr(expr)));

        return "(True if " + transpile(left) + " < " +
            transpile(right) + " else [])";
    }

    if (op->atom == "eq?"){
        SExpr* left = car(cdr(expr));
        SExpr* right = car(cdr(cdr(expr)));

        string leftValue = transpile(left);
        string rightValue = transpile(right);

        return "(True if not isinstance(" + leftValue + ", list) and " +
            "not isinstance(" + rightValue + ", list) and " +
            leftValue + " == " + rightValue + " else [])";
    }

    if (op->atom == "and?"){
        SExpr* left = car(cdr(expr));
        SExpr* right = car(cdr(cdr(expr)));

        return "(" + transpile(right) + " if " + transpile(left) +
            " != [] else [])";
    }

    if (op->atom == "or?"){
        SExpr* left = car(cdr(expr));
        SExpr* right = car(cdr(cdr(expr)));

        return "(" + transpile(left) + " if " + transpile(left) +
            " != [] else " + transpile(right) + ")";
    }

    if (op->atom == "not?"){
        SExpr* arg = car(cdr(expr));

        return "(True if " + transpile(arg) + " == [] else [])";
    }

    if (op->atom == "if"){
        SExpr* condition = car(cdr(expr));
        SExpr* trueValue = car(cdr(cdr(expr)));
        SExpr* falseValue = car(cdr(cdr(cdr(expr))));

        return "(" + transpile(trueValue) + " if " + transpile(condition) +
            " != [] else " + transpile(falseValue) + ")";
    }

    if (op->atom == "set"){
        SExpr* name = car(cdr(expr));
        SExpr* value = car(cdr(cdr(expr)));

        return name->atom + " = " + transpile(value);
    }

    if (op->atom == "quote"){
        SExpr* value = car(cdr(expr));

        if (isAtom(value)){
            if (isNumber(value)){
                return value->atom;
            }

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

        return "[" + transpile(first) + "] + " + transpile(rest);
    }

    if (op->atom == "nil?"){
        SExpr* value = car(cdr(expr));

        return "(True if " + transpile(value) + " == [] else [])";
    }

    if (op->atom == "atom?"){
        SExpr* value = car(cdr(expr));

        return "(True if not isinstance(" + transpile(value) +
            ", list) else [])";
    }

    if (op->atom == "list?"){
        SExpr* value = car(cdr(expr));

        return "(True if isinstance(" + transpile(value) +
            ", list) else [])";
    }

    if (op->atom == "number?"){
        SExpr* value = car(cdr(expr));

        return "(True if isinstance(" + transpile(value) +
            ", int) else [])";
    }

    if (op->atom == "cond"){
        SExpr* clauses = car(cdr(expr));

        return transpileCond(clauses);
    }

    if (op->atom == "def"){
        SExpr* name = car(cdr(expr));
        SExpr* params = car(cdr(cdr(expr)));
        SExpr* body = car(cdr(cdr(cdr(expr))));

        return "def " + name->atom + "(" + transpileParameters(params) +
            "):\n    return " + transpile(body);
    }

    if (op->atom == "function"){
        SExpr* params = car(cdr(expr));
        SExpr* body = car(cdr(cdr(expr)));

        return "lambda " + transpileParameters(params) +
            ": " + transpile(body);
    }

    string result = op->atom + "(";
    SExpr* args = cdr(expr);

    while (!isNil(args)){
        result += transpile(car(args));
        args = cdr(args);

        if (!isNil(args)){
            result += ", ";
        }
    }

    result += ")";

    return result;
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
