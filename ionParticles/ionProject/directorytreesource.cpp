/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QtGui>
#include <QDir>
#include <stdexcept>

#include "treeitem.h"
#include "directorytreesource.h"

namespace IonProject {
namespace Private {

TreeItem *DirectoryTreeSource::setupData()
{
    if (!root) {
        root = new TreeItemImpl("Name", "", "", -1, NULL);
    }

    if (initialDir.length()) {
        addDirectory(root, initialDir);
    }

    return root;
}

void DirectoryTreeSource::addDirectory(TreeItem *parent, QString directory)
{
    QList<TreeItem*> parents;
    parents << parent;

    while (parents.count()) {
        TreeItem* currentTreeItemsParent = parents.last();
        parents.pop_back();
        QString currentDirName = currentTreeItemsParent->getPath();
        QDir currentDir(currentDirName);

        foreach (QString subDirName, currentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) {
            QString fullPath = currentDir.absolutePath()+"/"+subDirName+"/";
            TreeItem* newTreeItem = new TreeItemImpl(subDirName, subDirName, fullPath, -1, currentTreeItemsParent);
            currentTreeItemsParent->appendChild(newTreeItem);

            parents << newTreeItem;
        }

        foreach (QString fileName, currentDir.entryList(QDir::Files, QDir::Name)) {
            QString fullPath = currentDir.absolutePath()+"/"+fileName;
            currentTreeItemsParent->appendChild(new TreeItemImpl(fileName, fileName, fullPath, -1, currentTreeItemsParent));
        }
    }
}


}
}
