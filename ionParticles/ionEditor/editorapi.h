/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef EDITORAPI_H
#define EDITORAPI_H

#include <QTextBlock>
#include <QEvent>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <ionParticles/ionLayout/layoutapi.h>
#include <ionHeart/plugin.h>

namespace IonEditor {

class EditorComponentInfo {
public:
    virtual ~EditorComponentInfo() {}
    virtual QTextBlock firstVisibleBlock() const = 0;
    virtual QRectF blockBoundingGeometry(const QTextBlock &block) const = 0;
    virtual QPointF contentOffset() const = 0;
    virtual QRectF blockBoundingRect(const QTextBlock &block) const = 0;
};

class EditorComponent {
public:
    virtual ~EditorComponent() {}
    virtual void editorEvent(QEvent * ) = 0;
    virtual int getWidth() = 0;
};

class Editor : public IonLayout::PanelWidget {
public:
    virtual ~Editor() {}
    virtual const EditorComponentInfo &getEditorInfo() const = 0;
    virtual void addEventListener(QEvent::Type type, EditorComponent *component) = 0;
    virtual void updateViewportMargins() = 0;
    virtual void setComponents(QList<EditorComponent* > components) = 0;
    virtual QPlainTextEdit* getEditorInstance() = 0;
    virtual void focusOnLine(int line) = 0;
};

struct EditorComponentFactory {
    virtual IonEditor::EditorComponent *operator()(Editor *) = 0;
    virtual QString getIdentifier() = 0;
};
struct HighlighterFactory {
    virtual QSyntaxHighlighter *operator()(Editor *) = 0;
};

class EditorWidgetBuilder
{
public:
    virtual ~EditorWidgetBuilder(){}
    virtual Editor *createEditor(QString path) = 0;

    virtual void registerFileType(QString fileExt, QString fileType) = 0;
    virtual void registerHighlighterFactory(QString const & filetype, HighlighterFactory *highlighter) = 0;
    virtual void registerComponentFactory(QString const & filetype, EditorComponentFactory *component) = 0;
};

class EditorPlugin : public IonHeart::BasicPlugin {
public:
    static QString name() {return "ionEditor";}
    virtual EditorWidgetBuilder *getEditorWidgetBuilder() = 0;
    virtual void openFile(QString path, int line) = 0;
};


}

#endif // EDITORAPI_H
