#include <iostream>
#include <stack>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <cmath>
#include <cctype>

using namespace std;

unordered_map<string, double(*)(double)> oneArgumentFuncs = {
    {"sin", sin},
    {"cos", cos},
    {"tan", tan},
    {"log", log},
    {"sqrt", sqrt},
    {"exp", exp}
};

double performOperation(double op1, double op2, const string& operation){
    if(operation == "+") return op1 + op2;
    if(operation == "-") return op1 - op2;
    if(operation == "*") return op1 * op2;
    if(operation == "^") return pow(op1,op2);
    if(operation == "/") {
        if(op2 == 0) throw runtime_error("Ошибка: Деление на ноль.");
        return op1 / op2;
    }
    throw runtime_error("Ошибка: Неизвестная операция.");
};

void evaluateRPN(const string& expression){
    stack<double> st;
    stringstream ss(expression);
    string token;

    while(ss >> token) {
        if (oneArgumentFuncs.find(token) != oneArgumentFuncs.end()) {
            if(st.empty()) { 
                cerr << "Ошибка: Не достаточно операций для" << token << '\n';
                return;
            }
            double operand = st.top();
            st.pop();
            st.push(oneArgumentFuncs[token](operand));
        }
        else if(token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            if (st.size() < 2) {
                cerr << "Ошибка недостаточно операндов для " << token << '\n';
                return;
            }
            double op2 = st.top(); st.pop();
            double op1 = st.top(); st.pop();
            st.push(performOperation(op1,op2,token));
        }
        else {
            try {
                double value = stod(token);
                st.push(value);
            }
            catch (const invalid_argument&){
                cerr << "Ошибка неизвестный токен '" << token << "'" << '\n';
                return;
            }
        }
    }

    if (st.size() != 1) {
        cerr << "Ошибка: Неправильное ОПН" << '\n';
    }
    else {
        cout << "Результат: " << st.top() << '\n';
    }
}
unordered_map<string, int> precedence = {
    {"+", 1},
    {"-", 1},
    {"*", 2},
    {"/", 2},
    {"^", 3},
    {"sqrt", 4},
    {"sin", 4},
    {"cos", 4},
    {"tan", 4},
    {"log", 4},
    {"exp", 4}
};

bool isOperator(const string &s) {
    return (s == "+" || s == "-" || s == "*" || s == "/" || s == "^");
}

bool isOperand(const string &s) {
    for (char c : s) {
        if (!isdigit(c) && c != '.') return false;
    }
    return true;
}

bool isFunction(const string &s) {
    return precedence.find(s) != precedence.end();
}

bool hasHigherPrecedence(const string &op1, const string &op2) {
    return precedence[op1] > precedence[op2];
}

string infixToRPN(const string &expression) {
    stack<string> operators;
    queue<string> output;
    string currentToken = "";
    bool readingFunction = false;

    for (size_t i = 0; i < expression.size(); i++) {
        char token = expression[i];

        if (isspace(token)) {
            continue;  
        }

        if (isdigit(token) || token == '.') {
            currentToken += token;
        } else if (isalpha(token)) {
            currentToken += token;
            readingFunction = true;
        } else {
            if (!currentToken.empty() && !readingFunction) {
                output.push(currentToken);
                currentToken = "";
            }

            if (!currentToken.empty() && readingFunction) {
                if (isFunction(currentToken)) {
                    operators.push(currentToken);  
                }
                currentToken = "";
                readingFunction = false;
            }

            string op(1, token);
            if (op == "(") {
                operators.push(op);  
            } else if (op == ")") {
                while (!operators.empty() && operators.top() != "(") {
                    output.push(operators.top());
                    operators.pop();
                }
                operators.pop();  
            } else if (isOperator(op)) {
                while (!operators.empty() && isOperator(operators.top()) &&
                       hasHigherPrecedence(operators.top(), op)) {
                    output.push(operators.top());
                    operators.pop();
                }
                operators.push(op);
            }
        }
    }

    if (!currentToken.empty()) {
        output.push(currentToken);
    }

    while (!operators.empty()) {
        output.push(operators.top());
        operators.pop();
    }

    stringstream result;
    while (!output.empty()) {
        result << output.front() << " ";
        output.pop();
    }

    return result.str();
}
int main(){
    cout << '\n' << "Введите выражение: " << endl;
    string infixInput = "";
    getline(cin, infixInput); 
    string inputPRN = infixToRPN(infixInput);
    evaluateRPN(inputPRN);
    return 0;
}