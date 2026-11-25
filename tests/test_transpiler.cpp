#include "../src/transpiler.h"

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

int testsPassed = 0;
int testsFailed = 0;

struct TestFailure {
    string testFunction;
    string test;
    int line;
};

vector<TestFailure> failedTests;
string currentTestFunction;

void printMultiline(string label, string text){
    string testIndent = "                ";
    string codeIndent(label.length(), ' ');

    cout << testIndent << label << "| ";

    for (int i = 0; i < text.length(); i++){
        cout << text[i];

        if (text[i] == '\n' && i + 1 < text.length()){
            cout << testIndent << codeIndent << "| ";
        }
    }
}

void check(bool condition, string test, string actual, string expected, int line){
    if (condition){
        cout << "\n✅ PASS \t";
        testsPassed++;
    }
    else {
        cout << "\n❌ FAIL \t";
        testsFailed++;

        failedTests.push_back({currentTestFunction, test, line});
    }

    cout << test;

    if (actual.find('\n') != string::npos || expected.find('\n') != string::npos){
        cout << endl;

        printMultiline("Actual:    ", actual);

        cout << endl;
        printMultiline("Expected:  ", expected);

        cout << endl;
    }
    else {
        cout << " | Actual: " << actual
             << " | Expected: " << expected << endl;
    }
}

#define check(condition, test, actual, expected) check(condition, test, actual, expected, __LINE__)

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
    currentTestFunction = "testBasicValues";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "------------------ TEST BASIC VALUES ----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("42");
    expected = "42";
    check(actual == expected, "Transpiles number", actual, expected);

    actual = transpileString("x");
    expected = "x";
    check(actual == expected, "Transpiles symbol", actual, expected);

    actual = transpileString("()");
    expected = "[]";
    check(actual == expected, "Transpiles nil", actual, expected);
}

void testArithmetic(){
    currentTestFunction = "testArithmetic";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "------------------- TEST ARITHMETIC -----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(add 2 3)");
    expected = "(2 + 3)";
    check(actual == expected, "Transpiles add", actual, expected);

    actual = transpileString("(sub 10 4)");
    expected = "(10 - 4)";
    check(actual == expected, "Transpiles sub", actual, expected);

    actual = transpileString("(mul 3 5)");
    expected = "(3 * 5)";
    check(actual == expected, "Transpiles mul", actual, expected);

    actual = transpileString("(div 10 3)");
    expected = "(10 // 3)";
    check(actual == expected, "Transpiles div", actual, expected);

    actual = transpileString("(rem 10 3)");
    expected = "(10 % 3)";
    check(actual == expected, "Transpiles rem", actual, expected);

    actual = transpileString("(mul (add 2 3) (sub 10 4))");
    expected = "((2 + 3) * (10 - 4))";
    check(actual == expected, "Transpiles nested arithmetic", actual, expected);
}

void testSet(){
    currentTestFunction = "testSet";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------------- TEST SET --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(set x 5)");
    expected = "x = 5";
    check(actual == expected, "Transpiles set with number", actual, expected);

    actual = transpileString("(set name 'a)");
    expected = "name = \"a\"";
    check(actual == expected, "Transpiles set with quoted atom", actual, expected);

    actual = transpileString("(set values '(1 2 3))");
    expected = "values = [1, 2, 3]";
    check(actual == expected, "Transpiles set with quoted list", actual, expected);

    actual = transpileString("(set x (add 2 3))");
    expected = "x = (2 + 3)";
    check(actual == expected, "Transpiles set with expression", actual, expected);
}

void testVariables(){
    currentTestFunction = "testVariables";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "------------------- TEST VARIABLES ------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(set x 10)");
    expected = "x = 10";
    check(actual == expected, "Transpiles set", actual, expected);

    actual = transpileString("(set x (add 2 3))");
    expected = "x = (2 + 3)";
    check(actual == expected, "Transpiles set with an expression", actual, expected);

    actual = transpileString("(set y (mul (add 2 3) 4))");
    expected = "y = ((2 + 3) * 4)";
    check(actual == expected, "Transpiles set with nested expressions", actual, expected);
}

void testQuote(){
    currentTestFunction = "testQuote";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "--------------------- TEST QUOTE --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("'a");
    expected = "\"a\"";
    check(actual == expected, "Transpiles quoted atom", actual, expected);

    actual = transpileString("(quote a)");
    expected = "\"a\"";
    check(actual == expected, "Transpiles quote", actual, expected);

    actual = transpileString("'42");
    expected = "42";
    check(actual == expected, "Transpiles quoted number", actual, expected);

    actual = transpileString("'(a b c)");
    expected = "[\"a\", \"b\", \"c\"]";
    check(actual == expected, "Transpiles quoted list", actual, expected);

    actual = transpileString("'((a b) c)");
    expected = "[[\"a\", \"b\"], \"c\"]";
    check(actual == expected, "Transpiles nested quoted list", actual, expected);

    actual = transpileString("'T");
    expected = "\"T\"";
    check(actual == expected, "Transpiles quoted T", actual, expected);

    actual = transpileString("'(a 42 b -7)");
    expected = "[\"a\", 42, \"b\", -7]";
    check(actual == expected, "Transpiles quoted list with numbers", actual, expected);
}

void testCar(){
    currentTestFunction = "testCar";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST CAR ---------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(car '(a b c))");
    expected = "[\"a\", \"b\", \"c\"][0]";
    check(actual == expected, "Transpiles car with quoted list", actual, expected);

    actual = transpileString("(car '((a b) c))");
    expected = "[[\"a\", \"b\"], \"c\"][0]";
    check(actual == expected, "Transpiles car with nested list", actual, expected);

    actual = transpileString("(car x)");
    expected = "x[0]";
    check(actual == expected, "Transpiles car with variable", actual, expected);
}

void testCdr(){
    currentTestFunction = "testCdr";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST CDR ---------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(cdr '(a b c))");
    expected = "[\"a\", \"b\", \"c\"][1:]";
    check(actual == expected, "Transpiles cdr with quoted list", actual, expected);

    actual = transpileString("(cdr '(a))");
    expected = "[\"a\"][1:]";
    check(actual == expected, "Transpiles cdr with one element", actual, expected);

    actual = transpileString("(cdr x)");
    expected = "x[1:]";
    check(actual == expected, "Transpiles cdr with variable", actual, expected);
}

void testCons(){
    currentTestFunction = "testCons";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST CONS --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(cons 'a ())");
    expected = "[\"a\"] + []";
    check(actual == expected, "Transpiles cons with empty list", actual, expected);

    actual = transpileString("(cons 'a '(b c))");
    expected = "[\"a\"] + [\"b\", \"c\"]";
    check(actual == expected, "Transpiles cons with quoted list", actual, expected);

    actual = transpileString("(cons 1 '(2 3))");
    expected = "[1] + [2, 3]";
    check(actual == expected, "Transpiles cons with numbers", actual, expected);

    actual = transpileString("(cons 'a x)");
    expected = "[\"a\"] + x";
    check(actual == expected, "Transpiles cons with variable", actual, expected);
}

void testAnd(){
    currentTestFunction = "testAnd";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST AND? --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(and? 'a 'b)");
    expected = "(\"b\" if \"a\" != [] and (type(\"a\") != bool or \"a\") else False)";
    check(actual == expected, "Transpiles and? with true values", actual, expected);

    actual = transpileString("(and? () 'b)");
    expected = "(\"b\" if [] != [] and (type([]) != bool or []) else False)";
    check(actual == expected, "Transpiles and? with nil", actual, expected);

    actual = transpileString("(and? 0 'b)");
    expected = "(\"b\" if 0 != [] and (type(0) != bool or 0) else False)";
    check(actual == expected, "Transpiles and? with zero", actual, expected);

    actual = transpileString("(and? '(a) 5)");
    expected = "(5 if [\"a\"] != [] and (type([\"a\"]) != bool or [\"a\"]) else False)";
    check(actual == expected, "Transpiles and? with list", actual, expected);
}

void testOr(){
    currentTestFunction = "testOr";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------------- TEST OR? --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(or? 'a 'b)");
    expected = "(\"a\" if \"a\" != [] and (type(\"a\") != bool or \"a\") else \"b\")";
    check(actual == expected, "Transpiles or? with true first value", actual, expected);

    actual = transpileString("(or? () 'b)");
    expected = "([] if [] != [] and (type([]) != bool or []) else \"b\")";
    check(actual == expected, "Transpiles or? with nil first value", actual, expected);

    actual = transpileString("(or? 0 'b)");
    expected = "(0 if 0 != [] and (type(0) != bool or 0) else \"b\")";
    check(actual == expected, "Transpiles or? with zero", actual, expected); 

    actual = transpileString("(or? '(a) 5)");
    expected = "([\"a\"] if [\"a\"] != [] and (type([\"a\"]) != bool or [\"a\"]) else 5)";
    check(actual == expected, "Transpiles or? with list", actual, expected);
}

void testIf(){
    currentTestFunction = "testIf";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------------- TEST IF ---------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(if 'T 'a 'b)");
    expected = "(\"a\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else \"b\")";
    check(actual == expected, "Transpiles if with true condition", actual, expected);

    actual = transpileString("(if () 'a 'b)");
    expected = "(\"a\" if [] != [] and (type([]) != bool or []) else \"b\")";
    check(actual == expected, "Transpiles if with nil condition", actual, expected);

    actual = transpileString("(if 0 'a 'b)");
    expected = "(\"a\" if 0 != [] and (type(0) != bool or 0) else \"b\")";
    check(actual == expected, "Transpiles if with zero condition", actual, expected);

    actual = transpileString("(if '(a) 1 2)");
    expected = "(1 if [\"a\"] != [] and (type([\"a\"]) != bool or [\"a\"]) else 2)";
    check(actual == expected, "Transpiles if with list condition", actual, expected);
}

void testCond(){
    currentTestFunction = "testCond";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST COND --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(cond ('T 'yes 'T 'no))");
    expected = "(\"yes\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else (\"no\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else False))";
    check(actual == expected, "Transpiles cond with true first condition", actual, expected);

    actual = transpileString("(cond (() 'first 'T 'second))");
    expected = "(\"first\" if [] != [] and (type([]) != bool or []) else (\"second\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else False))";
    check(actual == expected, "Transpiles cond with nil first condition", actual, expected);

    actual = transpileString("(cond ((lt x 0) 'negative (eq? x 0) 'zero 'T 'positive))");
    expected = "(\"negative\" if (x < 0) != [] and (type((x < 0)) != bool or (x < 0)) else (\"zero\" if (not isinstance(x, list) and not isinstance(0, list) and x == 0) != [] and (type((not isinstance(x, list) and not isinstance(0, list) and x == 0)) != bool or (not isinstance(x, list) and not isinstance(0, list) and x == 0)) else (\"positive\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else False)))";
    check(actual == expected, "Transpiles cond with multiple conditions", actual, expected);

    actual = transpileString("(cond ((number? x) 'number (atom? x) 'atom 'T 'other))");
    expected = "(\"number\" if (type(x) == int) != [] and (type((type(x) == int)) != bool or (type(x) == int)) else (\"atom\" if (not isinstance(x, list)) != [] and (type((not isinstance(x, list))) != bool or (not isinstance(x, list))) else (\"other\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else False)))";
    check(actual == expected, "Transpiles cond with predicates", actual, expected);

    actual = transpileString("(cond (() 'no))");
    expected = "(\"no\" if [] != [] and (type([]) != bool or []) else False)";
    check(actual == expected, "Transpiles cond with no true condition", actual, expected);

    actual = transpileString("(cond (0 'zero 'T 'other))");
    expected = "(\"zero\" if 0 != [] and (type(0) != bool or 0) else (\"other\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else False))";
    check(actual == expected, "Transpiles cond with zero as true condition", actual, expected);
}

void testNil(){
    currentTestFunction = "testNil";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST NIL? --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(nil? ())");
    expected = "([] == [])";
    check(actual == expected, "Transpiles nil? with nil", actual, expected);

    actual = transpileString("(nil? 'a)");
    expected = "(\"a\" == [])";
    check(actual == expected, "Transpiles nil? with atom", actual, expected);

    actual = transpileString("(nil? 0)");
    expected = "(0 == [])";
    check(actual == expected, "Transpiles nil? with zero", actual, expected);

    actual = transpileString("(nil? '(a b))");
    expected = "([\"a\", \"b\"] == [])";
    check(actual == expected, "Transpiles nil? with list", actual, expected);
}

void testAtom(){
    currentTestFunction = "testAtom";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST ATOM? -------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(atom? 'a)");
    expected = "(not isinstance(\"a\", list))";
    check(actual == expected, "Transpiles atom? with atom", actual, expected);

    actual = transpileString("(atom? 42)");
    expected = "(not isinstance(42, list))";
    check(actual == expected, "Transpiles atom? with number", actual, expected);

    actual = transpileString("(atom? '(a b))");
    expected = "(not isinstance([\"a\", \"b\"], list))";
    check(actual == expected, "Transpiles atom? with list", actual, expected);
}

void testList(){
    currentTestFunction = "testList";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------------- TEST LIST? -------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(list? '(a b))");
    expected = "(isinstance([\"a\", \"b\"], list))";
    check(actual == expected, "Transpiles list? with list", actual, expected);

    actual = transpileString("(list? 'a)");
    expected = "(isinstance(\"a\", list))";
    check(actual == expected, "Transpiles list? with atom", actual, expected);

    actual = transpileString("(list? 42)");
    expected = "(isinstance(42, list))";
    check(actual == expected, "Transpiles list? with number", actual, expected);

    actual = transpileString("(list? x)");
    expected = "(isinstance(x, list))";
    check(actual == expected, "Transpiles list? with variable", actual, expected);
}

void testNumber(){
    currentTestFunction = "testNumber";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "--------------------- TEST NUMBER? ------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(number? 42)");
    expected = "(type(42) == int)";
    check(actual == expected, "Transpiles number? with number", actual, expected);

    actual = transpileString("(number? '42)");
    expected = "(type(42) == int)";
    check(actual == expected, "Transpiles number? with quoted number", actual, expected);

    actual = transpileString("(number? '-42)");
    expected = "(type(-42) == int)";
    check(actual == expected, "Transpiles number? with negative number", actual, expected);

    actual = transpileString("(number? 'a)");
    expected = "(type(\"a\") == int)";
    check(actual == expected, "Transpiles number? with atom", actual, expected);

    actual = transpileString("(number? '(1 2))");
    expected = "(type([1, 2]) == int)";
    check(actual == expected, "Transpiles number? with list", actual, expected);

    actual = transpileString("(number? x)");
    expected = "(type(x) == int)";
    check(actual == expected, "Transpiles number? with variable", actual, expected);

    actual = transpileString("(number? (lt 1 2))");
    expected = "(type((1 < 2)) == int)";
    check(actual == expected, "Transpiles number? with true value", actual, expected);
}

void testNot(){
    currentTestFunction = "testNot";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------------- TEST NOT? -------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(not? ())");
    expected = "([] == [] or [] == False)";
    check(actual == expected, "Transpiles not? with nil", actual, expected);

    actual = transpileString("(not? 'a)");
    expected = "(\"a\" == [] or \"a\" == False)";
    check(actual == expected, "Transpiles not? with atom", actual, expected);

    actual = transpileString("(not? 0)");
    expected = "(0 == [] or 0 == False)";
    check(actual == expected, "Transpiles not? with zero", actual, expected);

    actual = transpileString("(not? '(a))");
    expected = "([\"a\"] == [] or [\"a\"] == False)";
    check(actual == expected, "Transpiles not? with list", actual, expected);
}

void testEq(){
    currentTestFunction = "testEq";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------------- TEST EQ? --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(eq? 'a 'a)");
    expected = "(not isinstance(\"a\", list) and not isinstance(\"a\", list) and \"a\" == \"a\")";
    check(actual == expected, "Transpiles eq? with equal atoms", actual, expected);

    actual = transpileString("(eq? 'a 'b)");
    expected = "(not isinstance(\"a\", list) and not isinstance(\"b\", list) and \"a\" == \"b\")";
    check(actual == expected, "Transpiles eq? with different atoms", actual, expected);

    actual = transpileString("(eq? 42 42)");
    expected = "(not isinstance(42, list) and not isinstance(42, list) and 42 == 42)";
    check(actual == expected, "Transpiles eq? with numbers", actual, expected);

    actual = transpileString("(eq? '42 42)");
    expected = "(not isinstance(42, list) and not isinstance(42, list) and 42 == 42)";
    check(actual == expected, "Transpiles eq? with quoted number", actual, expected);

    actual = transpileString("(eq? '(a) '(a))");
    expected = "(not isinstance([\"a\"], list) and not isinstance([\"a\"], list) and [\"a\"] == [\"a\"])";
    check(actual == expected, "Transpiles eq? with lists", actual, expected);

    actual = transpileString("(eq? () ())");
    expected = "(not isinstance([], list) and not isinstance([], list) and [] == [])";
    check(actual == expected, "Transpiles eq? with nil", actual, expected);
}

void testLt(){
    currentTestFunction = "testLt";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------------- TEST LT ---------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(lt 2 5)");
    expected = "(2 < 5)";
    check(actual == expected, "Transpiles lt with smaller first value", actual, expected);

    actual = transpileString("(lt 5 2)");
    expected = "(5 < 2)";
    check(actual == expected, "Transpiles lt with larger first value", actual, expected);

    actual = transpileString("(lt -5 2)");
    expected = "(-5 < 2)";
    check(actual == expected, "Transpiles lt with negative number", actual, expected);

    actual = transpileString("(lt (add 1 2) 5)");
    expected = "((1 + 2) < 5)";
    check(actual == expected, "Transpiles lt with expression", actual, expected);
}

void testDef(){
    currentTestFunction = "testDef";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------------- TEST DEF --------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(def square (x) (mul x x))");
    expected = "def square(x):\n    return (x * x)";
    check(actual == expected, "Transpiles function definition", actual, expected);

    actual = transpileString("(def addTwo (x y) (add x y))");
    expected = "def addTwo(x, y):\n    return (x + y)";
    check(actual == expected, "Transpiles function with multiple parameters", actual, expected);

    actual = transpileString("(def isZero (x) (eq? x 0))");
    expected = "def isZero(x):\n    return (not isinstance(x, list) and not isinstance(0, list) and x == 0)";
    check(actual == expected, "Transpiles function with predicate body", actual, expected);
}

void testFunctionCall(){
    currentTestFunction = "testFunctionCall";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "----------------- TEST FUNCTION CALL ----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(square 5)");
    expected = "square(5)";
    check(actual == expected, "Transpiles function call", actual, expected);

    actual = transpileString("(addTwo 2 3)");
    expected = "addTwo(2, 3)";
    check(actual == expected, "Transpiles function call with multiple arguments", actual, expected);

    actual = transpileString("(square (add 2 3))");
    expected = "square((2 + 3))";
    check(actual == expected, "Transpiles function call with expression argument", actual, expected);

    actual = transpileString("(foo 'a '(b c))");
    expected = "foo(\"a\", [\"b\", \"c\"])";
    check(actual == expected, "Transpiles function call with quoted arguments", actual, expected);
}

void testFunction(){
    currentTestFunction = "testFunction";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "-------------------- TEST FUNCTION ------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    string actual;
    string expected;

    actual = transpileString("(function (x) (add x 1))");
    expected = "lambda x: (x + 1)";
    check(actual == expected, "Transpiles function with one parameter", actual, expected);

    actual = transpileString("(function (x y) (mul x y))");
    expected = "lambda x, y: (x * y)";
    check(actual == expected, "Transpiles function with multiple parameters", actual, expected);

    actual = transpileString("(set increment (function (x) (add x 1)))");
    expected = "increment = lambda x: (x + 1)";
    check(actual == expected, "Transpiles function assigned with set", actual, expected);

    actual = transpileString("((function (x) (add x 1)) 5)");
    expected = "(lambda x: (x + 1))(5)";
    check(actual == expected, "direct anonymous function call", actual, expected);
}
void testPrint(){
    currentTestFunction = "testPrint";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "------------------------ TEST PRINT -----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl; 
    
    string actual;
    string expected;

    actual = transpileString("(print (square 5))");
    expected = "print(square(5))";
    check(actual == expected, "Transpiles print with function call", actual, expected);

    actual = transpileString("(print (add 10 20))");
    expected = "print((10 + 20))";
    check(actual == expected, "Transpiles print with expression", actual, expected);

    actual = transpileString("(print 'hello)");
    expected = "print(\"hello\")";
    check(actual == expected, "Transpiles print with quoted symbol", actual, expected);

}

void testTranspileFile(){
    currentTestFunction = "testTranspileFile";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------- TEST TRANSPILE FILE ----------------------" << endl;
    cout << "-----------------------------------------------------------" << endl; 

    ofstream input("test_program.lisp");
    input << "(def square (x) (mul x x))" << endl;
    input << "(set x 5)" << endl;
    input << "(square x)" << endl;
    input.close();

    transpileFile("test_program.lisp", "test_program.py");

    ifstream output("test_program.py");
    string actual(
        (istreambuf_iterator<char>(output)),
        istreambuf_iterator<char>()
    );
    output.close();

    string expected =
        "def square(x):\n"
        "    return (x * x)\n\n"
        "x = 5\n\n"
        "square(x)\n\n";

    check(actual == expected, "Transpiles Lisp file to Python file", actual, expected);

    ofstream input2("test_expressions.lisp");
    input2 << "(add 2 3)" << endl;
    input2 << "(if 'T 'yes 'no)" << endl;
    input2 << "(number? 5)" << endl;
    input2.close();

    transpileFile("test_expressions.lisp", "test_expressions.py");

    ifstream output2("test_expressions.py");
    string actual2(
        (istreambuf_iterator<char>(output2)),
        istreambuf_iterator<char>()
    );
    output2.close();

    expected =
            "(2 + 3)\n\n"
            "(\"yes\" if \"T\" != [] and (type(\"T\") != bool or \"T\") else \"no\")\n\n"
            "(type(5) == int)\n\n";

    check(actual2 == expected, "Transpiles multiple expressions in file", actual2, expected);

    ofstream input3("test_statements.lisp");
    input3 << "(set x 5)" << endl;
    input3 << "(set y 10)" << endl;
    input3 << "(def addTwo (a b) (add a b))" << endl;
    input3.close();

    transpileFile("test_statements.lisp", "test_statements.py");

    ifstream output3("test_statements.py");
    string actual3(
        (istreambuf_iterator<char>(output3)),
        istreambuf_iterator<char>()
    );
    output3.close();

    expected =
        "x = 5\n\n"
        "y = 10\n\n"
        "def addTwo(a, b):\n"
        "    return (a + b)\n\n";

    check(actual3 == expected, "Transpiles definitions and assignments", actual3, expected);

    ofstream basicInput("test_basic_file.lisp");
    basicInput << "42" << endl;
    basicInput << "()" << endl;
    basicInput.close();

    transpileFile("test_basic_file.lisp", "test_basic_file.py");

    ifstream basicOutput("test_basic_file.py");
    actual = string(
        (istreambuf_iterator<char>(basicOutput)),
        istreambuf_iterator<char>()
    );
    basicOutput.close();

    expected =
        "42\n"
        "\n"
        "[]\n"
        "\n";

    check(actual == expected, "Transpiles basic values from file", actual, expected);

    bool success = transpileFile("does_not_exist.lisp", "does_not_exist.py");

    actual = success ? "true" : "false";
    expected = "false";

    check(actual == expected, "Returns false when input file does not exist", actual, expected);

    remove("test_program.lisp");
    remove("test_program.py");
    remove("test_expressions.lisp");
    remove("test_expressions.py");
    remove("test_statements.lisp");
    remove("test_statements.py");
    remove("test_basic_file.lisp");
    remove("test_basic_file.py");
    remove("does_not_exist.py");
}

void testExecutePythonFile(){
    currentTestFunction = "testExecutePythonFile";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "--------------- TEST EXECUTE PYTHON FILE -----------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    ofstream pythonFile("test_execute.py");
    pythonFile << "print(2 + 3)" << endl;
    pythonFile.close();

    int result = executePythonFile("test_execute.py", "test_execute.out");

    ifstream output("test_execute.out");
    string actual(
        (istreambuf_iterator<char>(output)),
        istreambuf_iterator<char>()
    );
    output.close();

    string expected = "5\n";

    check(result == 0, "Executes Python file successfully",
        to_string(result), "0");

    check(actual == expected, "Writes Python output to result file: print(2 + 3)",
        actual, expected);

    remove("test_execute.py");
    remove("test_execute.out");
}

void testFullProgram(){
    currentTestFunction = "testFullProgram";
    cout << "\n-----------------------------------------------------------" << endl;
    cout << "---------------- TEST FULL PROGRAM ------------------------" << endl;
    cout << "-----------------------------------------------------------" << endl;

    ofstream input("test_full.lisp");
    input << "(def square (x) (mul x x))" << endl;
    input << "(set x 5)" << endl;
    input << "(print (square x))" << endl;
    input << "(print (add (square x) 10))" << endl;
    input.close();

    transpileFile("test_full.lisp", "test_full.py");

    int result = executePythonFile("test_full.py", "test_full.out");

    ifstream output("test_full.out");
    string actual(
        (istreambuf_iterator<char>(output)),
        istreambuf_iterator<char>()
    );
    output.close();

    string expected =
        "25\n"
        "35\n";

    check(result == 0, "Executes transpiled program successfully",
        to_string(result), "0");

    check(actual == expected, "Produces correct transpiled program output",
        actual, expected);

    remove("test_full.lisp");
    remove("test_full.py");
    remove("test_full.out");
}

void testReport(){
    cout << "\n===========================================================" << endl;
    cout << "                        TEST REPORT                        " << endl;
    cout << "===========================================================" << endl;
    cout << "✅ Passed: " << testsPassed << endl;
    cout << "❌ Failed: " << testsFailed << endl;
    cout << "Total:  " << testsPassed + testsFailed << endl;

    if (testsFailed > 0){
        cout << "\nFailed Tests:" << endl;

        string lastTestFunction = "";

        for (TestFailure failure : failedTests){
            if (failure.testFunction != lastTestFunction){
                cout << "\n❌ " << failure.testFunction << endl;
                lastTestFunction = failure.testFunction;
            }

            cout << "   - Line " << failure.line << ": " << failure.test << endl;
        }
    }
}

int main(){
    testBasicValues();
    testArithmetic();
    testVariables();

    testQuote();
    testCar();
    testCdr();
    testCons();

    testNil();
    testAtom();
    testList();
    testNumber();
    testNot();

    testAnd();
    testOr();
    testEq();
    
    testIf();
    testCond();
    testSet();
    testLt();

    testDef();
    testFunctionCall();
    testFunction();
    testPrint();

    testTranspileFile();
    testExecutePythonFile();
    testFullProgram();

    testReport();

    return 0;
}