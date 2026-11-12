#include "../src/transpiler.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int testsPassed = 0;
int testsFailed = 0;

void check(bool condition, string test, string actual, string expected){
    if (condition){
        cout << "\n✅ PASS \t";
        testsPassed++;
    }
    else {
        cout << "\n❌ FAIL \t";
        testsFailed++;
    }

    cout << test << " | Actual: " << actual
         << " | Expected: " << expected << endl;
}

string transpileString(string input){
    istringstream testInput(input);
    streambuf* originalCin = cin.rdbuf(testInput.rdbuf());

    cin.clear();
    SExpr* expr = readExpr();

    cin.rdbuf(originalCin);
    cin.clear();

    return transpile(expr);
}

void testArithmetic(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "------------------- TEST ARITHMETIC -----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(add 2 3)");
    check(result == "(2 + 3)", "Transpiles add", result, "(2 + 3)");

    result = transpileString("(sub 10 4)");
    check(result == "(10 - 4)", "Transpiles sub", result, "(10 - 4)");

    result = transpileString("(mul 3 5)");
    check(result == "(3 * 5)", "Transpiles mul", result, "(3 * 5)");

    result = transpileString("(div 10 3)");
    check(result == "(10 // 3)", "Transpiles div", result, "(10 // 3)");

    result = transpileString("(rem 10 3)");
    check(result == "(10 % 3)", "Transpiles rem", result, "(10 % 3)");

    result = transpileString("(mul (add 2 3) (sub 10 4))");
    check(result == "((2 + 3) * (10 - 4))", "Transpiles nested arithmetic", result, "((2 + 3) * (10 - 4))");
}

void testComparisons(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "------------------ TEST COMPARISONS -----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(lt 2 5)");
    check(result == "(2 < 5)", "Transpiles lt", result, "(2 < 5)");

    result = transpileString("(eq? 5 5)");
    check(result == "(5 == 5)", "Transpiles eq?", result, "(5 == 5)");

    result = transpileString("(lt (add 2 3) (mul 2 4))");
    check(result == "((2 + 3) < (2 * 4))", "Transpiles comparison with nested expressions", result, "((2 + 3) < (2 * 4))");
}

void testBooleans(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "-------------------- TEST BOOLEANS ------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(and? (lt 2 5) (eq? 3 3))");
    check(result == "((2 < 5) and (3 == 3))", "Transpiles and?", result, "((2 < 5) and (3 == 3))");

    result = transpileString("(or? (eq? 2 3) (lt 2 5))");
    check(result == "((2 == 3) or (2 < 5))", "Transpiles or?", result, "((2 == 3) or (2 < 5))");

    result = transpileString("(not? (lt 5 2))");
    check(result == "(not (5 < 2))", "Transpiles not?", result, "(not (5 < 2))");
}

void testConditionals(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------- TEST CONDITIONALS -----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(if (lt 2 5) 10 20)");
    check(result == "(10 if (2 < 5) else 20)", "Transpiles if", result, "(10 if (2 < 5) else 20)");

    result = transpileString("(if (eq? 2 3) (add 1 2) (mul 3 4))");
    check(result == "((1 + 2) if (2 == 3) else (3 * 4))", "Transpiles if with expressions", result, "((1 + 2) if (2 == 3) else (3 * 4))");
}

void testVariables(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "------------------- TEST VARIABLES ------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(set x 10)");
    check(result == "x = 10", "Transpiles set", result, "x = 10");

    result = transpileString("(set x (add 2 3))");
    check(result == "x = (2 + 3)", "Transpiles set with an expression", result, "x = (2 + 3)");

    result = transpileString("(set y (mul (add 2 3) 4))");
    check(result == "y = ((2 + 3) * 4)", "Transpiles set with nested expressions", result, "y = ((2 + 3) * 4)");
}

void testQuote(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "--------------------- TEST QUOTE --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("'hello");
    check(result == "\"hello\"", "Transpiles a quoted atom", result, "\"hello\"");

    result = transpileString("'(a b c)");
    check(result == "[\"a\", \"b\", \"c\"]", "Transpiles a quoted list", result, "[\"a\", \"b\", \"c\"]");

    result = transpileString("'((a b) c d)");
    check(result == "[[\"a\", \"b\"], \"c\", \"d\"]", "Transpiles a nested quoted list", result, "[[\"a\", \"b\"], \"c\", \"d\"]");
}

void testReport(){
    cout << "\n\n===========================================================" << endl;
    cout << "                        TEST REPORT                        " << endl;
    cout << "===========================================================" << endl;
    cout << "✅ Passed: " << testsPassed << endl;
    cout << "❌ Failed: " << testsFailed << endl;
    cout << "Total:  " << testsPassed + testsFailed << endl;

    // if (testsFailed > 0){
    //     cout << "\nFailed Tests:" << endl;

    //     string lastTestFunction = "";

    //     for (TestFailure failure : failedTests){
    //         if (failure.testFunction != lastTestFunction){
    //             cout << "\n❌ " << failure.testFunction << endl;
    //             lastTestFunction = failure.testFunction;
    //         }

    //         cout << "   - Line " << failure.line << ": " << failure.test << endl;
    //     }
    // }
}

int main(){
    testArithmetic();
    testComparisons();
    testBooleans();
    testConditionals();
    testVariables();
    testQuote();

    testReport();

    return 0;
}