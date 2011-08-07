%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2011 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id: zend_language_parser.y 306939 2011-01-01 02:19:59Z felipe $ */

/*
 * LALR shift/reduce conflicts and how they are resolved:
 *
 * - 2 shift/reduce conflicts due to the dangling elseif/else ambiguity. Solved by shift.
 *
 */


#include "phpParser/ionParserLib.h"


#define YYERROR_VERBOSE
#define YYSTYPE ASTNode


%}

%pure_parser
%expect 2

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

start:
        top_statement_list
;

top_statement_list:
                top_statement_list top_statement
        |	/* empty */
;

namespace_name:
                T_STRING { $$ = new_ASTNode("namespace_name")->addChild($1); }
        |	namespace_name T_NS_SEPARATOR T_STRING { $$ = new_ASTNode("namespace_name")->addChild($3); }
;

top_statement:
                statement
        |	function_declaration_statement
        |	class_declaration_statement
        |	T_HALT_COMPILER '(' ')' ';'      { $$ = $1; YYACCEPT; }
        |	T_NAMESPACE namespace_name ';'	{ $$ = new_ASTNode("namespace")->addChild($2); }
        |	T_NAMESPACE namespace_name '{'
                top_statement_list '}'		{ $$ = new_ASTNode("namespace")->addChild($2)->addChild($4); }
        |	T_NAMESPACE '{'
                top_statement_list '}'		{ $$ = new_ASTNode("namespace")->addChild($3); }
        |	T_USE use_declarations ';'       { $$ = new_ASTNode("use")->addChild($2); }
        |	constant_declaration ';'
;

use_declarations:
                use_declarations ',' use_declaration
        |	use_declaration
;

use_declaration:
                namespace_name
        |	namespace_name T_AS T_STRING	{ $$ = new_ASTNode("as")->addChild($1)->addChild($3); }
        |	T_NS_SEPARATOR namespace_name    { $$ = new_ASTNode("namespaceroot")->addChild($2); }
        |	T_NS_SEPARATOR namespace_name T_AS T_STRING { $$ = new_ASTNode("as")->addChild(new_ASTNode("namespaceroot")->addChild($2))->addChild($4); }
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
        |	T_HALT_COMPILER '(' ')' ';'   { zend_error(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }
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
        |	T_ECHO echo_expr_list ';'
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
        T_CATCH '(' fully_qualified_class_name  T_VARIABLE ')'  '{' inner_statement_list '}' { zend_do_end_catch(&$1 TSRMLS_CC); }
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
                T_STRING '=' static_scalar					{ zend_do_declare_stmt(&$1, &$3 TSRMLS_CC); }
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
        |	optional_class_type '&' T_VARIABLE '=' static_scalar			{ znode tmp;  fetch_simple_variable(&tmp, &$3, 0 TSRMLS_CC); $$.op_type = IS_CONST; Z_LVAL($$.u.constant)=1; Z_TYPE($$.u.constant)=IS_LONG; INIT_PZVAL(&$$.u.constant); zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$5, &$1, &$3, 1 TSRMLS_CC); }
        |	optional_class_type T_VARIABLE '=' static_scalar				{ znode tmp;  fetch_simple_variable(&tmp, &$2, 0 TSRMLS_CC); $$.op_type = IS_CONST; Z_LVAL($$.u.constant)=1; Z_TYPE($$.u.constant)=IS_LONG; INIT_PZVAL(&$$.u.constant); zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$4, &$1, &$2, 0 TSRMLS_CC); }
        |	non_empty_parameter_list ',' optional_class_type T_VARIABLE
        |	non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE
        |	non_empty_parameter_list ',' optional_class_type '&' T_VARIABLE	 '=' static_scalar { znode tmp;  fetch_simple_variable(&tmp, &$5, 0 TSRMLS_CC); $$=$1; Z_LVAL($$.u.constant)++; zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$7, &$3, &$5, 1 TSRMLS_CC); }
        |	non_empty_parameter_list ',' optional_class_type T_VARIABLE '=' static_scalar 	{ znode tmp;  fetch_simple_variable(&tmp, &$4, 0 TSRMLS_CC); $$=$1; Z_LVAL($$.u.constant)++; zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$6, &$3, &$4, 0 TSRMLS_CC); }
;


optional_class_type:
                /* empty */
        |	fully_qualified_class_name	{ $$ = $1; }
        |	T_ARRAY						{ $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_NULL;}
;


function_call_parameter_list:
                non_empty_function_call_parameter_list	{ $$ = $1; }
        |	/* empty */
;


non_empty_function_call_parameter_list:
                expr_without_variable
        |	variable
        |	'&' w_variable
        |	non_empty_function_call_parameter_list ',' expr_without_variable	{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
        |	non_empty_function_call_parameter_list ',' variable					{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAR, Z_LVAL($$.u.constant) TSRMLS_CC); }
        |	non_empty_function_call_parameter_list ',' '&' w_variable			{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$4, ZEND_SEND_REF, Z_LVAL($$.u.constant) TSRMLS_CC); }
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
        |	T_VAR							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
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
                class_variable_declaration ',' T_VARIABLE					{ zend_do_declare_property(&$3, NULL, CG(access_type) TSRMLS_CC); }
        |	class_variable_declaration ',' T_VARIABLE '=' static_scalar
        |	T_VARIABLE
        |	T_VARIABLE '=' static_scalar
;

class_constant_declaration:
                class_constant_declaration ',' T_STRING '=' static_scalar
        |	T_CONST T_STRING '=' static_scalar
;

echo_expr_list:
                echo_expr_list ',' expr
        |	expr
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
        |	variable '=' expr
        |	variable '=' '&' variable
        |	variable '=' '&' T_NEW class_name_reference { zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");  zend_check_writable_variable(&$1); zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&$4, &$5 TSRMLS_CC); } ctor_arguments { zend_do_end_new_object(&$3, &$4, &$7 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); $3.u.EA.type = ZEND_PARSED_NEW; zend_do_assign_ref(&$$, &$1, &$3 TSRMLS_CC); }
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
        |	T_VARIABLE								{ zend_do_fetch_lexical_variable(&$1, 0 TSRMLS_CC); }
        |	'&' T_VARIABLE							{ zend_do_fetch_lexical_variable(&$2, 1 TSRMLS_CC); }
;

function_call:
                namespace_name '('
                                function_call_parameter_list
                                ')'
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
        |	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC { $$ = $2; CG(heredoc) = Z_STRVAL($1.u.constant); CG(heredoc_len) = Z_STRLEN($1.u.constant); }
        |	T_START_HEREDOC T_END_HEREDOC
;


static_scalar: /* compile-time evaluated scalars */
                common_scalar		{ $$ = $1; }
        |	namespace_name
        |	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); $3 = $$; zend_do_fetch_constant(&$$, NULL, &$3, ZEND_CT, 0 TSRMLS_CC); }
        |	T_NS_SEPARATOR namespace_name
        |	'+' static_scalar
        |	'-' static_scalar
        |	T_ARRAY '(' static_array_pair_list ')'
        |	static_class_constant { $$ = $1; }
;

static_class_constant:
                class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_CT, 0 TSRMLS_CC); }
;

scalar:
                T_STRING_VARNAME		{ $$ = $1; }
        |	class_constant		{ $$ = $1; }
        |	namespace_name
        |	T_NAMESPACE T_NS_SEPARATOR namespace_name
        |	T_NS_SEPARATOR namespace_name
        |	common_scalar			{ $$ = $1; }
        |	'"' encaps_list '"' 	{ $$ = $2; }
        |	T_START_HEREDOC encaps_list T_END_HEREDOC { $$ = $2; CG(heredoc) = Z_STRVAL($1.u.constant); CG(heredoc_len) = Z_STRLEN($1.u.constant); }
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
                non_empty_static_array_pair_list ',' static_scalar T_DOUBLE_ARROW static_scalar	{ zend_do_add_static_array_element(&$$, &$3, &$5); }
        |	non_empty_static_array_pair_list ',' static_scalar
        |	static_scalar T_DOUBLE_ARROW static_scalar
        |	static_scalar
;

expr:
                r_variable					{ $$ = $1; }
        |	expr_without_variable		{ $$ = $1; }
;


r_variable:
        variable
;


w_variable:
        variable	{ zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); $$ = $1;
                                  zend_check_writable_variable(&$1); }
;

rw_variable:
        variable	{ zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); $$ = $1;
                                  zend_check_writable_variable(&$1); }
;

variable:
                base_variable_with_function_calls T_OBJECT_OPERATOR
                        object_property  method_or_not variable_properties

        |	base_variable_with_function_calls { $$ = $1; }
;

variable_properties:
                variable_properties variable_property { $$.u.EA.type = $2.u.EA.type; }
        |	/* empty */ { $$.u.EA.type = 0; }
;


variable_property:
                T_OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); } method_or_not { $$.u.EA.type = $4.u.EA.type; }
;

method_or_not:
                '('
                                function_call_parameter_list ')'
                        { zend_do_end_function_call(&$1, &$$, &$3, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);
                          zend_do_push_object(&$$ TSRMLS_CC); $$.u.EA.type = ZEND_PARSED_METHOD_CALL; }
        |	/* empty */
;

variable_without_objects:
                reference_variable { $$ = $1; }
        |	simple_indirect_reference reference_variable
;

static_member:
                class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects
        |	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects { $$ = $3; zend_do_fetch_static_member(&$$, &$1 TSRMLS_CC); }

;

variable_class_name:
                reference_variable { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); $$=$1;; }
;

base_variable_with_function_calls:
                base_variable		{ $$ = $1; }
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
                /* empty */		{ $$.op_type = IS_UNUSED; }
        |	expr			{ $$ = $1; }
;


object_property:
                object_dim_list { $$ = $1; }
        |	variable_without_objects  { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
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
                variable								{ zend_do_add_list_element(&$1 TSRMLS_CC); }
        |	T_LIST '('  assignment_list ')'	{ zend_do_new_list_end(TSRMLS_C); }
        |	/* empty */							{ zend_do_add_list_element(NULL TSRMLS_CC); }
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
        |	T_VARIABLE '['  encaps_var_offset ']'	{ fetch_array_begin(&$$, &$1, &$4 TSRMLS_CC); }
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
 * indent-tabs-mode: t
 * End:
 */