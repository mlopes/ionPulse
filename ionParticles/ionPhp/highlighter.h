/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPHIGHLIGHTER_H
#define PHPHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

namespace IonPhp {
namespace Private {

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit Highlighter(QPlainTextEdit *parent);
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat multiLineCommentFormat;

protected:
    virtual void highlightBlock(const QString &text);


signals:

public slots:

};

}
}

#endif // PHPHIGHLIGHTER_H