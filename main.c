#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/***

The language selected for the lexical analyzer  
front. in is C
**/
int lexLen;
int token;
int nextToken;
int charClass;
char lexeme[100];
char nextChar;
FILE * in_fp;
void expression(void);
void term(void);
void factor(void);
int lookup(char ch);
void getNonBlank(void);
int lex(void);
void addChar(void);
void getChar(void);
void error(void);

/* Characters */
#define LETTER 0 //[a-zA-Z]
#define DIGIT 1 //[0-9]
#define SINGLE_QUOTE 2 //[\']
#define DOUBLE_QUOTE 3 //[\"]
#define DECIMAL 4 //[\.]
#define MYSTERY 99 //[^a-zA-Z0-9]

#define FLOAT_LITERAL 9
#define INTEGER_LITERAL 10
#define INDENT 11
#define LEFT_BRACKET 12
#define RIGHT_BRACKET 13
#define SEMICOLON 14
#define COLON 15
#define ASSIGN_OPERATOR 20
#define ADD_OPERATOR 21
#define SUB_OPERATOR 22
#define MULT_OPERATOR 23
#define DIV_OPERATOR 24
#define LEFT_PARENTHESES 25
#define RIGHT_PARENTHESES 26
#define DECIMAL_POINT 27
#define COMMA 28
#define FOR_CODE 30
#define IF_CODE 31
#define ELSE_CODE 32
#define WHILE_CODE 33
#define DO_CODE 34
#define INT_CODE 35
#define FLOAT_CODE 36
#define SWITCH_CODE 37
#define FOREACH_CODE 38

/* Main */
int main(void) {
  if ((in_fp = fopen("front.in", "r")) == NULL) printf("ERROR - cannot open front.in \n");
  else {
    getChar();
    do lex();
    while (nextToken != EOF);
    expression();
  }
  return 0;
}

void boolexpr(void) {
  printf("  ENTER <BOOLEXPR>\n");
  while (nextToken != RIGHT_PARENTHESES) {
    printf("%s ", lexeme);
    lex();
  }
  printf("\n  EXIT <BOOLEXPR> \n");
}
void forboolexpr(void) {
  printf("  ENTER <FORBOOLEXPR>\n");
  while (nextToken != SEMICOLON) {
    printf("    %s ", lexeme);
    lex();
  }
  printf("\n  EXIT <BOOLEXPR> \n");
}
void foreachexpr(void) {
  printf("  ENTER <FOR EACH EXPRESSION>\n");
  while (nextToken != RIGHT_PARENTHESES) {
    lex();
  }
  printf("  EXIT <FOR EXPRESSION>\n");
}
void forexpr(void) {
  printf("  ENTER <FOR EXPRESSION>\n");
  int count = 0;
  while (nextToken != RIGHT_PARENTHESES) {
    if (nextToken == SEMICOLON) {
      count++;
    }
    if (count == 0) {
      //Assignment
      lex();
    } else if (count == 1) {
      forboolexpr();
    } else if (count == 2) {
      //Increment
      lex();
    }
  }
  printf("  EXIT <FOR EXPRESSION>\n");
}
void returnstmt(void) {
  printf("  ENTER <RETURN STATEMENT>\n");
  lex();
  printf("    Returning %s \n", lexeme);
  printf("  EXIT <RETURN STATEMENT>\n");
}
/***
permits any statement to have any amount
or several amts of code
***/
void statement(void) {
  printf("  ENTER <block>\n");
  int count = 0;
/***
using the value count 3 takes into count of 
the incorporation to excuse errors 
**/
  while (nextToken != RIGHT_BRACKET) {
    lex();
    if (nextToken == -1) {
      count++;
      if (count == 3) {
        printf("SYNTAX ERROR:NO CLOSING BRACKET\n");
        error();
      }
    }
  }
  printf("  EXIT <block>\n");
}
void switchblock(void){
  printf("  ENTER <SWITCH block>\n");
  int caseint = 0;
  while (nextToken != RIGHT_BRACKET){
    lex();
    if (nextToken == COLON) {
      printf("    Case %d\n", caseint);
    }
    if (strcmp(lexeme, "break") == 0){
      lex();
      if (nextToken != SEMICOLON) {
        printf("token doesnt = ;\n");
        error();
      }
      else {
        caseint++;
      }
    }
  }
  printf("  EXIT <SWITCH BLOCK>\n");
}
void boolstmt(void) {
  printf("<BOOL STATEMENT>\n");
  lex();
  if (nextToken != LEFT_PARENTHESES) {
    printf("token doesnt = (\n");
    error();
  } else {
    lex();
    boolexpr();
    if (nextToken != RIGHT_PARENTHESES) {
      printf("token doesnt = )\n");
      error();
    } else {
      lex();
      if (nextToken != LEFT_BRACKET) {
        printf("token doesnt = {\n");
        error();
      } else {
        statement();
        if (nextToken != RIGHT_BRACKET) {
          printf("token doesnt = }\n");
          error();
        }
      }
    }
  }
  printf("EXIT <BOOL STATEMENT>\n\n\n");
}
void dowhilestmt() {
  printf("<DOWHILE STATEMENT>\n");
  lex();
  if (nextToken != LEFT_BRACKET) {
    printf("token doesnt = {\n");
    error();
  } else {
    lex();
    statement();
    if (nextToken != RIGHT_BRACKET) {
      printf("token doesnt = }\n");
      error();
    }
    lex();
    if (nextToken != LEFT_PARENTHESES) {
      printf("token doesnt = (\n");
      error();
    } else {
      lex();
      boolexpr();
      if (nextToken != RIGHT_PARENTHESES) {
        printf("token doesnt = )\n");
        error();
      }
    }
  }
  printf("EXIT <DOWHILE STATEMENT>\n\n\n");
}
void forstmt(forEach) {
  if (forEach) {
    printf("<FOR EACH STATEMENT>\n");
    lex();
    if (nextToken != LEFT_PARENTHESES) {
      printf("token doesnt = (\n");
      error();
    } else {
      foreachexpr();
      if (nextToken != RIGHT_PARENTHESES) {
        printf("token doesnt = )\n");
        error();
      } else {
        lex();
        if (nextToken != LEFT_BRACKET) {
          printf("token doesnt = {\n");
          error();
        } else {
          statement();
          if (nextToken != RIGHT_BRACKET) {
            printf("token doesnt = }\n");
            error();
          }
        }
      }
    }
    printf("EXIT <FOR EACH STATEMENT> \n\n\n");
  } else {
    //Run code for forStatement
    printf("<FOR STATEMENT>\n");
    lex();
    if (nextToken != LEFT_PARENTHESES) {
      printf("token doesnt = (\n");
      error();
    } else {
      forexpr();
      lex();
      if (nextToken != LEFT_BRACKET) {
        printf("token doesnt = {\n");
        error();
      } else {
        statement();
        if (nextToken != RIGHT_BRACKET) {
          printf("token doesnt = }\n");
          error();
        }
      }
    }
    printf("EXIT <FOR STATEMENT> \n\n\n");
  }
}
void switchstmt() {
  printf("<SWITCH STATEMENT>\n");
  lex();
  if (nextToken != LEFT_PARENTHESES){
    printf("token doesnt = (\n");
    error();
  } else {
    //Todo: Check value and iterate accordingly
    lex();
    lex();
    if (nextToken != RIGHT_PARENTHESES){
      printf("token doesnt = )\n");
      error();
    } else {
      switchblock();
      if (nextToken != RIGHT_BRACKET){
        printf("token doesnt =\n");
        error();
      }
    }
  }
  printf("EXIT <switch statement>\n\n\n");
}

int lookup(char ch) {
  addChar();
  switch (ch) {
  case '(':
    nextToken = LEFT_PARENTHESES;
    break;
  case ')':
    nextToken = RIGHT_PARENTHESES;
    break;
  case '+':
    nextToken = ADD_OPERATOR;
    break;
  case '-':
    nextToken = SUB_OPERATOR;
    break;
  case '*':
    nextToken = MULT_OPERATOR;
    break;
  case '/':
    nextToken = DIV_OPERATOR;
    break;
  case '=':
    nextToken = ASSIGN_OPERATOR;
    break;
  case '.':
    nextToken = DECIMAL_POINT;
    break;
  case ',':
    nextToken = COMMA;
    break;
  case '\'':
    nextToken = SINGLE_QUOTE;
    break;
  case '\"':
    nextToken = DOUBLE_QUOTE;
    break;
  case '{':
    nextToken = LEFT_BRACKET;
    break;
  case '}':
    nextToken = RIGHT_BRACKET;
    break;
  case ';':
    nextToken = SEMICOLON;
    break;
  case ':':
    nextToken = COLON;
    break;
  default:
    nextToken = EOF;
    break;
  }
  return nextToken;
}
void getChar(void) {
  if ((nextChar = getc(in_fp)) != EOF) {
    if (isalpha(nextChar)) charClass = LETTER;
    else if (isdigit(nextChar)) charClass = DIGIT;
    else charClass = MYSTERY;
  } else charClass = EOF;
}
void addChar(void) {
  if (lexLen <= 98) {
    lexeme[lexLen++] = nextChar;
    lexeme[lexLen] = '\0';
  } else printf("Error - lexeme is too long \n");
}
void getNonBlank(void) {
  while (isspace(nextChar)) getChar();
}
int statementtoken() {
  if (strcmp(lexeme, "if") == 0) {
    boolstmt();
  } else if (strcmp(lexeme, "for") == 0) {
    forstmt(false);
  } else if (strcmp(lexeme, "else") == 0) return ELSE_CODE;
  else if (strcmp(lexeme, "while") == 0) {
    boolstmt();
  } else if (strcmp(lexeme, "dowhile") == 0) {
    dowhilestmt();
  } else if (strcmp(lexeme, "int") == 0) return INT_CODE;
  else if (strcmp(lexeme, "float") == 0) return FLOAT_CODE;
  else if (strcmp(lexeme, "switch") == 0) {
    switchstmt();
    return SWITCH_CODE;
    }
  else if (strcmp(lexeme, "foreach") == 0) {
    forstmt(true);
    //return FOREACH_CODE;
  } else if (strcmp(lexeme, "return") == 0) returnstmt();
  return 0;
}
/* Interpret expressions */
int lex(void) {
  lexLen = 0;
  getNonBlank();
  switch (charClass) {
  case LETTER:
    addChar();
    getChar();
    while (charClass == LETTER || charClass == DIGIT) {
      addChar();
      getChar();
    }
    nextToken = statementtoken();
    break;
  case DIGIT:
    addChar();
    getChar();
    while (charClass == DIGIT) {
      addChar();
      getChar();
    }
    if (charClass == MYSTERY && nextChar == '.') {
      addChar();
      getChar();
      while (charClass == DIGIT) {
        addChar();
        getChar();
      }
      nextToken = FLOAT_LITERAL;
    } else nextToken = INTEGER_LITERAL;
    break;
  case DECIMAL:
    addChar();
    getChar();
    if (charClass == DIGIT) {
      addChar();
      getChar();
      while (charClass == DIGIT) {
        addChar();
        getChar();
      }
      nextToken = FLOAT_LITERAL;
    } else nextToken = DECIMAL_POINT;
    break;
  case MYSTERY:
    lookup(nextChar);
    getChar();
    break;
  case EOF:
    nextToken = EOF;
    lexeme[0] = 'E';
    lexeme[1] = 'O';
    lexeme[2] = 'F';
    lexeme[3] = '\0';
    break;
  }
printf("The token that is coming up is: %d, Next lexeme is %s\n", nextToken, lexeme);
  return nextToken;
}


void expression(void) {
  term();
  while (nextToken == ADD_OPERATOR || nextToken == SUB_OPERATOR) {
    lex();
    term();
  }
}

void term(void) {
  factor();
  while (nextToken == MULT_OPERATOR || nextToken == DIV_OPERATOR) {
    lex();
    factor();
  }
}


void factor(void) {
  if (nextToken == INDENT || nextToken == INTEGER_LITERAL || nextToken == FLOAT_LITERAL) lex();
  else {
    if (nextToken == LEFT_PARENTHESES) {
      lex();
      expression();
      if (nextToken == RIGHT_PARENTHESES) lex();
      else error();
    } else error();
  }
  printf("Exit <factor>\n");
}

void error(void) {
  exit(1);
}