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

// parseEquation разбирает строку и преобразовывает ее в структуру QuadraticEquation
QuadraticEquation parseEquation(const char* line) {
    QuadraticEquation equation = { 0, 0, 0 }; // инициализация уравнения с нулевыми коэффициентами
    bool isRightSide = false; // флаг для определения, достигли ли мы правой части уравнения (после знака '=')
    int sign = 1; // знак перед переменной (по умолчанию положительный)
    double value = 0; // значение переменной
    int i = 0;

    while (i < strlen(line)) { 
        if (line[i] >= '0' && line[i] <= '9') { // если символ - цифра
            value = value * 10 + (line[i] - '0'); // преобразуем символ в число и добавляем к текущему значению
        }
        else if (line[i] == 'x') { // если символ - 'x' (переменная)
            i++;
            if (value == 0) {
                value = 1;
            }
            if (line[i] == '^') { // если после 'x' идет '^' (степень)
                if ((line[i + 1] > '2') || (i < strlen(line) - 2 && line[i + 2] >= '0' && line[i + 2] <= '9')) {
                    cout << "This is a trap, equation is not quadratic" << endl; // если степень не равна 2, то это не квадратное уравнение
                    equation = { 0, 0, 0 };
                    return equation;
                }
                if (line[i + 1] == '2') { // если степень равна 2
                    i++;
                    addValue(equation, sign, value, 2); // добавляем значение к коэффициенту при x^2
                }
                else if (line[i + 1] == '0') { // если степень равна 0
                    i++;
                    addValue(equation, sign, value, 0); // добавляем значение к свободному члену
                }
                else { // если степень равна 1
                    addValue(equation, sign, value, 1); // добавляем значение к коэффициенту при x
                }
                value = 0;
            }
            else { // если после 'x' нет '^', то степень равна 1
                i--;
                if (value == 0) {
                    value = 1;
                }
                addValue(equation, sign, value, 1); // добавляем значение к коэффициенту при x
            }
            value = 0;
        }
        else if (line[i] == '+' || line[i] == '-') { // если символ - '+' или '-'
            if (value != 0) {
                addValue(equation, sign, value, 0); // добавляем значение к свободному члену
                value = 0;
            }
            sign = (line[i] == '+') ? 1 : -1; // меняем знак перед следующей переменной
            if (isRightSide) {
                sign *= -1;
            }
        }
        else if (line[i] == '=') { // если символ - '='
            if (value != 0) {
                addValue(equation, sign, value, 0); // добавляем значение к свободному члену
                value = 0;
            }
            isRightSide = true; // меняем флаг, указывающий, что достигли правой части уравнения
            sign = -1; // меняем знак перед следующей переменной
        }
        i++;
    }

    if (value != 0) {
        addValue(equation, sign, value, 0); // добавляем оставшееся значение к свободному члену
    }
    if (equation.a == 0) {
        cout << "This is a trap, equation is not quadratic" << endl; // если коэффициент при x^2 равен 0, то это не квадратное уравнение
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

// calculateRoots решает квадратное уравнение и возвращает значения корней в виде комплексных чисел
pair<ComplexNumber, ComplexNumber> calculateRoots(QuadraticEquation equation) {
    // Вычисляем дискриминант уравнения
    double discriminant = equation.b * equation.b - 4 * equation.a * equation.c;

    // Проверяем, не является ли уравнение тождеством (все коэффициенты равны нулю)
    if (equation.a == 0 && equation.b == 0 && equation.c == 0) {
        throw invalid_argument("It's a trap");
    }
    // Если дискриминант больше нуля, уравнение имеет два различных вещественных корня
    else if (discriminant > 0) {
        double root1_real = (-equation.b + sqrt(discriminant)) / (2 * equation.a);
        double root2_real = (-equation.b - sqrt(discriminant)) / (2 * equation.a);
        return make_pair(ComplexNumber(root1_real, 0.0), ComplexNumber(root2_real, 0.0));
    }
    // Если дискриминант равен нулю, уравнение имеет один вещественный корень (кратный корень)
    else if (discriminant == 0) {
        double root_real = -equation.b / (2 * equation.a);
        // Возвращаем пару одинаковых вещественных корней
        return make_pair(ComplexNumber(root_real, 0.0), ComplexNumber(root_real, 0.0));
    }
    // Если дискриминант меньше нуля, уравнение имеет два комплексных корня
    else {
        double realPart = -equation.b / (2 * equation.a);
        double imagPart = sqrt(-discriminant) / (2 * equation.a);
        // Возвращаем пару комплексных корней
        return make_pair(ComplexNumber(realPart, imagPart), ComplexNumber(realPart, -imagPart));
    }
}


void writeToFile(const pair<ComplexNumber, ComplexNumber>& roots, FileWriter& file) {
    const int precision = 5;//точность
    const int length = 20;
    char buffer[2 * (precision + length) + 1];//длина итоговой строки

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
