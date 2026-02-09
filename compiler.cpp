/*
  Grammar (EBNF) — matches lexer/tokenization and identifier rules:
    program         = "begin", statement_list, "end", "." ;
    statement_list  = statement, { statement } ;
    statement       = assignment_statement ;
    assignment_statement = identifier, "=", expr, ";" ;
    expr            = term, { ("+" | "-"), term } ;
    term            = factor, { ("*" | "/"), factor } ;
    factor          = identifier | number | "(", expr, ")" ;
    number          = digit, { digit } ;
*/

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cctype>

// ---------- Globals ----------
int  charClass;
char lexeme[100];
char nextChar;
int  lexLen;
int  nextToken;
FILE* in_fp;

// ---------- Character classes ----------
#define LETTER  0
#define DIGIT   1
#define UNKNOWN 99

// ---------- Token codes ----------
#define INT_LIT     10
#define IDENT       11
#define ASSIGN_OP   20
#define ADD_OP      21
#define SUB_OP      22
#define MULT_OP     23
#define DIV_OP      24
#define LEFT_PAREN  25
#define RIGHT_PAREN 26
#define UNDERSCORE  28
#define END_PERIOD  29
#define BEGIN       30
#define END         31
#define SEMICOLON   32

// ---------- Lexer declarations ----------
void addChar();
void getChar();
void getNonBlank();
int  lex();
int  lookup(char ch);

// ---------- Parser declarations ----------
void program();
void statement_list();
void statement();
void assignment_statement();
void expr();
void term();
void factor();
void identifier();

// ---------- error ----------
[[noreturn]] void error(const char* message) {
    std::cerr << "Error: " << message << "\n"
              << "NextToken: " << nextToken << "\n"
              << "NextChar: " << (nextChar == (char)EOF ? ' ' : nextChar) << "\n"
              << "Lexeme: " << lexeme << "\n";
    std::exit(1);
}

// ---------- main ----------
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    if ((in_fp = fopen(argv[1], "r")) == NULL) {
        std::cerr << "ERROR - cannot open " << argv[1] << "\n";
        return 1;
    }

    getChar(); // prime first character
    lex();     // prime first token

    program();

    if (nextToken != EOF) {
        error("Unexpected symbols after end of program");
    }

    std::cout << "Parsing completed successfully.\n";
    fclose(in_fp);
    return 0;
}

/*****************************************************/
/* lookup - lookup operators/punct and return token */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '_':
            addChar();
            nextToken = UNDERSCORE;
            break;
        case '.':
            addChar();
            nextToken = END_PERIOD;
            break;
        case ';':
            addChar();
            nextToken = SEMICOLON;
            break;
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;

        case '~': {
            // skip comment: consume chars until newline or EOF
            while (nextChar != '\n' && nextChar != (char)EOF) {
                getChar();
            }
            if (nextChar == '\n') getChar(); // consume newline
            nextToken = UNKNOWN;             // tell lex() to restart
            break;
        }

        default:
            addChar();
            nextToken = EOF;
            break;
    }

    return nextToken;
}

/*****************************************************/
/* addChar - add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = '\0';
    } else {
        error("lexeme is too long");
    }
}

/*****************************************************/
/* getChar - get next character and classify */
// static_cast to force typing
void getChar() {
    int c = getc(in_fp);
    if (c != EOF) {
        nextChar = static_cast<char>(c);
        if (isalpha(static_cast<unsigned char>(nextChar))) {
            charClass = LETTER;
        } else if (isdigit(static_cast<unsigned char>(nextChar))) {
            charClass = DIGIT;
        } else {
            charClass = UNKNOWN;
        }
    } else {
        nextChar = static_cast<char>(EOF);
        charClass = EOF;
    }
}

/*****************************************************/
/* getNonBlank - skip whitespace */
void getNonBlank() {
    while (isspace(static_cast<unsigned char>(nextChar))) {
        getChar();
    }
}

/*****************************************************/
/* lex - lexical analyzer */
int lex() {
    lexLen = 0;
    getNonBlank();

    switch (charClass) {
        case LETTER: {
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }

            if (strcmp(lexeme, "begin") == 0) {
                nextToken = BEGIN;
            } else if (strcmp(lexeme, "end") == 0) {
                nextToken = END;
            } else {
                nextToken = IDENT;
            }
            break;
        }

        case DIGIT: {
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
            break;
        }

        case UNKNOWN: {
            // handle comments here too
            if (nextChar == '~') {
                lookup(nextChar); 
                return lex(); // restart lex after comment
            }

            lookup(nextChar);
            getChar();
            break;
        }

        case EOF:
        default:
            nextToken = EOF;
            strcpy(lexeme, "EOF");
            break;
    }

    return nextToken;
}

/*****************************************************/
/*
program = "begin", statement_list, "end", "." ;
*/
void program() {
    if (nextToken != BEGIN) {
        error("Program must start with 'begin'");
    }
    lex(); // consume BEGIN

    statement_list();

    if (nextToken != END) {
        error("Program must end with 'end'");
    }
    lex(); // consume END

    if (nextToken != END_PERIOD) {
        error("Missing '.' after 'end'");
    }
    lex(); // consume '.'
}

/*****************************************************/
/*
statement_list = statement, { statement } ;
*/
void statement_list() {
    statement();

    while (nextToken == IDENT) {
        statement();
    }
}

/*****************************************************/
/*
statement = assignment_statement ;
*/
void statement() {
    assignment_statement();
}

/*****************************************************/
/*
assignment_statement = identifier, "=", expr, ";" ;
*/
void assignment_statement() {
    identifier();

    if (nextToken != ASSIGN_OP) {
        error("Assignment operator '=' missing in assignment_statement");
    }
    lex(); // consume '='

    expr();

    if (nextToken != SEMICOLON) {
        error("Semicolon ';' missing at end of assignment_statement");
    }
    lex(); // consume ';'
}

/*****************************************************/
/*
expr = term, { ("+" | "-"), term } ;
*/
void expr() {
    term();

    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex(); // consume +/- 
        term();
    }
}

/*****************************************************/
/*
term = factor, { ("*" | "/"), factor } ;
*/
void term() {
    factor();

    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex(); // consume */ 
        factor();
    }
}

/*****************************************************/
/*
factor = identifier | number | "(", expr, ")" ;
*/
void factor() {
    if (nextToken == IDENT) {
        identifier();
        return;
    }

    if (nextToken == INT_LIT) {
        lex(); // consume number
        return;
    }

    if (nextToken == LEFT_PAREN) {
        lex(); // consume '('
        expr();

        if (nextToken != RIGHT_PAREN) {
            error("Right parenthesis ')' expected");
        }
        lex(); // consume ')'
        return;
    }

    error("Expected identifier, number, or '(' in factor");
}

/*****************************************************/
/*
identifier rules:
  - '_' cannot be first (must start with IDENT)
  - '_' can be last
  - no consecutive underscores
*/
void identifier() {
    if (nextToken != IDENT) {
        error("identifier must start with IDENT (letter)");
    }

    lex(); // consume first IDENT chunk

    while (nextToken == UNDERSCORE) {
        lex(); // consume ONE underscore

        if (nextToken == UNDERSCORE) {
            error("Consecutive underscores '__' not allowed in identifier");
        }

        // underscore may be last
        if (nextToken == IDENT || nextToken == INT_LIT) {
            lex(); // consume next IDENT chunk and continue
        } else {
            break; // trailing underscore ends identifier
        }
    }
}
