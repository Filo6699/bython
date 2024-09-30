#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void readFileContent(const string& filename, string& content) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        exit(1);
    }
    ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    file.close(); 
}

string replaceNewlines(const string& input) {
    string output = input;
    string replacement = ";\"\n\t\"";

    size_t pos = 0;
    while ((pos = output.find('\n', pos)) != string::npos) {
        output.replace(pos, 1, replacement);
        pos += replacement.length();
    }

    return output;
}

void replacePlaceholderInTemplate(const string& templateFilename, const string& inputContent, const string& outputFilename) {
    ifstream templateFile(templateFilename);
    if (!templateFile.is_open()) {
        cerr << "Error: Could not open the template file " << templateFilename << endl;
        exit(1);
    }

    string templateContent((istreambuf_iterator<char>(templateFile)), istreambuf_iterator<char>());
    templateFile.close();

    string orz = replaceNewlines(inputContent);

    size_t pos = templateContent.find("<REPLACE>");
    if (pos != string::npos) {
        templateContent.replace(pos, 9, "\"" + orz + "\"");
    }

    ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        cerr << "Error: Could not open the output file " << outputFilename << endl;
        exit(1);
    }

    outputFile << templateContent;
    outputFile.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_filename> <output_filename>" << endl;
        return 1;
    }

    string inputFilename = argv[1];
    string templateFilename = "template.txt";
    
    string inputContent;
    readFileContent(inputFilename, inputContent);

    string outputFilename = argv[2];
    replacePlaceholderInTemplate(templateFilename, inputContent, outputFilename);

    cout << "Processed template and wrote to " << outputFilename << endl;

    return 0;
}
