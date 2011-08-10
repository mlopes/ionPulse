%{


#include "phpParser/ionParserLib.h"
#include "phpparser.h"

#define YYERROR_VERBOSE
#define YYSTYPE pASTNode
#define ion_php_scanner context->__scanner


#define yyparse ion_php_parse
#define yylex   context->__lex
#define yyerror context->__error
#define yylval  ion_php_lval
#define yychar  ion_php_char
#define yydebug ion_php_debug
#define yynerrs ion_php_nerrs

%}

%pure_parser
%glr-parser
%expect 2
%parse-param { IonPhp::phpParser* context }
%lex-param   { void* ion_php_scanner  }

%left T_INCLUDE T_INCLUDE_ONCE T_EVAL T_REQUIRE T_REQUIRE_ONCE
%left ','
%left T_LOGICAL_OR
%left T_LOGICAL_XOR
%left T_LOGICAL_AND
%right T_PRINT
%left '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL
%left '?' ':'
%left T_BOOLEAN_OR
%left T_BOOLEAN_AND
%left '|'
%left '^'
%left '&'
%nonassoc T_IS_EQUAL T_IS_NOT_EQUAL T_IS_IDENTICAL T_IS_NOT_IDENTICAL
%nonassoc '<' T_IS_SMALLER_OR_EQUAL '>' T_IS_GREATER_OR_EQUAL
%left T_SL T_SR
%left '+' '-' '.'
%left '*' '/' '%'
%right '!'
%nonassoc T_INSTANCEOF
%right '~' T_INC T_DEC T_INT_CAST T_DOUBLE_CAST T_STRING_CAST T_ARRAY_CAST T_OBJECT_CAST T_BOOL_CAST T_UNSET_CAST '@'
%right '['
%nonassoc T_NEW T_CLONE
%token T_EXIT
%token T_IF
%left T_ELSEIF
%left T_ELSE
%left T_ENDIF
%token T_LNUMBER
%token T_DNUMBER
%token T_STRING
%token T_STRING_VARNAME
%token T_VARIABLE
%token T_NUM_STRING
%token T_INLINE_HTML
%token T_CHARACTER
%token T_BAD_CHARACTER
%token T_ENCAPSED_AND_WHITESPACE
%token T_CONSTANT_ENCAPSED_STRING
%token T_ECHO
%token T_DO
%token T_WHILE
%token T_ENDWHILE
%token T_FOR
%token T_ENDFOR
%token T_FOREACH
%token T_ENDFOREACH
%token T_DECLARE
%token T_ENDDECLARE
%token T_AS
%token T_SWITCH
%token T_ENDSWITCH
%token T_CASE
%token T_DEFAULT
%token T_BREAK
%token T_CONTINUE
%token T_GOTO
%token T_FUNCTION
%token T_CONST
%token T_RETURN
%token T_TRY
%token T_CATCH
%token T_THROW
%token T_USE
%token T_GLOBAL
%right T_STATIC T_ABSTRACT T_FINAL T_PRIVATE T_PROTECTED T_PUBLIC
%token T_VAR
%token T_UNSET
%token T_ISSET
%token T_EMPTY
%token T_HALT_COMPILER
%token T_CLASS
%token T_INTERFACE
%token T_EXTENDS
%token T_IMPLEMENTS
%token T_OBJECT_OPERATOR
%token T_DOUBLE_ARROW
%token T_LIST
%token T_ARRAY
%token T_CLASS_C
%token T_METHOD_C
%token T_FUNC_C
%token T_LINE
%token T_FILE
%token T_COMMENT
%token T_DOC_COMMENT
%token T_OPEN_TAG
%token T_OPEN_TAG_WITH_ECHO
%token T_CLOSE_TAG
%token T_WHITESPACE
%token T_START_HEREDOC
%token T_END_HEREDOC
%token T_DOLLAR_OPEN_CURLY_BRACES
%token T_CURLY_OPEN
%token T_PAAMAYIM_NEKUDOTAYIM
%token T_NAMESPACE
%token T_NS_C
%token T_DIR
%token T_NS_SEPARATOR

%% /* Rules */

%start start;

start: top_statement_list {context->__result = $1;};

top_statement_list:
                top_statement_list top_statement {if ($2) { $1->addChild($2);}}
        |	/* empty */ {$$ = ASTNode::create("top_statement_list");}
;

namespace_name:
                T_STRING { $$ = ASTNode::create("namespace_name")->addChild($1); }
        |	namespace_name T_NS_SEPARATOR T_STRING { $1->addChild($3); }
;

top_statement:
                statement
        |	function_declaration_statement
        |	class_declaration_statement
        |	T_HALT_COMPILER '(' ')' ';'      { $$ = $1; YYACCEPT; }
        |	T_NAMESPACE namespace_name ';'	{ $$ = ASTNode::create("namespace")->addChild($2); }
        |	T_NAMESPACE namespace_name '{'
                top_statement_list '}'		{ $$ = ASTNode::create("namespace")->addChild($2)->addChild($4); }
        |	T_NAMESPACE '{'
                top_statement_list '}'		{ $$ = ASTNode::create("namespace")->addChild($3); }
        |	T_USE use_declarations ';'       { $$ = ASTNode::create("use")->addChild($2); }
        |	constant_declaration ';'
;

use_declarations:
                use_declarations ',' use_declaration
        |	use_declaration
;

use_declaration:
                namespace_name
        |	namespace_name T_AS T_STRING	{ $$ = ASTNode::create("as")->addChild($1)->addChild($3); }
        |	T_NS_SEPARATOR namespace_name    { $$ = ASTNode::create("namespaceroot")->addChild($2); }
        |	T_NS_SEPARATOR namespace_name T_AS T_STRING { $$ = ASTNode::create("as")->addChild(ASTNode::create("namespaceroot")->addChild($2))->addChild($4); }
;

constant_declaration:
                constant_declaration ',' T_STRING '=' static_scalar
        |	T_CONST T_STRING '=' static_scalar
;

inner_statement_list:
                inner_statement_list  inner_statement
        |	/* empty */
;


inner_statement:
                statement
        |	function_declaration_statement
        |	class_declaration_statement
;


statement:
                unticked_statement
        |	T_STRING ':'
;

unticked_statement:
                '{' inner_statement_list '}'
        |	T_IF '(' expr ')'  statement  elseif_list else_single
        |	T_IF '(' expr ')' ':'  inner_statement_list  new_elseif_list new_else_single T_ENDIF ';'
        |	T_WHILE '('  expr  ')' while_statement
        |	T_DO  statement T_WHILE '(' expr ')' ';'
        |	T_FOR
                        '('
                                for_expr
                        ';'
                                for_expr
                        ';'
                                for_expr
                        ')'
                        for_statement
        |	T_SWITCH '(' expr ')'	 switch_case_list
        |	T_BREAK ';'
        |	T_BREAK expr ';'
        |	T_CONTINUE ';'
        |	T_CONTINUE expr ';'
        |	T_RETURN ';'
        |	T_RETURN expr_without_variable ';'
        |	T_RETURN variable ';'
        |	T_GLOBAL global_var_list ';'
        |	T_STATIC static_var_list ';'
        |	T_ECHO echo_expr_list ';' {$$=$2;}
        |	T_INLINE_HTML
        |	expr ';'
        |	T_UNSET '(' unset_variables ')' ';'
        |	T_FOREACH '(' variable T_AS

                foreach_variable foreach_optional_arg ')'
                foreach_statement
        |	T_FOREACH '(' expr_without_variable T_AS

                variable foreach_optional_arg ')'
                foreach_statement
        |	T_DECLARE  '(' declare_list ')' declare_statement
        |	';'		/* empty statement */
        |	T_TRY  '{' inner_statement_list '}'
                T_CATCH '('
                fully_qualified_class_name
                T_VARIABLE ')'
                '{' inner_statement_list '}'
                additional_catches
        |	T_THROW expr ';'
        |	T_GOTO T_STRING ';'
;


additional_catches:
                non_empty_additional_catches { $$ = $1; }
        |	/* empty */
;

non_empty_additional_catches:
                additional_catch { $$ = $1; }
        |	non_empty_additional_catches additional_catch { $$ = $2; }
;


additional_catch:
        T_CATCH '(' fully_qualified_class_name  T_VARIABLE ')'  '{' inner_statement_list '}'
;


unset_variables:
                unset_variable
        |	unset_variables ',' unset_variable
;

unset_variable:
                variable
;

function_declaration_statement:
                unticked_function_declaration_statement
;

class_declaration_statement:
                unticked_class_declaration_statement
;


is_reference:
                /* empty */
        |	'&'
;


unticked_function_declaration_statement:
                function is_reference T_STRING
                        '(' parameter_list ')' '{' inner_statement_list '}'
;

unticked_class_declaration_statement:
                class_entry_type T_STRING extends_from

                        implements_list
                        '{'
                                class_statement_list
                        '}'
        |	interface_entry T_STRING

                        interface_extends_list
                        '{'
                                class_statement_list
                        '}'
;


class_entry_type:
                T_CLASS
        |	T_ABSTRACT T_CLASS
        |	T_FINAL T_CLASS
;

extends_from:
                /* empty */
        |	T_EXTENDS fully_qualified_class_name
;

interface_entry:
        T_INTERFACE
;

interface_extends_list:
                /* empty */
        |	T_EXTENDS interface_list
;

implements_list:
                /* empty */
        |	T_IMPLEMENTS interface_list
;

interface_list:
                fully_qualified_class_name
        |	interface_list ',' fully_qualified_class_name
;

foreach_optional_arg:
                /* empty */
        |	T_DOUBLE_ARROW foreach_variable	{ $$ = $2; }
;


foreach_variable:
                variable
        |	'&' variable
;

for_statement:
                statement
        |	':' inner_statement_list T_ENDFOR ';'
;


foreach_statement:
                statement
        |	':' inner_statement_list T_ENDFOREACH ';'
;


declare_statement:
                statement
        |	':' inner_statement_list T_ENDDECLARE ';'
;


declare_list:
                T_STRING '=' static_scalar
        |	declare_list ',' T_STRING '=' static_scalar
;


switch_case_list:
                '{' case_list '}'					{ $$ = $2; }
        |	'{' ';' case_list '}'				{ $$ = $3; }
        |	':' case_list T_ENDSWITCH ';'		{ $$ = $2; }
        |	':' ';' case_list T_ENDSWITCH ';'	{ $$ = $3; }
;


case_list:
                /* empty */
        |	case_list T_CASE expr case_separator  inner_statement_list
        |	case_list T_DEFAULT case_separator  inner_statement_list
;


case_separator:
                ':'
        |	';'
;


while_statement:
                statement
        |	':' inner_statement_list T_ENDWHILE ';'
;



elseif_list:
                /* empty */
        |	elseif_list T_ELSEIF '(' expr ')'  statement
;


new_elseif_list:
                /* empty */
        |	new_elseif_list T_ELSEIF '(' expr ')' ':'  inner_statement_list
;


else_single:
                /* empty */
        |	T_ELSE statement
;


new_else_single:
                /* empty */
        |	T_ELSE ':' inner_statement_list
;


parameter_list:
                non_empty_parameter_list
        |	/* empty */
;


non_empty_parameter_list:
                optional_class_type T_VARIABLE
        |	optional_class_type '&' T_VARIABLE
        |	optional_class_type '&' T_VARIABLE '=' static_scalar
        |	optional_class_type T_VARIABLE '=' static_scalar
        |	non_empty_parameter_list ',' optional_class_type T_VARIABLE
        |	non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE
        |	non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE	 '=' static_scalar
        |	non_empty_parameter_list ',' optional_class_type T_VARIABLE '=' static_scalar
;


optional_class_type:
                /* empty */
        |	fully_qualified_class_name	{ $$ = $1; }
        |	T_ARRAY
;


function_call_parameter_list:
                non_empty_function_call_parameter_list
        |	/* empty */ {$$=ASTNode::create("function_call_parameter_list");}
;


non_empty_function_call_parameter_list:
                expr_without_variable {$$=ASTNode::create("function_call_parameter_list")->addChild($1);}
        |	variable               {$$=ASTNode::create("function_call_parameter_list")->addChild($1);}
        |	'&' w_variable         {$$=ASTNode::create("function_call_parameter_list")->addChild($2);$2->setData("reference", "1");}
        |	non_empty_function_call_parameter_list ',' expr_without_variable {$1->addChild($3);}
        |	non_empty_function_call_parameter_list ',' variable              {$1->addChild($3);}
        |	non_empty_function_call_parameter_list ',' '&' w_variable        {$1->addChild($4);$4->setData("reference", "1");}
;

global_var_list:
                global_var_list ',' global_var
        |	global_var
;


global_var:
                T_VARIABLE			{ $$ = $1; }
        |	'$' r_variable		{ $$ = $2; }
        |	'$' '{' expr '}'	{ $$ = $3; }
;


static_var_list:
                static_var_list ',' T_VARIABLE
        |	static_var_list ',' T_VARIABLE '=' static_scalar
        |	T_VARIABLE
        |	T_VARIABLE '=' static_scalar

;


class_statement_list:
                class_statement_list class_statement
        |	/* empty */
;


class_statement:
                variable_modifiers  class_variable_declaration ';'
        |	class_constant_declaration ';'
        |	method_modifiers function is_reference T_STRING  '('
                        parameter_list ')' method_body
;


method_body:
                ';' /* abstract method */
        |	'{' inner_statement_list '}'
;

variable_modifiers:
                non_empty_member_modifiers		{ $$ = $1; }
        |	T_VAR
;

method_modifiers:
                /* empty */
        |	non_empty_member_modifiers
;

non_empty_member_modifiers:
                member_modifier						{ $$ = $1; }
        |	non_empty_member_modifiers member_modifier
;

member_modifier:
                T_PUBLIC
        |	T_PROTECTED
        |	T_PRIVATE
        |	T_STATIC
        |	T_ABSTRACT
        |	T_FINAL
;

class_variable_declaration:
                class_variable_declaration ',' T_VARIABLE
        |	class_variable_declaration ',' T_VARIABLE '=' static_scalar
        |	T_VARIABLE
        |	T_VARIABLE '=' static_scalar
;

class_constant_declaration:
                class_constant_declaration ',' T_STRING '=' static_scalar
        |	T_CONST T_STRING '=' static_scalar
;

echo_expr_list:
                echo_expr_list ',' expr {$1->addChild($2);}
        |	expr {$$ = ASTNode::create("echo_expr_list")->addChild($1);}
;


for_expr:
                /* empty */
        |	non_empty_for_expr	{ $$ = $1; }
;

non_empty_for_expr:
                non_empty_for_expr ','	 expr
        |	expr					{ $$ = $1; }
;

expr_without_variable:
                T_LIST '('  assignment_list ')' '=' expr
        |	variable '=' expr {$$=ASTNode::create("assignment")->addChild($1)->addChild($3);}
        |	variable '=' '&' variable
        |	variable '=' '&' T_NEW class_name_reference
        |	T_NEW class_name_reference  ctor_arguments
        |	T_CLONE expr
        |	variable T_PLUS_EQUAL expr
        |	variable T_MINUS_EQUAL expr
        |	variable T_MUL_EQUAL expr
        |	variable T_DIV_EQUAL expr
        |	variable T_CONCAT_EQUAL expr
        |	variable T_MOD_EQUAL expr
        |	variable T_AND_EQUAL expr
        |	variable T_OR_EQUAL expr
        |	variable T_XOR_EQUAL expr
        |	variable T_SL_EQUAL expr
        |	variable T_SR_EQUAL expr
        |	rw_variable T_INC
        |	T_INC rw_variable
        |	rw_variable T_DEC
        |	T_DEC rw_variable
        |	expr T_BOOLEAN_OR  expr
        |	expr T_BOOLEAN_AND  expr
        |	expr T_LOGICAL_OR  expr
        |	expr T_LOGICAL_AND  expr
        |	expr T_LOGICAL_XOR expr
        |	expr '|' expr
        |	expr '&' expr
        |	expr '^' expr
        |	expr '.' expr
        |	expr '+' expr
        |	expr '-' expr
        |	expr '*' expr
        |	expr '/' expr
        |	expr '%' expr
        | 	expr T_SL expr
        |	expr T_SR expr
        |	'+' expr %prec T_INC
        |	'-' expr %prec T_INC
        |	'!' expr
        |	'~' expr
        |	expr T_IS_IDENTICAL expr
        |	expr T_IS_NOT_IDENTICAL expr
        |	expr T_IS_EQUAL expr
        |	expr T_IS_NOT_EQUAL expr
        |	expr '<' expr
        |	expr T_IS_SMALLER_OR_EQUAL expr
        |	expr '>' expr
        |	expr T_IS_GREATER_OR_EQUAL expr
        |	expr T_INSTANCEOF class_name_reference
        |	'(' expr ')' 	{ $$ = $2; }
        |	expr '?'
                expr ':'
                expr
        |	expr '?' ':'
                expr
        |	internal_functions_in_yacc { $$ = $1; }
        |	T_INT_CAST expr
        |	T_DOUBLE_CAST expr
        |	T_STRING_CAST expr
        |	T_ARRAY_CAST expr
        |	T_OBJECT_CAST expr
        |	T_BOOL_CAST expr
        |	T_UNSET_CAST expr
        |	T_EXIT exit_expr
        |	'@'  expr
        |	scalar				{ $$ = $1; }
        |	T_ARRAY '(' array_pair_list ')' { $$ = $3; }
        |	'`' backticks_expr '`'
        |	T_PRINT expr
        |	function is_reference '('
                        parameter_list ')' lexical_vars '{' inner_statement_list '}'
;

function:
        T_FUNCTION
;

lexical_vars:
                /* empty */
        |	T_USE '(' lexical_var_list ')'
;

lexical_var_list:
                lexical_var_list ',' T_VARIABLE
        |	lexical_var_list ',' '&' T_VARIABLE
        |	T_VARIABLE
        |	'&' T_VARIABLE
;

function_call:
                namespace_name '('
                                function_call_parameter_list
                                ')'
               {$$=ASTNode::create("function_call")->addChild($1)->addChild($3);}
        |	T_NAMESPACE T_NS_SEPARATOR namespace_name '('
                                function_call_parameter_list
                                ')'
        |	T_NS_SEPARATOR namespace_name '('
                                function_call_parameter_list
                                ')'
        |	class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING '('
                        function_call_parameter_list
                        ')'
        |	class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects '('
                        function_call_parameter_list
                        ')'
        |	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING '('
                        function_call_parameter_list
                        ')'
        |	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects '('
                        function_call_parameter_list
                        ')'
        |	variable_without_objects  '('
                        function_call_parameter_list ')'

;

class_name:
                T_STATIC
        |	namespace_name { $$ = $1; }
        |	T_NAMESPACE T_NS_SEPARATOR namespace_name
        |	T_NS_SEPARATOR namespace_name
;

fully_qualified_class_name:
                namespace_name { $$ = $1; }
        |	T_NAMESPACE T_NS_SEPARATOR namespace_name
        |	T_NS_SEPARATOR namespace_name
;



class_name_reference:
                class_name
        |	dynamic_class_name_reference
;


dynamic_class_name_reference:
                base_variable T_OBJECT_OPERATOR
                        object_property  dynamic_class_name_variable_properties

        |	base_variable { $$ = $1; }
;


dynamic_class_name_variable_properties:
                dynamic_class_name_variable_properties dynamic_class_name_variable_property
        |	/* empty */
;


dynamic_class_name_variable_property:
                T_OBJECT_OPERATOR object_property
;

exit_expr:
                /* empty */
        |	'(' ')'
        |	'(' expr ')'	{ $$ = $2; }
;

backticks_expr:
                /* empty */
        |	T_ENCAPSED_AND_WHITESPACE	{ $$ = $1; }
        |	encaps_list	{ $$ = $1; }
;


ctor_arguments:
                /* empty */
        |	'(' function_call_parameter_list ')'	{ $$ = $2; }
;


common_scalar:
                T_LNUMBER 					{ $$ = $1; }
        |	T_DNUMBER 					{ $$ = $1; }
        |	T_CONSTANT_ENCAPSED_STRING	{ $$ = $1; }
        |	T_LINE 						{ $$ = $1; }
        |	T_FILE 						{ $$ = $1; }
        |	T_DIR   					{ $$ = $1; }
        |	T_CLASS_C					{ $$ = $1; }
        |	T_METHOD_C					{ $$ = $1; }
        |	T_FUNC_C					{ $$ = $1; }
        |	T_NS_C						{ $$ = $1; }
        |	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC
        |	T_START_HEREDOC T_END_HEREDOC
;


static_scalar: /* compile-time evaluated scalars */
                common_scalar		{ $$ = $1; }
        |	namespace_name
        |	T_NAMESPACE T_NS_SEPARATOR namespace_name
        |	T_NS_SEPARATOR namespace_name
        |	'+' static_scalar
        |	'-' static_scalar
        |	T_ARRAY '(' static_array_pair_list ')'
        |	static_class_constant { $$ = $1; }
;

static_class_constant:
                class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
;

scalar:
                T_STRING_VARNAME		{ $$ = $1; }
        |	class_constant		{ $$ = $1; }
        |	namespace_name
        |	T_NAMESPACE T_NS_SEPARATOR namespace_name
        |	T_NS_SEPARATOR namespace_name
        |	common_scalar			{ $$ = $1; }
        |	'"' encaps_list '"' 	{ $$ = $2; }
        |	T_START_HEREDOC encaps_list T_END_HEREDOC
;


static_array_pair_list:
                /* empty */
        |	non_empty_static_array_pair_list possible_comma	{ $$ = $1; }
;

possible_comma:
                /* empty */
        |	','
;

non_empty_static_array_pair_list:
                non_empty_static_array_pair_list ',' static_scalar T_DOUBLE_ARROW static_scalar
        |	non_empty_static_array_pair_list ',' static_scalar
        |	static_scalar T_DOUBLE_ARROW static_scalar
        |	static_scalar
;

expr:
                r_variable
        |	expr_without_variable
;


r_variable:
        variable
;


w_variable:
        variable
;

rw_variable:
        variable
;

variable:
                base_variable_with_function_calls T_OBJECT_OPERATOR
                        object_property  method_or_not variable_properties

        |	base_variable_with_function_calls
;

variable_properties:
                variable_properties variable_property
        |	/* empty */
;


variable_property:
                T_OBJECT_OPERATOR object_property  method_or_not
;

method_or_not:
                '('
                                function_call_parameter_list ')'

        |	/* empty */
;

variable_without_objects:
                reference_variable { $$ = $1; }
        |	simple_indirect_reference reference_variable
;

static_member:
                class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects
        |	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects

;

variable_class_name:
                reference_variable
;

base_variable_with_function_calls:
                base_variable
        |	function_call
;


base_variable:
                reference_variable
        |	simple_indirect_reference reference_variable
        |	static_member
;

reference_variable:
                reference_variable '[' dim_offset ']'
        |	reference_variable '{' expr '}'
        |	compound_variable
;


compound_variable:
                T_VARIABLE			{ $$ = $1; }
        |	'$' '{' expr '}'	{ $$ = $3; }
;

dim_offset:
                /* empty */
        |	expr			{ $$ = $1; }
;


object_property:
                object_dim_list { $$ = $1; }
        |	variable_without_objects
;

object_dim_list:
                object_dim_list '[' dim_offset ']'
        |	object_dim_list '{' expr '}'
        |	variable_name
;

variable_name:
                T_STRING		{ $$ = $1; }
        |	'{' expr '}'	{ $$ = $2; }
;

simple_indirect_reference:
                '$'
        |	simple_indirect_reference '$'
;

assignment_list:
                assignment_list ',' assignment_list_element
        |	assignment_list_element
;


assignment_list_element:
                variable
        |	T_LIST '('  assignment_list ')'
        |	/* empty */
;


array_pair_list:
                /* empty */
        |	non_empty_array_pair_list possible_comma	{ $$ = $1; }
;

non_empty_array_pair_list:
                non_empty_array_pair_list ',' expr T_DOUBLE_ARROW expr
        |	non_empty_array_pair_list ',' expr
        |	expr T_DOUBLE_ARROW expr
        |	expr
        |	non_empty_array_pair_list ',' expr T_DOUBLE_ARROW '&' w_variable
        |	non_empty_array_pair_list ',' '&' w_variable
        |	expr T_DOUBLE_ARROW '&' w_variable
        |	'&' w_variable
;

encaps_list:
                encaps_list encaps_var
        |	encaps_list T_ENCAPSED_AND_WHITESPACE
        |	encaps_var
        |	T_ENCAPSED_AND_WHITESPACE encaps_var
;



encaps_var:
                T_VARIABLE
        |	T_VARIABLE '['  encaps_var_offset ']'
        |	T_VARIABLE T_OBJECT_OPERATOR T_STRING
        |	T_DOLLAR_OPEN_CURLY_BRACES expr '}'
        |	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}'
        |	T_CURLY_OPEN variable '}' { $$ = $2; }
;


encaps_var_offset:
                T_STRING		{ $$ = $1; }
        |	T_NUM_STRING	{ $$ = $1; }
        |	T_VARIABLE
;


internal_functions_in_yacc:
                T_ISSET '(' isset_variables ')' { $$ = $3; }
        |	T_EMPTY '(' variable ')'
        |	T_INCLUDE expr
        |	T_INCLUDE_ONCE expr
        |	T_EVAL '(' expr ')'
        |	T_REQUIRE expr
        |	T_REQUIRE_ONCE expr
;

isset_variables:
                variable
        |	isset_variables ','  variable
;

class_constant:
                class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
        |	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
;

%%

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: s
 * End:
 */
