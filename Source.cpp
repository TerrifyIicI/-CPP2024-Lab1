#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <string>
#include <complex>

using namespace std;

struct QuadraticEquation {
    double a;
    double b;
    double c;
};

class FileReader {
private:
    FILE* file;

public:
    FileReader(const char* filename) {
        file = fopen(filename, "rt");
        if (file == nullptr) {
            cerr << "Error file " << filename << endl;
            exit(EXIT_FAILURE);
        }
    }

    ~FileReader() {
        if (file != nullptr) {
            fclose(file);
        }
    }

    bool isEndOfFile() {
        return feof(file);
    }

    char* readLine() {
        char* line = new char[256];
        fgets(line, 256, file);
        return line;
    }
};

void addValue(QuadraticEquation& equation, int sign, double value, int power) {
    if (power == 2) {
        equation.a += sign * value;
    }
    else if (power == 1) {
        equation.b += sign * value;
    }
    else {
        equation.c += sign * value;
    }
}

// parseEquation ��������� ������ � ��������������� �� � ��������� QuadraticEquation
QuadraticEquation parseEquation(const char* line) {
    QuadraticEquation equation = { 0, 0, 0 }; // ������������� ��������� � �������� ��������������
    bool isRightSide = false; // ���� ��� �����������, �������� �� �� ������ ����� ��������� (����� ����� '=')
    int sign = 1; // ���� ����� ���������� (�� ��������� �������������)
    double value = 0; // �������� ����������
    int i = 0;

    while (i < strlen(line)) { 
        if (line[i] >= '0' && line[i] <= '9') { // ���� ������ - �����
            value = value * 10 + (line[i] - '0'); // ����������� ������ � ����� � ��������� � �������� ��������
        }
        else if (line[i] == 'x') { // ���� ������ - 'x' (����������)
            i++;
            if (value == 0) {
                value = 1;
            }
            if (line[i] == '^') { // ���� ����� 'x' ���� '^' (�������)
                if ((line[i + 1] > '2') || (i < strlen(line) - 2 && line[i + 2] >= '0' && line[i + 2] <= '9')) {
                    cout << "This is a trap, equation is not quadratic" << endl; // ���� ������� �� ����� 2, �� ��� �� ���������� ���������
                    equation = { 0, 0, 0 };
                    return equation;
                }
                if (line[i + 1] == '2') { // ���� ������� ����� 2
                    i++;
                    addValue(equation, sign, value, 2); // ��������� �������� � ������������ ��� x^2
                }
                else if (line[i + 1] == '0') { // ���� ������� ����� 0
                    i++;
                    addValue(equation, sign, value, 0); // ��������� �������� � ���������� �����
                }
                else { // ���� ������� ����� 1
                    addValue(equation, sign, value, 1); // ��������� �������� � ������������ ��� x
                }
                value = 0;
            }
            else { // ���� ����� 'x' ��� '^', �� ������� ����� 1
                i--;
                if (value == 0) {
                    value = 1;
                }
                addValue(equation, sign, value, 1); // ��������� �������� � ������������ ��� x
            }
            value = 0;
        }
        else if (line[i] == '+' || line[i] == '-') { // ���� ������ - '+' ��� '-'
            if (value != 0) {
                addValue(equation, sign, value, 0); // ��������� �������� � ���������� �����
                value = 0;
            }
            sign = (line[i] == '+') ? 1 : -1; // ������ ���� ����� ��������� ����������
            if (isRightSide) {
                sign *= -1;
            }
        }
        else if (line[i] == '=') { // ���� ������ - '='
            if (value != 0) {
                addValue(equation, sign, value, 0); // ��������� �������� � ���������� �����
                value = 0;
            }
            isRightSide = true; // ������ ����, �����������, ��� �������� ������ ����� ���������
            sign = -1; // ������ ���� ����� ��������� ����������
        }
        i++;
    }

    if (value != 0) {
        addValue(equation, sign, value, 0); // ��������� ���������� �������� � ���������� �����
    }
    if (equation.a == 0) {
        cout << "This is a trap, equation is not quadratic" << endl; // ���� ����������� ��� x^2 ����� 0, �� ��� �� ���������� ���������
        equation = { 0, 0, 0 };
        return equation;
    }
    return equation;
}


void printEquation(QuadraticEquation equation) {
    printf("%lfx^2 ", equation.a);
    if (equation.b < 0) {
        printf("- %lfx ", -equation.b);
    }
    else {
        printf("+ %lfx ", equation.b);
    }
    if (equation.c < 0) {
        printf("- %lf ", -equation.c);
    }
    else {
        printf("+ %lf ", equation.c);
    }
    printf("= 0\n");
}

class FileWriter {
public:
    FILE* file;
    FileWriter(const char* name) {
        file = fopen(name, "wt");
    }
    int write(const char* string) {
        if (strlen(string) == 1)
            return 0;
        fputs(string, file);
        return 1;
    }
    ~FileWriter() {
        if (file != nullptr) {
            fclose(file);
        }
    }
};

struct ComplexNumber {
    double real;
    double imag;

    ComplexNumber(double r = 0.0, double i = 0.0) : real(r), imag(i) {}

    ComplexNumber operator+(const ComplexNumber& other) const {
        return ComplexNumber(real + other.real, imag + other.imag);
    }
};

// calculateRoots ������ ���������� ��������� � ���������� �������� ������ � ���� ����������� �����
pair<ComplexNumber, ComplexNumber> calculateRoots(QuadraticEquation equation) {
    // ��������� ������������ ���������
    double discriminant = equation.b * equation.b - 4 * equation.a * equation.c;

    // ���������, �� �������� �� ��������� ���������� (��� ������������ ����� ����)
    if (equation.a == 0 && equation.b == 0 && equation.c == 0) {
        throw invalid_argument("It's a trap");
    }
    // ���� ������������ ������ ����, ��������� ����� ��� ��������� ������������ �����
    else if (discriminant > 0) {
        double root1_real = (-equation.b + sqrt(discriminant)) / (2 * equation.a);
        double root2_real = (-equation.b - sqrt(discriminant)) / (2 * equation.a);
        return make_pair(ComplexNumber(root1_real, 0.0), ComplexNumber(root2_real, 0.0));
    }
    // ���� ������������ ����� ����, ��������� ����� ���� ������������ ������ (������� ������)
    else if (discriminant == 0) {
        double root_real = -equation.b / (2 * equation.a);
        // ���������� ���� ���������� ������������ ������
        return make_pair(ComplexNumber(root_real, 0.0), ComplexNumber(root_real, 0.0));
    }
    // ���� ������������ ������ ����, ��������� ����� ��� ����������� �����
    else {
        double realPart = -equation.b / (2 * equation.a);
        double imagPart = sqrt(-discriminant) / (2 * equation.a);
        // ���������� ���� ����������� ������
        return make_pair(ComplexNumber(realPart, imagPart), ComplexNumber(realPart, -imagPart));
    }
}


void writeToFile(const pair<ComplexNumber, ComplexNumber>& roots, FileWriter& file) {
    const int precision = 5;//��������
    const int length = 20;
    char buffer[2 * (precision + length) + 1];//����� �������� ������

    ComplexNumber root1 = roots.first;
    ComplexNumber root2 = roots.second;

    stringstream ss;

    ss << fixed << setprecision(precision) << (abs(root1.real) < pow(10, -precision) ? 0.0 : root1.real);

    string root1_str = (abs(root1.real) < pow(10, -precision) ? " " + ss.str() : (root1.real > 0 ? " " : "") + to_string(root1.real));
    string root2_str = (abs(root2.real) < pow(10, -precision) ? " " + ss.str() : (root2.real > 0 ? " " : "") + to_string(root2.real));

    if ((root1.imag == 0) && (root1.imag == 0)) {
        root1_str += "";
    }
    else {
        root1_str += (root1.imag >= 0 ? " + " : " - ") + to_string(abs(root1.imag)) + "i";
    }

    if ((root2.imag == 0) && (root2.imag == 0)) {
        root2_str += "";
    }
    else {
        root2_str += (root2.imag >= 0 ? " + " : " - ") + to_string(abs(root2.imag)) + "i";
    }

    if ((root1.imag == 0) && (root1.imag == 0)) {
        fprintf(file.file, "%-*s    %-*s\n", length, root1_str.c_str(), length, root2_str.c_str());
    }
    else {
        fprintf(file.file, "%-*s   %-*s\n", length, root1_str.c_str(), length, root2_str.c_str());
    }
}

void solveAndWrite(QuadraticEquation equation, FileWriter& file) {
    try {
        pair<ComplexNumber, ComplexNumber> roots = calculateRoots(equation);
        writeToFile(roots, file);
    }
    catch (const invalid_argument& e) {
        fprintf(file.file, "%s\n", e.what());
    }
}

int main() {
    char filename[16];
    //strcpy(filename, "equations.txt");
    cout << "Enter the input file name: ";
    cin >> filename;
    FileReader fileReader(filename);

    cout << "Enter the output file name: ";
    cin >> filename;
    FileWriter outputFile(filename);

    while (!fileReader.isEndOfFile()) {
        char* line = fileReader.readLine();
        QuadraticEquation equation = parseEquation(line);
        printEquation(equation);
        solveAndWrite(equation, outputFile);
        delete[] line;
    }

    return 0;
}
