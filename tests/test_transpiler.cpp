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

void testBasicValues(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------- TEST BASIC VALUES -----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("42");
    check(result == "42",
          "Transpiles atom",
          result,
          "42");

    result = transpileString("()");
    check(result == "[]",
          "Transpiles empty list",
          result,
          "[]");
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

void testCar(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST CAR ---------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(car '(a b c))");
    check(result == "[\"a\", \"b\", \"c\"][0]",
          "Transpiles car",
          result,
          "[\"a\", \"b\", \"c\"][0]");

    result = transpileString("(car '((a b) c d))");
    check(result == "[[\"a\", \"b\"], \"c\", \"d\"][0]",
          "Transpiles car with nested list",
          result,
          "[[\"a\", \"b\"], \"c\", \"d\"][0]");

    result = transpileString("(car x)");
    check(result == "x[0]",
          "Transpiles car with variable",
          result,
          "x[0]");
}

void testCdr(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST CDR ---------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(cdr '(a b c))");
    check(result == "[\"a\", \"b\", \"c\"][1:]",
          "Transpiles cdr",
          result,
          "[\"a\", \"b\", \"c\"][1:]");

    result = transpileString("(cdr '((a b) c d))");
    check(result == "[[\"a\", \"b\"], \"c\", \"d\"][1:]",
          "Transpiles cdr with nested list",
          result,
          "[[\"a\", \"b\"], \"c\", \"d\"][1:]");

    result = transpileString("(cdr x)");
    check(result == "x[1:]",
          "Transpiles cdr with variable",
          result,
          "x[1:]");
}

void testCons(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST CONS --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(cons 'a '(b c))");
    check(result == "([\"a\"] + [\"b\", \"c\"])",
        "Transpiles cons",
        result,
        "([\"a\"] + [\"b\", \"c\"])");

    result = transpileString("(cons x y)");
    check(result == "([x] + y)",
        "Transpiles cons with variables",
        result,
        "([x] + y)");

    result = transpileString("(cons (add 2 3) x)");
    check(result == "([(2 + 3)] + x)",
        "Transpiles cons with expression",
        result,
        "([(2 + 3)] + x)");
}

void testListOperations(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "--------------- TEST LIST OPERATIONS ----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(car (cdr '(a b c)))");
    check(result == "[\"a\", \"b\", \"c\"][1:][0]",
          "Transpiles car of cdr",
          result,
          "[\"a\", \"b\", \"c\"][1:][0]");

    result = transpileString("(cdr (cdr '(a b c d)))");
    check(result == "[\"a\", \"b\", \"c\", \"d\"][1:][1:]",
          "Transpiles nested cdr",
          result,
          "[\"a\", \"b\", \"c\", \"d\"][1:][1:]");

    result = transpileString("(car (cons 'a '(b c)))");
    check(result == "([\"a\"] + [\"b\", \"c\"])[0]",
          "Transpiles car of cons",
          result,
          "([\"a\"] + [\"b\", \"c\"])[0]");
}

void testNil(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST NIL ---------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(nil? ())");
    check(result == "([] == [])",
          "Transpiles nil? with empty list",
          result,
          "([] == [])");

    result = transpileString("(nil? x)");
    check(result == "(x == [])",
          "Transpiles nil? with variable",
          result,
          "(x == [])");

    result = transpileString("(nil? '(a b))");
    check(result == "([\"a\", \"b\"] == [])",
          "Transpiles nil? with list",
          result,
          "([\"a\", \"b\"] == [])");
}

void testAtom(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST ATOM --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(atom? x)");
    check(result == "(not isinstance(x, list))",
          "Transpiles atom? with variable",
          result,
          "(not isinstance(x, list))");

    result = transpileString("(atom? '(a b))");
    check(result == "(not isinstance([\"a\", \"b\"], list))",
          "Transpiles atom? with list",
          result,
          "(not isinstance([\"a\", \"b\"], list))");

    result = transpileString("(atom? ())");
    check(result == "(not isinstance([], list))",
          "Transpiles atom? with empty list",
          result,
          "(not isinstance([], list))");
}

void testList(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST LIST --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(list? x)");
    check(result == "isinstance(x, list)",
          "Transpiles list? with variable",
          result,
          "isinstance(x, list)");

    result = transpileString("(list? '(a b))");
    check(result == "isinstance([\"a\", \"b\"], list)",
          "Transpiles list? with list",
          result,
          "isinstance([\"a\", \"b\"], list)");

    result = transpileString("(list? ())");
    check(result == "isinstance([], list)",
          "Transpiles list? with empty list",
          result,
          "isinstance([], list)");
}

void testNumber(){
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "--------------------- TEST NUMBER -------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string result;

    result = transpileString("(number? x)");
    check(result == "isinstance(x, int)",
          "Transpiles number? with variable",
          result,
          "isinstance(x, int)");

    result = transpileString("(number? 42)");
    check(result == "isinstance(42, int)",
          "Transpiles number? with number",
          result,
          "isinstance(42, int)");

    result = transpileString("(number? '(a b))");
    check(result == "isinstance([\"a\", \"b\"], int)",
          "Transpiles number? with list",
          result,
          "isinstance([\"a\", \"b\"], int)");
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
    testBasicValues();
    testArithmetic();
    testComparisons();
    testBooleans();
    testConditionals();
    testVariables();
    testQuote();
    testCar();
    testCdr();
    testCons();
    testListOperations();
    testNil();
    testAtom();
    testList();
    testNumber();

    testReport();

    return 0;
}