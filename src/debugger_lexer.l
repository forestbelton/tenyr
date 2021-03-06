%{
#include <stdio.h>
#include <ctype.h>

#include "common.h"
#include "debugger_global.h"
#include "debugger_parser.h"

int tdbg_error(YYLTYPE *locp, struct debugger_data *dd, const char *s);

static int savestr(yyscan_t yyscanner);

#define savecol  yyextra->lexstate.savecol
#define saveline yyextra->lexstate.saveline

#define YY_USER_ACTION \
    do { \
        int len = MIN(yyleng, sizeof saveline - savecol); \
        strcopy(&saveline[savecol], yytext, len + 1); \
        saveline[savecol += len] = 0; \
        yylloc->first_line = yylloc->last_line = yylineno; \
        yylloc->first_column = yycolumn; \
        yylloc->last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng; \
    } while (0);

%}

%option reentrant bison-bridge bison-locations
%option noyywrap always-interactive yylineno
%option extra-type="struct debugger_data *"
%option prefix="tdbg_"

regname ($[A-Pa-p])
hexnum  (-?"0"[xX][0-9a-fA-F]+)
octnum  (-?"0"[0-7]+)
decnum  (-?[0-9]+)
ident   ([A-Za-z0-9_]+)
inspace ([ \t]+)
trailsp ([ \t]*$)

%%

"\t"                    { yycolumn += 7; /* 1 added in YY_USER_ACTION */ }
"\n"                    { saveline[savecol = 0] = 0; return NL; }

{regname}               { yylval->chr = toupper(yytext[1]); return REGISTER; }

{hexnum}                { savestr(yyscanner); return INTEGER; }
{octnum}                { savestr(yyscanner); return INTEGER; }
{decnum}                { savestr(yyscanner); return INTEGER; }

[*bcdipqx/]             { return yytext[0]; }

si                      { return STEPI; }
print                   { return PRINT; }
quit                    { return 'q'; }
display                 { return DISPLAY; }
info                    { return INFO; }

{ident}                 { savestr(yyscanner); return IDENT; }

[\t\f\v\r ]+            { return WHITESPACE; }

.                       { return UNKNOWN; }

%%

static int savestr(yyscan_t yyscanner)
{
    int len          = yyget_leng(yyscanner);
    YYSTYPE *lval    = yyget_lval(yyscanner);
    YYLTYPE *lloc    = yyget_lloc(yyscanner);
    const char *text = yyget_text(yyscanner);
    void *extra      = yyget_extra(yyscanner);

    if (len >= (signed)sizeof lval->str) {
        tdbg_error(lloc, extra, "token too long");
        return 1;
    } else {
        memset(lval->str, 0, sizeof lval->str);
        strcopy(lval->str, text, len + 1);
        return 0;
    }
}

