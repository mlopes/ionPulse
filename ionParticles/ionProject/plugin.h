/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONEDITORPROJECT_H
#define IONEDITORPROJECT_H

#include <QObject>
#include <ionHeart/plugin.h>
#include <ionParticles/ionEditor/editorapi.h>

#define PROJECT_PLUGIN_NAME "ionProject"

namespace IonProject {


class Plugin : public QObject, public IonHeart::BasicPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonHeart::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    void initialize();
    QString getName() {
        return PROJECT_PLUGIN_NAME;
    }
    virtual QList<QString> getDependencies() {
        QList<QString> ret;
        ret.append(EDITOR_PLUGIN_NAME);
        return ret;
    }
    virtual void addParent(BasicPlugin *parent);
private:
    IonEditor::EditorPlugin *editorPlugin;
signals:

public slots:
protected slots:
    void openFile(QString path);

};

}

#endif // IONEDITORPROJECT_H




