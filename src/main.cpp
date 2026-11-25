#include "transpiler.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]){
    if (argc != 2){
        cout << "Usage: ./transpiler <lisp file>" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = inputFile;

    size_t dot = outputFile.find_last_of('.');

    if (dot != string::npos){
        outputFile = outputFile.substr(0, dot);
    }

    outputFile += ".py";

    if (!transpileFile(inputFile, outputFile)){
        cout << "Input file '" << inputFile << "' does not exist." << endl;
        return 1;
    }

    cout << "Created " << outputFile << endl;

    string resultFile = outputFile.substr(0, outputFile.find_last_of('.')) + ".out";

    int result = executePythonFile(outputFile, resultFile);

    if (result == 0){
        cout << "Executed " << outputFile << " successfully. See " << resultFile << " for output." << endl;
    }
    else {
        cout << "Error executing " << outputFile << ". See " << resultFile << " for error." << endl;
    }

    return 0;
}