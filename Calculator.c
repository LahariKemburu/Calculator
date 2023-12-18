#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define MAX_SIZE 101

double calculateFunction(char func, double operand);

struct PostfixNode {
    double data;
    bool isOperator;
    struct PostfixNode* next;
};

char operatorStack[MAX_SIZE];
double operandStack[MAX_SIZE];
int topOperand = -1;
int topOperator = -1;

void pushOperator(char operator) {
    if (topOperator == MAX_SIZE - 1) {
        return;
    }
    operatorStack[++topOperator] = operator;
}

char popOperator() {
    if (topOperator == -1) {
        return '\0';
    }
    return operatorStack[topOperator--];
}

void pushOperand(double operand) {
    if (topOperand == MAX_SIZE - 1) {
        return;
    }
    operandStack[++topOperand] = operand;
}

double popOperand() {
    if (topOperand == -1) {
        return 0;
    }
    return operandStack[topOperand--];
}

int returnPriority(char operator) {
    switch (operator) {
        case '^': return 3;
        case '%':
        case '*':
        case '/': return 2;
        case '+':
        case '-': return 1;
        case 's':
        case 'l': return 4; // square root and log
        default: return 0;
    }
}

bool isOperator(char character) {
    if (character == '+' || character == '-' || character == '*' || character == '/' || character == '%' || character == '^' || character == 's' || character == 'l') {
        return true;
    }
    return false;
}

double convertToDouble(char* str) {
    double number = 0;
    bool hasFractionalPart = false;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            hasFractionalPart = true;
            break;
        }
        number *= 10;
        number += (str[i] - '0');
    }
    double fractionalPart = 0;
    if (hasFractionalPart) {
        for (int i = strlen(str) - 1; str[i] != '.'; i--) {
            fractionalPart += (str[i] - '0');
            fractionalPart /= 10;
        }
    }
    number += fractionalPart;
    return number;
}

char* infixToPostfix(char infixExpression[]) {
    int i = 0, j = 0;
    char* postfixExpression = (char*)malloc(MAX_SIZE * sizeof(char));
    while (infixExpression[i] != '\0') {
        if (infixExpression[i] != ' ') {
            if (infixExpression[i] == '(') {
                pushOperator(infixExpression[i]);
            } else if (infixExpression[i] == ')') {
                while (topOperator >= 0 && operatorStack[topOperator] != '(') {
                    postfixExpression[j++] = popOperator();
                }
                if (operatorStack[topOperator] == '(') {
                    popOperator();
                }
            } else if (isOperator(infixExpression[i])) {
                if ((topOperator != -1 && returnPriority(infixExpression[i]) > returnPriority(operatorStack[topOperator])) || topOperator == -1) {
                    pushOperator(infixExpression[i]);
                    postfixExpression[j++] = ' ';
                } else if (topOperator != -1 && returnPriority(infixExpression[i]) <= returnPriority(operatorStack[topOperator])) {
                    while (returnPriority(infixExpression[i]) <= returnPriority(operatorStack[topOperator])) {
                        postfixExpression[j++] = popOperator();
                    }
                    pushOperator(infixExpression[i]);
                }
            } else {
                postfixExpression[j++] = infixExpression[i];
            }
        }
        i++;
    }
    while (topOperator >= 0) {
        postfixExpression[j++] = popOperator();
    }
    postfixExpression[j] = '\0';
    return postfixExpression;
}

struct PostfixNode* createNumNode(char* str) {
    struct PostfixNode* newNode = (struct PostfixNode*)malloc(sizeof(struct PostfixNode));
    newNode->data = convertToDouble(str);
    newNode->next = NULL;
    newNode->isOperator = false;
    return newNode;
}

struct PostfixNode* createOperatorNode(char operator) {
    struct PostfixNode* newNode = (struct PostfixNode*)malloc(sizeof(struct PostfixNode));
    newNode->data = operator;
    newNode->next = NULL;
    newNode->isOperator = true;
    return newNode;
}

void evaluateStack(char operator) {
    if (operator == 's' || operator == 'l') {
        double operand = popOperand();
        pushOperand(calculateFunction(operator, operand));
    } else {
        double num1, num2;
        num2 = popOperand();
        num1 = popOperand();
        switch (operator) {
            case '^':
                pushOperand(pow(num1, num2));
                break;
            case '%':
                pushOperand((int)num1 % (int)num2);
                break;
            case '*':
                pushOperand(num1 * num2);
                break;
            case '/':
                pushOperand(num1 / num2);
                break;
            case '+':
                pushOperand(num1 + num2);
                break;
            case '-':
                pushOperand(num1 - num2);
                break;
        }
    }
}

double calculateFunction(char func, double operand) {
    switch (func) {
        case 's':
            return sqrt(operand);
        case 'l':
            return log(operand);
        default:
            return 0.0;
    }
}

double evaluateExpression(char* expression) {
    struct PostfixNode* head = NULL;
    struct PostfixNode* topNode = NULL;
    char str[10];
    int j = 0;
    for (int i = 0; i < strlen(expression); i++) {
        if (isOperator(expression[i])) {
            if (j > 0) {
                str[j] = '\0';
                j = 0;
                if (head == NULL) {
                    head = createNumNode(str);
                    topNode = head;
                } else {
                    topNode->next = createNumNode(str);
                    topNode = topNode->next;
                }
            }
            topNode->next = createOperatorNode(expression[i]);
            topNode = topNode->next;
        } else if (expression[i] == ' ') {
            if (j > 0) {
                str[j] = '\0';
                j = 0;
                if (head == NULL) {
                    head = createNumNode(str);
                    topNode = head;
                } else {
                    topNode->next = createNumNode(str);
                    topNode = topNode->next;
                }
            }
        } else {
            str[j++] = expression[i];
        }
    }

    struct PostfixNode* temp = head;
    while (temp != NULL) {
        if (temp->isOperator) {
            evaluateStack((int)temp->data);
        } else {
            pushOperand(temp->data);
        }
        temp = temp->next;
    }
    double answer = popOperand();
    return answer;
}

int main() {
    char* infixExpression = (char*)malloc(MAX_SIZE * sizeof(char));
    char* postfixExpression;
    char choice;
    double result;
    printf("Enter:\n1 to give infix expression\n2 for postfix expression\n");
    scanf(" %c", &choice);

    switch (choice) {
        case '1': {
            printf("Enter the expression:");
            scanf(" %[^\n]", infixExpression);
            postfixExpression = (char*)malloc((MAX_SIZE + 1) * sizeof(char)); // Allocate memory for postfixExpression
            postfixExpression = infixToPostfix(infixExpression);
            result = evaluateExpression(postfixExpression);
            printf("The answer for the given expression %s is %.2lf\n", infixExpression, result);
            break;
        }
        case '2': {
            printf("Enter the postfix expression:");
            postfixExpression = (char*)malloc((MAX_SIZE + 1) * sizeof(char));
            scanf(" %[^\n]", postfixExpression);
            result = evaluateExpression(postfixExpression);
            printf("The answer for the given expression %s is %.2lf\n", postfixExpression, result);
            break;
        }
    }
    free(infixExpression);
    free(postfixExpression);

    return 0;
}

