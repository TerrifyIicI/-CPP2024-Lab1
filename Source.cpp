#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <string>
#include <complex>
#include <string.h>
#include <regex>
#include <vector>

using namespace std;

struct QuadraticEquation {
    double a;
    double b;
    double c;
};

struct Solution {
    string equation;
    vector<string> solutions;
    string name;
};

class FileReader {
private:
    FILE* file;

public:
    FileReader(const char* filename) {
        file = fopen(filename, "rt");
        if (file == nullptr) {
            std::cerr << "Ошибка файла " << filename << std::endl;
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

    Solution readAndProcessLine() {
        char* line = readLine();
        string str(line);
        delete[] line;

        Solution sol;
        istringstream iss(str);
        vector<std::string> tokens;
        string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        // Имя студента всегда последнее
        sol.name = tokens.back();
        tokens.pop_back();

        // Проверяем, является ли последний элемент корнем
        regex root_regex("([\\-\\+]?\\d*\\.?\\d*(?:[\\-\\+]?\\d*\\.?\\d*i)?)");
        if (std::regex_match(tokens.back(), root_regex)) {
            sol.solutions.push_back(tokens.back());
            tokens.pop_back();
        }

        // Проверяем, является ли следующий элемент корнем
        if (!tokens.empty() && std::regex_match(tokens.back(), root_regex)) {
            sol.solutions.push_back(tokens.back());
            tokens.pop_back();
        }

        // Остальное считаем уравнением
        std::ostringstream oss;
        for (const auto& t : tokens) {
            oss << t << " ";
        }
        sol.equation = oss.str();

        return sol;
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

// parseEquation разбирает уравение из строки и записывает его в структуру QuadraticEquation
QuadraticEquation parseEquation(const char* line) {
    QuadraticEquation equation = { 0, 0, 0 }; // инициализация структуры QuadraticEquation нулями
    bool isRightSide = false; // флаг, указывающий, находимся ли мы на правой стороне уравнения (после знака '=')
    int sign = 1; // текущий знак (по умолчанию положительный)
    double value = 0; // текущее значение (коэффициент при переменной)
    int i = 0;

    while (i < strlen(line)) {
        if (line[i] >= '0' && line[i] <= '9') { // если текущий символ - цифра
            value = value * 10 + (line[i] - '0'); // добавляем цифру к текущему значению
        }
        else if (line[i] == 'x') { // если текущий символ - 'x' (переменная)
            i++;
            if (value == 0) {
                value = 1;
            }
            if (line[i] == '^') { // если после 'x' следует '^' (степень)
                if ((line[i + 1] > '2') || (i < strlen(line) - 2 && line[i + 2] >= '0' && line[i + 2] <= '9')) {
                    cout << "Это ловушка, уравнение не квадратное" << endl; // если степень больше 2, то уравнение не квадратное
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
                addValue(equation, sign, value, 1); // добавляем значен  е к коэффициенту при x
            }
            value = 0;
        }
        else if (line[i] == '+' || line[i] == '-') { // если текущий символ - '+' или '-'
            if (value != 0) {
                addValue(equation, sign, value, 0); // добавляем значение к свободному члену
                value = 0;
            }
            sign = (line[i] == '+') ? 1 : -1; // меняем знак на противоположный
            if (isRightSide) {
                sign *= -1;
            }
        }
        else if (line[i] == '=') { // если текущий символ - '='
            if (value != 0) {
                addValue(equation, sign, value, 0); // добавляем значение к свободному члену
                value = 0;
            }
            isRightSide = true; // переходим на правую сторону уравнения
            sign = -1; // меняем знак на противоположный
        }
        i++;
    }

    if (value != 0) {
        addValue(equation, sign, value, 0); // добавляем значение к свободному члену
    }
    if (equation.a == 0) {
        cout << "Это ловушка, уравнение не квадратное" << endl; // если коэффициент при x^2 равен 0, то уравнение не квадратное
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

// calculateRoots вычисляет корни квадратного уравнения и   озвращает их в виде пары комплексных чисел
pair<ComplexNumber, ComplexNumber> calculateRoots(QuadraticEquation equation) {
    // вычисляем дискриминант
    double discriminant = equation.b * equation.b - 4 * equation.a * equation.c;

    // проверяем, не является ли уравнение тривиальным
    if (equation.a == 0 && equation.b == 0 && equation.c == 0) {
        throw invalid_argument(" 0");
    }
    // если дискриминант больше 0, то корни вещественные и различные
    else if (discriminant > 0) {
        double root1_real = (-equation.b + sqrt(discriminant)) / (2 * equation.a);
        double root2_real = (-equation.b - sqrt(discriminant)) / (2 * equation.a);
        return make_pair(ComplexNumber(root1_real, 0.0), ComplexNumber(root2_real, 0.0));
    }
    // если дискриминант равен 0, то корень вещественный и один
    else if (discriminant == 0) {
        double root_real = -equation.b / (2 * equation.a);
        // возвращаем пару одинаковых комплексных чисел
        return make_pair(ComplexNumber(root_real, 0.0), ComplexNumber(root_real, 0.0));
    }
    // если дискриминант меньше 0, то корни комплексные и сопряженные
    else {
        double realPart = -equation.b / (2 * equation.a);
        double imagPart = sqrt(-discriminant) / (2 * equation.a);
        // возвращаем пару комплексных чисел
        return make_pair(ComplexNumber(realPart, imagPart), ComplexNumber(realPart, -imagPart));
    }
}


void writeToFile(const pair<ComplexNumber, ComplexNumber>& roots, FileWriter& file) {
    const int precision = 5;//точность
    const int length = 20;
    char buffer[2 * (precision + length) + 1];//буфер для записи

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


    if ((root1.real == root2.real) && (root1.imag == root2.imag)) {
        fprintf(file.file, "%-*s\n", length, root1_str.c_str());
    }
    else if ((root1.imag == 0) && (root1.imag == 0)) {
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
    cout << "Введите имя входного файла: ";
    cin >> filename;
    FileReader fileReader(filename);

    cout << "Введите имя выходного файла: ";
    cin >> filename;
    FileWriter outputFile(filename);

    while (!fileReader.isEndOfFile()) {
        Solution sol = fileReader.readAndProcessLine();
        QuadraticEquation equation = parseEquation(sol.equation.c_str());
        printEquation(equation);
        solveAndWrite(equation, outputFile);
    }

    return 0;
}
