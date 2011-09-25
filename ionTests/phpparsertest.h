/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPPARSERTEST_H
#define PHPPARSERTEST_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionPhp/phpparser.h>


#define QCOMPARE_3(actual, expected, actions) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        actions; \
        return;\
} while (0)

#define PRINT(QSTR) std::cout << QSTR.toStdString() << std::endl;
#define TEST_PHP_PARSER(CODE, ASTSTR) { \
    ASTRoot ret; \
    QVERIFY((ret = IonPhp::phpParser().parse(CODE)).data()); \
    QCOMPARE_3( \
        ret->toString(), \
        QString(ASTSTR), \
        PRINT(ret->toString()) \
    ); \
}


class PhpParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_openCloseNoEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ",
        "top_statement_list(empty statement; empty statement)"
    ); }
    void test_openCloseEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ?>",
        "top_statement_list(empty statement; empty statement; empty statement)"
    ); }
    void test_inlineHtml() { TEST_PHP_PARSER(
        "<?php ?>asd1<?php ?>asd2",
        "top_statement_list(empty statement; T_INLINE_HTML [text:asd1]; empty statement; T_INLINE_HTML [text:asd2])"
    ); }
    void test_scriptOpenTag() { TEST_PHP_PARSER(
        " <?<%<?php ?>asd?%<script language=\"php\">echo $d</script> asd",
        "top_statement_list(T_INLINE_HTML [text: <?<%]; empty statement; T_INLINE_HTML [text:asd?%]; echo(echo_expr_list(T_VARIABLE [text:$d])); T_INLINE_HTML [text: asd])"
    ); }
    void test_scriptOpenTagWOQuotes() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=php>echo $d</script> asd",
        "top_statement_list(empty statement; T_INLINE_HTML [text:asd]; echo(echo_expr_list(T_VARIABLE [text:$d])); T_INLINE_HTML [text: asd])"
    ); }
    void test_scriptOpenTagWrong() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=notphp>echo $d</script> asd",
        "top_statement_list(empty statement; T_INLINE_HTML [text:asd<script language=notphp>echo $d</script> asd])"
    ); }
    void test_variableAssignmentFromFncCall() { TEST_PHP_PARSER(
        "<?php $a = moo();",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; function_call(namespace_name(T_STRING [text:moo]); function_call_parameter_list)))"
    ); }
    void test_variableAssignmentFromFncCallWithParams() { TEST_PHP_PARSER(
        "<?php $a = moo(1, $s);",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; function_call(namespace_name(T_STRING [text:moo]); function_call_parameter_list(T_LNUMBER [text:1]; T_VARIABLE [text:$s]))))"
    ); }
    void test_functionDefinition() {
        TEST_PHP_PARSER(
            "<?php function myfnc() {}",
            "top_statement_list(function_declaration(is_reference [is_reference:0]; T_STRING [text:myfnc]; parameter_list; inner_statement_list))"
        );
    }
    void test_listDefinition() { TEST_PHP_PARSER(
        "<?php list($a, $b) = $c;",
        "top_statement_list(assignment(assignment_list(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]); T_VARIABLE [text:$c]))"
    ); }
    void test_assignDefinition() { TEST_PHP_PARSER(
        "<?php $a = $c;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_VARIABLE [text:$c]))"
    ); }
    void test_assignRefDefinition() { TEST_PHP_PARSER(
        "<?php $a = &$c;",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE [text:$a]; T_VARIABLE [text:$c]))"
    ); }
    void test_assignRefNewClassDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd;",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE [text:$a]; T_NEW(namespace_name(T_STRING [text:asd]))))"
    ); }
    void test_newClassDefinition() { TEST_PHP_PARSER(
        "<?php new asd(1, '2');",
        "top_statement_list(T_NEW(namespace_name(T_STRING [text:asd]); function_call_parameter_list(T_LNUMBER [text:1]; T_CONSTANT_ENCAPSED_STRING [text:2])))"
    ); }
    void test_cloneDefinition() { TEST_PHP_PARSER(
        "<?php clone $a;",
        "top_statement_list(clone(T_VARIABLE [text:$a]))"
    ); }
    void test_assignPlusDefinition() { TEST_PHP_PARSER(
        "<?php $a+=$b;",
        "top_statement_list(T_PLUS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignMinusDefinition() { TEST_PHP_PARSER(
        "<?php $a-=$b;",
        "top_statement_list(T_MINUS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignMulDefinition() { TEST_PHP_PARSER(
        "<?php $a*=$b;",
        "top_statement_list(T_MUL_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignDivDefinition() { TEST_PHP_PARSER(
        "<?php $a/=$b;",
        "top_statement_list(T_DIV_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignConcatDefinition() { TEST_PHP_PARSER(
        "<?php $a.=$b;",
        "top_statement_list(T_CONCAT_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignModDefinition() { TEST_PHP_PARSER(
        "<?php $a%=$b;",
        "top_statement_list(T_MOD_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignAndDefinition() { TEST_PHP_PARSER(
        "<?php $a&=$b;",
        "top_statement_list(T_AND_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignOrDefinition() { TEST_PHP_PARSER(
        "<?php $a|=$b;",
        "top_statement_list(T_OR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignXorDefinition() { TEST_PHP_PARSER(
        "<?php $a^=$b;",
        "top_statement_list(T_XOR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignShLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a<<=$b;",
        "top_statement_list(T_SL_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignShRightDefinition() { TEST_PHP_PARSER(
        "<?php $a>>=$b;",
        "top_statement_list(T_SR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_postIncDefinition() { TEST_PHP_PARSER(
        "<?php $a++;",
        "top_statement_list(POSTINC(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParserp_preIncDefinition() { TEST_PHP_PARSER(
        "<?php ++$b;",
        "top_statement_list(PREINC(T_VARIABLE [text:$b]))"
    ); }
    void test_postDecDefinition() { TEST_PHP_PARSER(
        "<?php $a--;",
        "top_statement_list(POSTDEC(T_VARIABLE [text:$a]))"
    ); }
    void test_preDecDefinition() { TEST_PHP_PARSER(
        "<?php --$a;",
        "top_statement_list(PREDEC(T_VARIABLE [text:$a]))"
    ); }
    void test_boolOrDefinition() { TEST_PHP_PARSER(
        "<?php $a || $b;",
        "top_statement_list(T_BOOLEAN_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_boolAndDefinition() { TEST_PHP_PARSER(
        "<?php $a && $b;",
        "top_statement_list(T_BOOLEAN_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_logicalOrDefinition() { TEST_PHP_PARSER(
        "<?php $a or $b;",
        "top_statement_list(T_LOGICAL_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_logicalAndDefinition() { TEST_PHP_PARSER(
        "<?php $a and $b;",
        "top_statement_list(T_LOGICAL_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_logicalXorDefinition() { TEST_PHP_PARSER(
        "<?php $a xor $b;",
        "top_statement_list(T_LOGICAL_XOR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_orDefinition() { TEST_PHP_PARSER(
        "<?php $a | $b;",
        "top_statement_list(T_BINARY_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_andDefinition() { TEST_PHP_PARSER(
        "<?php $a & $b;",
        "top_statement_list(T_BINARY_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_xorDefinition() { TEST_PHP_PARSER(
        "<?php $a ^ $b;",
        "top_statement_list(T_BINARY_XOR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_concatDefinition() { TEST_PHP_PARSER(
        "<?php $a . $b;",
        "top_statement_list(T_CONCAT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_plusDefinition() { TEST_PHP_PARSER(
        "<?php $a + $b;",
        "top_statement_list(T_PLUS(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_minusDefinition() { TEST_PHP_PARSER(
        "<?php $a - $b;",
        "top_statement_list(T_MINUS(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_mulDefinition() { TEST_PHP_PARSER(
        "<?php $a * $b;",
        "top_statement_list(T_MUL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_divDefinition() { TEST_PHP_PARSER(
        "<?php $a / $b;",
        "top_statement_list(T_DIV(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_modDefinition() { TEST_PHP_PARSER(
        "<?php $a % $b;",
        "top_statement_list(T_MOD(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_shLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a << $b;",
        "top_statement_list(T_SHIFT_LEFT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_shRightDefinition() { TEST_PHP_PARSER(
        "<?php $a >> $b;",
        "top_statement_list(T_SHIFT_RIGHT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_negateDefinition() { TEST_PHP_PARSER(
        "<?php !$a;",
        "top_statement_list(T_NEGATE(T_VARIABLE [text:$a]))"
    ); }
    void test_inverseDefinition() { TEST_PHP_PARSER(
        "<?php ~$a;",
        "top_statement_list(T_INVERSE(T_VARIABLE [text:$a]))"
    ); }
    void test_isIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a === $b;",
        "top_statement_list(T_IS_IDENTICAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_isNotIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a !== $b;",
        "top_statement_list(T_IS_NOT_IDENTICAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_isEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a == $b;",
        "top_statement_list(T_IS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_isNotEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a != $b;",
        "top_statement_list(T_IS_NOT_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_lessDefinition() { TEST_PHP_PARSER(
        "<?php $a < $b;",
        "top_statement_list(T_LESSTHAN(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_lessEqDefinition() { TEST_PHP_PARSER(
        "<?php $a <= $b;",
        "top_statement_list(T_LESSTHAN_EQ(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_moreDefinition() { TEST_PHP_PARSER(
        "<?php $a > $b;",
        "top_statement_list(T_GREATERTHAN(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_moreEqDefinition() { TEST_PHP_PARSER(
        "<?php $a >= $b;",
        "top_statement_list(T_GREATERTHAN_EQ(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_instanceOfDefinition() { TEST_PHP_PARSER(
        "<?php $a instanceof B;",
        "top_statement_list(T_INSTANCEOF(T_VARIABLE [text:$a]; namespace_name(T_STRING [text:B])))"
    ); }
    void test_parenthesesDefinition() { TEST_PHP_PARSER(
        "<?php ($a);",
        "top_statement_list(T_VARIABLE [text:$a])"
    ); }
    void test_ternaryDefinition() { TEST_PHP_PARSER(
        "<?php $a ? $b : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]; T_VARIABLE [text:$c]))"
    ); }
    void test_ternaryPartialDefinition() { TEST_PHP_PARSER(
        "<?php $a ? : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE [text:$a]; VOID; T_VARIABLE [text:$c]))"
    ); }
    void test_castIntDefinition() { TEST_PHP_PARSER(
        "<?php (int) $a ;",
        "top_statement_list(T_INT_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castDoubleDefinition() { TEST_PHP_PARSER(
        "<?php (double) $a ;",
        "top_statement_list(T_DOUBLE_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castStringDefinition() { TEST_PHP_PARSER(
        "<?php (string) $a ;",
        "top_statement_list(T_STRING_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castArrayDefinition() { TEST_PHP_PARSER(
        "<?php (array) $a ;",
        "top_statement_list(T_ARRAY_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castObjectDefinition() { TEST_PHP_PARSER(
        "<?php (object) $a ;",
        "top_statement_list(T_OBJECT_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castBoolDefinition() { TEST_PHP_PARSER(
        "<?php (bool) $a ;",
        "top_statement_list(T_BOOL_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castUnsetDefinition() { TEST_PHP_PARSER(
        "<?php (unset) $a ;",
        "top_statement_list(T_UNSET_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_exitDefinition() { TEST_PHP_PARSER(
        "<?php exit (1) ;",
        "top_statement_list(T_EXIT(T_LNUMBER [text:1]))"
    ); }
    void test_silenceDefinition() { TEST_PHP_PARSER(
        "<?php @$a ;",
        "top_statement_list(SILENCE(T_VARIABLE [text:$a]))"
    ); }
    void test_arrayDefinition() { TEST_PHP_PARSER(
        "<?php array(1=>$a, &$b, 'c'=>3+2,) ;",
        "top_statement_list(T_ARRAY(array_pair_list(array_pair(array_key(T_LNUMBER [text:1]); array_value(T_VARIABLE [text:$a])); array_pair(array_key; array_value [is_reference:1](T_VARIABLE [text:$b])); array_pair(array_key(T_CONSTANT_ENCAPSED_STRING [text:c]); array_value(T_PLUS(T_LNUMBER [text:3]; T_LNUMBER [text:2]))))))"
    ); }
    void test_arrayDefinitionRef() { TEST_PHP_PARSER(
        "<?php array(&$a, &$b) ;",
        "top_statement_list(T_ARRAY(array_pair_list(array_pair(array_key; array_value [is_reference:1](T_VARIABLE [text:$a])); array_pair(array_key; array_value [is_reference:1](T_VARIABLE [text:$b])))))"
    ); }
    void test_backticksDefinition() { TEST_PHP_PARSER(
        "<?php `$a boo $2 moo` ;",
        "top_statement_list(BACKTICKS(encaps_list(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text: boo $2 moo])))"
    ); }
    void test_backticksStartsWithBackslash() { TEST_PHP_PARSER(
        "<?php `\\$a boo moo` ;",
        "top_statement_list(BACKTICKS(T_ENCAPSED_AND_WHITESPACE [text:\\$a boo moo]))"
    ); }
    void test_backticksContainsBackslash() { TEST_PHP_PARSER(
        "<?php `asd\\$a boo {}moo` ;",
        "top_statement_list(BACKTICKS(T_ENCAPSED_AND_WHITESPACE [text:asd\\$a boo {}moo]))"
    ); }
    void test_backticksWithTwoVariables() { TEST_PHP_PARSER(
        "<?php `asd$a $boo moo` ;",
        "top_statement_list(BACKTICKS(encaps_list(T_ENCAPSED_AND_WHITESPACE [text:asd]; T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text: ]; T_VARIABLE [text:$boo]; T_ENCAPSED_AND_WHITESPACE [text: moo])))"
    ); }
    void test_backticksWithTwoVariablesAndSecondIsBracketed() { TEST_PHP_PARSER(
        "<?php `asd$a {$boo} moo` ;",
        "top_statement_list(BACKTICKS(encaps_list(T_ENCAPSED_AND_WHITESPACE [text:asd]; T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text: ]; T_VARIABLE [text:$boo]; T_ENCAPSED_AND_WHITESPACE [text: moo])))"
    ); }
    void test_printDefinition() { TEST_PHP_PARSER(
        "<?php print $x ;",
        "top_statement_list(T_PRINT(T_VARIABLE [text:$x]))"
    ); }
    void test_lambdaFncDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(is_reference [is_reference:0]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE; inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void test_lambdaFncUseDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) use ($b) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(is_reference [is_reference:0]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE(lexical_var_list(T_VARIABLE [text:$b])); inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void test_lambdaFncRefDefinition() { TEST_PHP_PARSER(
        "<?php function & ($a) use ($b, &$c) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(is_reference [is_reference:1]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE(lexical_var_list(T_VARIABLE [text:$b]; T_VARIABLE [is_reference:1, text:$c])); inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void test_doubleQuotesConst() { TEST_PHP_PARSER(
        "<?php $a = \"\\rtest $ {} string\\n\" ;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_CONSTANT_ENCAPSED_STRING [text:\\rtest $ {} string\\n]))"
    ); }
    void test_doubleQuotesWithVars() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest $moo more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE [text:$a]; "
                "doubleQuotes("
                    "encaps_list("
                        "T_ENCAPSED_AND_WHITESPACE [text:\\ntest ]; "
                        "T_VARIABLE [text:$moo]; "
                        "T_ENCAPSED_AND_WHITESPACE [text: more text\n\\n ]; "
                        "T_VARIABLE [text:$boo]; "
                        "T_STRING_VARNAME [text:buka]; "
                        "T_ENCAPSED_AND_WHITESPACE [text:s]; "
                        "offset(T_STRING_VARNAME [text:aa]; T_LNUMBER [text:2]); "
                        "T_ENCAPSED_AND_WHITESPACE [text: ]; "
                        "T_OBJECT_OPERATOR("
                            "T_VARIABLE [text:$o]; "
                            "T_STRING [text:prop]; "
                            "method_or_not; "
                            "variable_properties"
                        "); "
                        "T_ENCAPSED_AND_WHITESPACE [text:trin $ {} g]"
                    ")"
                ")"
            ")"
        ")"
    ); }
    void test_doubleQuotesWithVarsStartsBracketed() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest {$moo} more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE [text:$a]; "
                "doubleQuotes("
                    "encaps_list("
                        "T_ENCAPSED_AND_WHITESPACE [text:\\ntest ]; "
                        "T_VARIABLE [text:$moo]; "
                        "T_ENCAPSED_AND_WHITESPACE [text: more text\n\\n ]; "
                        "T_VARIABLE [text:$boo]; "
                        "T_STRING_VARNAME [text:buka]; "
                        "T_ENCAPSED_AND_WHITESPACE [text:s]; "
                        "offset(T_STRING_VARNAME [text:aa]; T_LNUMBER [text:2]); "
                        "T_ENCAPSED_AND_WHITESPACE [text: ]; "
                        "T_OBJECT_OPERATOR("
                            "T_VARIABLE [text:$o]; "
                            "T_STRING [text:prop]; "
                            "method_or_not; "
                            "variable_properties"
                        "); "
                        "T_ENCAPSED_AND_WHITESPACE [text:trin $ {} g]"
                    ")"
                ")"
            ")"
        ")"
    ); }
    void test_doubleQuotesWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = \"test $12 more text\" ;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_CONSTANT_ENCAPSED_STRING [text:test $12 more text]))"
    ); }
    void test_singleQuotesEscaping() { TEST_PHP_PARSER(
        "<?php $a = 'test\\' $a more text' ;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_CONSTANT_ENCAPSED_STRING [text:test\\' $a more text]))"
    ); }
    void test_hereDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex{t\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:tex{t]))"
    ); }
    void test_hereDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\r\ntext\r\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text]))"
    ); }
    void test_hereDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text]))"
    ); }
    void test_hereDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\na\rs\r\nd\n\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text\na\rs\r\nd\n]))"
    ); }
    void test_hereDocWithVars() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex$var1 {$var2} {$var3[$i]} {$var4->prop} ${var5[$i]}$var6[$i]$var7->prop t\nMYDOC; ",
        "top_statement_list("
            "assignment("
                "T_VARIABLE [text:$a]; "
                "hereDoc("
                    "encaps_list("
                        "T_ENCAPSED_AND_WHITESPACE [text:tex]; "
                        "T_VARIABLE [text:$var1]; "
                        "T_ENCAPSED_AND_WHITESPACE [text: ]; "
                        "T_VARIABLE [text:$var2]; "
                        "T_ENCAPSED_AND_WHITESPACE [text: ]; "
                        "offset(T_VARIABLE [text:$var3]; T_VARIABLE [text:$i]); "
                        "T_ENCAPSED_AND_WHITESPACE [text: ]; "
                        "T_OBJECT_OPERATOR(T_VARIABLE [text:$var4]; T_STRING [text:prop]; method_or_not; variable_properties); "
                        "T_ENCAPSED_AND_WHITESPACE [text: ]; "
                        "offset(T_STRING_VARNAME [text:var5]; T_VARIABLE [text:$i]); "
                        "offset(T_VARIABLE [text:$var6]; T_VARIABLE [text:$i]); "
                        "T_OBJECT_OPERATOR(T_VARIABLE [text:$var7]; T_STRING [text:prop]); "
                        "T_ENCAPSED_AND_WHITESPACE [text: t]"
                    ")"
                ")"
            ")"
        ")"
    ); }
    void test_hereDocWithQuoteLabels() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntext\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text]))"
    ); }
    void test_hereDocWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntex $123 t\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:tex $123 t]))"
    ); }
    void test_nowDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\ntext$nonvar w\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text$nonvar w]))"
    ); }
    void test_nowDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\r\ntext\r\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text]))"
    ); }
    void test_nowDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text]))"
    ); }
    void test_nowDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\na\rs\r\nd\n\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text\na\rs\r\nd\n]))"
    ); }
    void test_singleLineComment() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'\n$s=1; // asd \n // //..  ",
        "top_statement_list(assignment(T_VARIABLE [text:$s]; T_LNUMBER [text:1]))"
    ); }
    void test_singleLineCommentWindowsEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r\n$s=1; // asd \r\n // //..  ",
        "top_statement_list(assignment(T_VARIABLE [text:$s]; T_LNUMBER [text:1]))"
    ); }
    void test_singleLineCommentMacEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r$s=1; // asd \r // //..  ",
        "top_statement_list(assignment(T_VARIABLE [text:$s]; T_LNUMBER [text:1]))"
    ); }
    void test_singleLineCommentScriptStop() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'?>$s=1; // asd \n // //..  ",
        "top_statement_list(empty statement; T_INLINE_HTML [text:$s=1; // asd \n // //..  ])"
    ); }
    void test_singleLineHashComment() { TEST_PHP_PARSER(
        "<?php #$a = <<<'MYDOC'\n$s=1; # asd \n # /..  ",
        "top_statement_list(assignment(T_VARIABLE [text:$s]; T_LNUMBER [text:1]))"
    ); }
    void test_multiLineComment() { TEST_PHP_PARSER(
        "<?php /* $a = *<<<'MYDOC'\n$s=1; # asd \n # /.. */ $b=2; ",
        "top_statement_list(assignment(T_VARIABLE [text:$b]; T_LNUMBER [text:2]))"
    ); }
    void test_docComment() { TEST_PHP_PARSER(
        "<?php /** lala */ ",
        "top_statement_list"
    ); }
    void test_numbers() { TEST_PHP_PARSER(
        "<?php $a=12+12.2+.2+0x02+0123+2e1;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE [text:$a]; "
                "T_PLUS("
                    "T_PLUS("
                        "T_PLUS("
                            "T_PLUS("
                                "T_PLUS("
                                    "T_LNUMBER [text:12]; "
                                    "T_DNUMBER [text:12.2]"
                                "); "
                                "T_DNUMBER [text:.2]"
                            "); "
                            "T_LNUMBER [text:0x02]"
                        "); "
                        "T_LNUMBER [text:0123]"
                    "); "
                    "T_DNUMBER [text:2e1]"
                ")"
            ")"
        ")"
    ); }
    void test_phpConstants() { TEST_PHP_PARSER(
        "<?php $a=__CLASS__.__FUNCTION__.__METHOD__.__LINE__.__FILE__.__DIR__.__NAMESPACE__;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE [text:$a]; "
                "T_CONCAT("
                    "T_CONCAT("
                        "T_CONCAT("
                            "T_CONCAT("
                                "T_CONCAT("
                                    "T_CONCAT("
                                        "__CLASS__; "
                                        "__FUNCTION__"
                                    "); "
                                    "__METHOD__"
                                "); "
                                "__LINE__"
                            "); "
                            "__FILE__"
                        "); "
                        "__DIR__"
                    "); "
                    "__NAMESPACE__"
                ")"
            ")"
        ")"
    ); }
    void test_classDefinition() { TEST_PHP_PARSER(
        "<?php class A { }",
        "top_statement_list(class_declaration(class; T_STRING [text:A]; extends; implements; class_statement_list))"
    );}
    void test_classExtendingOtherDefinition() { TEST_PHP_PARSER(
        "<?php class A extends B { }",
        "top_statement_list(class_declaration(class; T_STRING [text:A]; extends(namespace_name(T_STRING [text:B])); implements; class_statement_list))"
    );}
    void test_classImplementingInterfaceDefinition() { TEST_PHP_PARSER(
        "<?php class A implements I { }",
        "top_statement_list(class_declaration(class; T_STRING [text:A]; extends; implements(interfaceList(namespace_name(T_STRING [text:I]))); class_statement_list))"
    );}
    void test_abstractClassDefinition() { TEST_PHP_PARSER(
        "<?php abstract class A { }",
        "top_statement_list(class_declaration(class [type:abstract]; T_STRING [text:A]; extends; implements; class_statement_list))"
    );}
    void test_finalClassDefinition() { TEST_PHP_PARSER(
        "<?php final class A { }",
        "top_statement_list(class_declaration(class [type:final]; T_STRING [text:A]; extends; implements; class_statement_list))"
    );}
    void test_interfaceDefinition() { TEST_PHP_PARSER(
        "<?php interface A { }",
        "top_statement_list(interface_declaration(interface; T_STRING [text:A]; extends; class_statement_list))"
    );}
    void test_interfaceInheritanceDefinition() { TEST_PHP_PARSER(
        "<?php interface A extends B, C { }",
        "top_statement_list(interface_declaration(interface; T_STRING [text:A]; extends(interfaceList(namespace_name(T_STRING [text:B]); namespace_name(T_STRING [text:C]))); class_statement_list))"
    );}

    void test_statementIf() { TEST_PHP_PARSER(
        "<?php if(1) {echo 'x';} elseif (2) { echo 'y'; } else echo 'z';",
        "top_statement_list("
            "if("
                "T_LNUMBER [text:1]; "
                "inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING [text:x]))); "
                "elseif_list("
                    "elseif("
                        "T_LNUMBER [text:2]; "
                        "inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING [text:y])))"
                    ")"
                "); "
                "else("
                    "echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING [text:z]))"
                ")"
            ")"
        ")"
    );}
    void test_statementWhile() { TEST_PHP_PARSER(
        "<?php while(1) {echo 'x';}",
        "top_statement_list(while(T_LNUMBER [text:1]; inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING [text:x])))))"
    );}
    void test_statementDoWhile() { TEST_PHP_PARSER(
        "<?php do {echo 'x';} while(1);",
        "top_statement_list(dowhile(inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING [text:x]))); T_LNUMBER [text:1]))"
    );}
//    void test_statementFor() { TEST_PHP_PARSER(
//        "<?php for($i=0;$i<1;$i++) {echo 'x';}",
//        ""
//    );}
//    void test_statementSwitch() { TEST_PHP_PARSER(
//        "<?php switch($i) {case 'x': echo 'y'; default: echo 'z';}",
//        ""
//    );}
//    void test_statementBreak() { TEST_PHP_PARSER(
//        "<?php break;",
//        ""
//    );}
//    void test_statementContinue() { TEST_PHP_PARSER(
//        "<?php continue;",
//        ""
//    );}
//    void test_statementReturn() { TEST_PHP_PARSER(
//        "<?php funtion x(){return 2;}",
//        ""
//    );}
//    void test_statementStaticVars() { TEST_PHP_PARSER(
//        "<?php funtion x(){static $a = 2; return $a;}",
//        ""
//    );}
//    void test_statementForeach() { TEST_PHP_PARSER(
//        "<?php foreach($ar as $a=>$v) {echo 'x';}",
//        ""
//    );}
//    void test_statementForeachRef() { TEST_PHP_PARSER(
//        "<?php foreach($ar as $a=>&$v) {echo 'x';}",
//        ""
//    );}
//    void test_statementThrow() { TEST_PHP_PARSER(
//        "<?php throw new Exception();",
//        ""
//    );}
//    void test_statementThyCatchCatch() { TEST_PHP_PARSER(
//        "<?php try {echo 'x';} catch (MyException $e) {} catch (Exception $e) {}",
//        ""
//    );}
};

#endif // PHPPARSERTEST_H
