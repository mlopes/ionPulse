/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeView>
#include <QFileSystemModel>
#include <ionParticles/ionLayout/layoutapi.h>
#include <QKeyEvent>

namespace IonEditor {

namespace Private {

class FileTreeWidget : public QTreeView
{
    Q_OBJECT
protected:
    QFileSystemModel *_fiModel;

    virtual void keyPressEvent ( QKeyEvent * event );
public:
    explicit FileTreeWidget(QWidget *parent = 0);

signals:
    void fileActivated(QString filename, int line);
public slots:
protected slots:
    void onItemActivated(const QModelIndex &index );

};
}
}

#endif // FILETREEWIDGET_H
