%{

#include <iostream>

#include "util/token.h"
#include "model/instance.h"
#include "controller/exceptions.h"

#define YYERROR_VERBOSE


int ttoi(Token* token);
void yyerror(char const *s);

int error_counter = 0;
ifstream* lexin;
Instance* yyinstance;

%}

%union {
    Token *token;
}

%token <token> T_INT
%token <token> T_STRING
%token <token> T_LINE_SEP

%%

instance		: /* no statements */
				| declarations
				;

declarations	: declaration T_LINE_SEP
				| declarations declaration T_LINE_SEP
				;

declaration		: /* empty */
                                | 'R' T_INT T_INT T_STRING		{ try { yyinstance->addResource(   $2->value, $3->value, $4->text ); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'J' T_INT T_INT T_INT T_STRING	{ try { yyinstance->addJob(        $2->value, $3->value, $4->value, $5->text); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'D' T_INT T_INT T_INT T_STRING	{ try { yyinstance->addDummyJob(        $2->value, $3->value, $4->value); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                    /* next alternative is for compatibility with older testsets which
                                     * may use Trains instead of Jobs */
                                | 'T' T_INT T_INT T_INT T_STRING	{ try { yyinstance->addJob(        $2->value, $3->value, $4->value, $5->text ); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'A' T_INT T_INT T_INT T_STRING	{ try { yyinstance->addActivity(   $2->value, $3->value, -1       , -1       , $4->value, $5->text, -1 ); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'A' T_INT T_INT T_INT T_STRING T_INT	{ try { yyinstance->addActivity(   $2->value, $3->value, -1       , -1       , $4->value, $5->text, $6->value ); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'A' T_INT T_INT T_INT T_STRING T_INT T_INT T_INT	{ try { yyinstance->addActivity(   $2->value, $3->value, $7->value   , $8->value, $4->value, $5->text, $6->value ); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'Q' T_INT T_INT T_INT T_INT		{ try { yyinstance->addRequirement($2->value, $3->value, $4->value, $5->value); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'P' T_INT T_INT T_INT T_INT		{ try { yyinstance->addPrecedence( $2->value, $3->value, $4->value, $5->value, true, 0); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'S' T_INT T_INT T_INT T_INT		{ try { yyinstance->addPrecedence( $2->value, $3->value, $4->value, $5->value, false); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'F' T_INT T_INT T_INT { try { yyinstance->setFlex($2->value, $3->value, $4->value); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                | 'G' T_INT T_INT T_INT T_INT { try { yyinstance->mergeGroup( $2->value, $3->value, $4->value, $5->value ); } catch(InstanceManipulationException const& e) { e.printWarning(); } }
                                ;

%%

void yyerror (char const *s) {
	++error_counter;
        cerr << "error: at token [" << yylval.token->text.toStdString().c_str() << "]: " << s << endl;
}

int yylex ()
{
	char ch;

	do {
                ch = lexin->peek ();
                if (isupper(ch) || ch == 'p') {
                        yylval.token = new Token;
                        yylval.token->type = ch;
                        yylval.token->text.append(ch);

                        lexin->get ();
			return ch;
		}
		else if (isdigit (ch)) {
			yylval.token = new Token;
			yylval.token->type = T_INT;

			yylval.token->value = 0;

                        while (!lexin->eof () && isdigit (ch)) {
                                lexin->get ();

				yylval.token->value = yylval.token->value * 10 + ch - '0';
                                yylval.token->text.append(ch);

                                ch = lexin->peek ();
			}

			return T_INT;
		}
		else if (ch == '"') {
                        yylval.token = new Token;
			yylval.token->type = T_STRING;

                        lexin->get ();
                        ch = lexin->peek ();
                        while (!lexin->eof () && ch != '"') {
                                yylval.token->text.append(ch);
                                lexin->get ();
                                ch = lexin->peek ();
			}
                        lexin->get(); // eat closing '"'

			return T_STRING;
		}
                else if (ch == '\n') {
                        lexin->get();
                        return T_LINE_SEP;
                } else {
                        lexin->get ();
                }
        } while (!lexin->eof ());
	
	return -1;
}
