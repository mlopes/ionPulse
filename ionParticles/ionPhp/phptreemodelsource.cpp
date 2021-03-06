/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "phptreemodelsource.h"
#include <iostream>
#include <QFile>
#include <QSqlQuery>
#include <ionCore/shared.h>

#include "phpparser.h"

#include <QFileInfo>
#include <QDateTime>

namespace IonPhp {
namespace Private {

IonProject::TreeItem *PhpTreeModelSourceDecorator::setupData()
{
    IonProject::TreeItem* root = origTreeModelSource->setupData();

    QVector<IonProject::TreeItem*> parents;
    parents.push_back(root);
    while (!parents.empty()) {
        IonProject::TreeItem *node = parents.back();
        parents.pop_back();
        if (node->getItemClass() == TREESOURCE_CLASS_DIR) {
            foreach (IonProject::TreeItem *child, node->getChildren()) {
                parents.push_back(child);
            }
        } else if (node->getItemClass() == TREESOURCE_CLASS_FILE) {
            decorateNode(node);
        }
    }
    return root;
}

void PhpTreeModelSourceDecorator::decorateNode(IonProject::TreeItem *node)
{
    QString path = node->getPath();
    if (path.toLower().endsWith(".php")) {
        addPhpFileInfo(node, path);
    }
}

void PhpTreeModelSourceDecorator::addPhpFileInfo(IonProject::TreeItem *node, QString path)
{
    int fileId = getStoredFile(path);
    if (-1 != fileId) {
        return;
    }
    try {
        if (node->childrenCount()) {
            node->clearChildren();
        }

        storage.beginTransaction();
        fileId = storeFile(path);
        QSharedPointer<QSqlQuery> q = storage.getFileClasses(fileId);
        while (q->next()) {
            QString className = q->value(1).toString();
            IonProject::TreeItem* classNode = treeItemFactory->createTreeItem("phpclass", className, className, q->value(2).toString(), q->value(3).toInt(), node);
            node->appendChild(classNode);

            QSharedPointer<QSqlQuery> qm = storage.getClassMethods(q->value(0).toInt());
            while (qm->next()) {
                QString methodName = qm->value(1).toString();
                classNode->appendChild(treeItemFactory->createTreeItem("phpmethod", methodName, className+"::"+methodName, qm->value(2).toString(), qm->value(3).toInt(), classNode));
            }
        }
        storage.commitTransaction();
    } catch (std::exception &err) {
        DEBUG_MSG(err.what());
        storage.rollbackTransaction();
        //TODO: mark node red
    }
}

int PhpTreeModelSourceDecorator::getStoredFile(QString path)
{
    QSharedPointer<QSqlQuery> fileQuery = storage.getFile(path);
    if (fileQuery->first()) {
        unsigned int mtimeStored = fileQuery->value(1).toInt();
        QFileInfo fileInfo(path);
        if (fileInfo.lastModified().toTime_t() > mtimeStored) {
            storage.removeFile(fileQuery->value(0).toInt());
            return -1;
        }
        return fileQuery->value(0).toInt();
    }
    return -1;
}

int PhpTreeModelSourceDecorator::storeFile(QString path)
{
    QFileInfo fileInfo(path);
    QSharedPointer<ASTRoot> fileAst = phpParser().parseFile(path);
    return storage.addFile(path, fileInfo.lastModified().toTime_t(), *fileAst);
}





}
}
