#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>
#include <ionParticles/ionPhp/phpparser.h>

class IonTest : public QObject
{
    Q_OBJECT

public:
    IonTest();

private Q_SLOTS:
    void testPhpParser_1() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?><?php ?><?php "));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list")
        );
    }
    void testPhpParser_2() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?><?php ?><?php ?>"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list")
        );
    }
    void testPhpParser_3() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd1<?php ?>asd2"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd1]; T_INLINE_HTML [text:asd2])")
         );
    }
    void testPhpParser_4() {
        //QSKIP("boo", SkipSingle);
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd<script language=\"php\">echo $d</script> asd"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd]; echo_expr_list(T_VARIABLE [text:$d]); T_INLINE_HTML [text: asd])")
        );
    }
    void testPhpParser_5() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd<script language=php>echo $d</script> asd"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd]; echo_expr_list(T_VARIABLE [text:$d]); T_INLINE_HTML [text: asd])")
        );
    }
    void testPhpParser_6() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd<script language=notphp>echo $d</script> asd"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd<script language=notphp>echo $d</script> asd])")
        );
    }
    void testPhpParser_7() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php $a = moo();"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(assignment(T_VARIABLE [text:$a]; function_call(namespace_name(T_STRING [text:moo]); function_call_parameter_list)))")
        );
    }
};

IonTest::IonTest()
{
}

QTEST_APPLESS_MAIN(IonTest);

#include "tst_iontest.moc"
