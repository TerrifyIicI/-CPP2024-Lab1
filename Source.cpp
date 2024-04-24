#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

struct Equation {
    double a;
    double b;
    double c;
};

class FileHandler {
private:
    FILE* file;

public:
    FileHandler(const char* filename) {
        file = fopen(filename, "rt");
        if (file == nullptr) {
            std::cerr << "Error file " << filename << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    ~FileHandler() {
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

void add_value(Equation& equation, int sign, double value, int power) {
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
Equation parse_equation(const char* line) {
    Equation equation = { 0, 0, 0 };
    bool isRightSide = false;
    int sign = 1;
    double value = 0;
    int i = 0;

    while (i < strlen(line)) {
        if (line[i] >= '0' && line[i] <= '9') {
            value = value * 10 + (line[i] - '0');
        }
        else if (line[i] == 'x') {
            i++;
            if (value == 0) {
                value = 1;
            }
            if (line[i] == '^') {
                if ((line[i + 1] > '2') || (i < strlen(line) - 2 && line[i + 2] >= '0' && line[i + 2] <= '9')) {
                    std::cout << "This is a trap equation is not quadratic\n";
                    equation = { 0, 0, 0 };
                    return equation;
                }
                if (line[i + 1] == '2') {
                    i++;
                    add_value(equation, sign, value, 2);
                }
                else if (line[i + 1] == '0') {
                    i++;
                    add_value(equation, sign, value, 0);
                }
                else {
                    add_value(equation, sign, value, 1);
                }
                value = 0;
            }
            else {
                i--;
                if (value == 0) {
                    value = 1;
                }
                add_value(equation, sign, value, 1);
            }
            value = 0;
        }
        else if (line[i] == '+' || line[i] == '-') {
            if (value != 0) {
                add_value(equation, sign, value, 0);
                value = 0;
            }
            sign = (line[i] == '+') ? 1 : -1;
            if (isRightSide) {
                sign *= -1;
            }
        }
        else if (line[i] == '=') {
            if (value != 0) {
                add_value(equation, sign, value, 0);
                value = 0;
            }
            isRightSide = true;
            sign = -1;
        }
        i++;
    }

    if (value != 0) {
        add_value(equation, sign, value, 0);
    }
    if (equation.a == 0) {
        std::cout << "This is a trap equation is not quadratic\n";
        equation = { 0, 0, 0 };
        return equation;
    }
    return equation;
}


void print_equation(Equation equation) {
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

void solve_equation(const Equation& equation, double& root1, double& root2) {
    double discriminant = equation.b * equation.b - 4 * equation.a * equation.c;
    if (discriminant > 0) {
        root1 = (-equation.b + sqrt(discriminant)) / (2 * equation.a);
        root2 = (-equation.b - sqrt(discriminant)) / (2 * equation.a);
    }
    else if (discriminant == 0) {
        root1 = root2 = -equation.b / (2 * equation.a);
    }
    else {
        root1 = root2 = NAN; // No real roots
    }
}class Wt_File {
public:
    FILE* file;
    Wt_File(const char* name) {
        file = fopen(name, "wt");
    }
    int Write(const char* string) {
        if (strlen(string) == 1)
            return 0;
        fputs(string, file);
        return 1;
    }
    ~Wt_File() {
        if (file != nullptr) {
            fclose(file);
        }
    }
};

void solve_and_write(Equation equation, Wt_File& file) {
    double discriminant = equation.b * equation.b - 4 * equation.a * equation.c;
    if (discriminant > 0) {
        double root1 = (-equation.b + sqrt(discriminant)) / (2 * equation.a);
        double root2 = (-equation.b - sqrt(discriminant)) / (2 * equation.a);
        fprintf(file.file, "Roots are real and different: %.2lf, %.2lf\n", root1, root2);
    }
    else if (discriminant == 0) {
        double root = -equation.b / (2 * equation.a);
        fprintf(file.file, "Roots are real and same: %.2lf\n", root);
    }
    else {
        fprintf(file.file, "Roots are complex and different.\n");
    }
}

int main() {
    char filename[16];
    std::strcpy(filename, "equations.txt");

    FileHandler fileHandler(filename);

    std::cout << "Enter the output file name: ";
    std::cin >> filename;
    Wt_File outputFile(filename);

    while (!fileHandler.isEndOfFile()) {
        char* line = fileHandler.readLine();
        Equation equation = parse_equation(line);
        print_equation(equation);
        solve_and_write(equation, outputFile);
        delete[] line;
    }

    return 0;
}