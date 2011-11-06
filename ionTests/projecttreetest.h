/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef ProjectTreeTest_H
#define ProjectTreeTest_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionProject/projectapi.h>
#include <ionParticles/ionProject/treeitem.h>
#include <ionParticles/ionProject/filetreemodel.h>

using namespace IonProject;
using namespace IonProject::Private;

namespace IonTests {

class MockTreeSource : public TreeModelSource {
public:
    virtual QString getTitle() const {return "test";}
    virtual TreeBranch * setupData() {
        TreeBranch* parent = new TreeBranchImpl("name", "path", NULL);

        TreeBranch* level1 = new TreeBranchImpl("dir1", "path1", parent);
        parent->appendChild(level1);

        level1->appendChild(new TreeItemImpl("fileName1", "path1/fileName1", level1));

        parent->appendChild(new TreeItemImpl("fileName2", "fileName2", parent));

        return parent;
    }
};


class ProjectDirectoryTreeSourceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_if_getTitleIsForDirectoryTree() {
        QString path;
        DirectoryTreeSource ds(path);

        QCOMPARE(ds.getTitle(), QString("Project Browser"));
    }
};


class ProjectTreeItemTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_if_treeItem_getChildren_returnsEmptyList() {
        TreeItemFactoryImpl factory;
        TreeItem* item = factory.createTreeItem("", "", NULL);
        QList<TreeItem*> list = item->getChildren();
        delete item;

        QCOMPARE(list.size(), 0);
    }
    void test_if_treeBranch_getChildren_returnsEmptyListIfThereAreNoChildren() {
        TreeItemFactoryImpl factory;
        TreeBranch* item = factory.createTreeBranch("", "", NULL);
        QList<TreeItem*> list = item->getChildren();
        delete item;

        QCOMPARE(list.size(), 0);
    }
    void test_if_treeBranch_getChildren_returnsEmptyListIfThereAreSomeChildren() {
        TreeItemFactoryImpl factory;
        TreeBranch* item = factory.createTreeBranch("", "", NULL);
        TreeItem* item2 = factory.createTreeItem("", "", item);
        item->appendChild(item2);
        QList<TreeItem*> list = item->getChildren();
        delete item;

        QCOMPARE(list.size(), 1);
        QCOMPARE(list.front(), item2); // note, item2 is already deleted, compare only the address
    }
};


class ProjectTreeModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_dataDimensions() {
        MockTreeSource source;
        FileTreeModel model(&source);

        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.rowCount(model.index(0, 0)), 1);
        QCOMPARE(model.columnCount(), 1);
    }

    void test_structureBetweenParentAndChild() {
        MockTreeSource source;
        FileTreeModel model(&source);

        QCOMPARE(model.parent(model.index(0, 0, model.index(0, 0))), model.index(0, 0));
    }

    void test_dataContainsCorrectFixtures() {
        MockTreeSource source;
        FileTreeModel model(&source);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("dir1"));
        QCOMPARE(model.data(model.index(1, 0), Qt::DisplayRole).toString(), QString("fileName2"));
        QCOMPARE(model.data(model.index(0, 0, model.index(0, 0)), Qt::DisplayRole).toString(), QString("fileName1"));
    }

    void test_dataIsReducedByFilter() {
        MockTreeSource source;
        FileTreeModel model(&source);

        model.filter("2");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 0);
        QCOMPARE(model.columnCount(), 1);

        model.filter("1");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 1);
        QCOMPARE(model.columnCount(), 1);
    }

    void test_filterWorksForPathBasis() {
        MockTreeSource source;
        FileTreeModel model(&source);

        model.filter("1/f");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 1);
        QCOMPARE(model.columnCount(), 1);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("dir1"));
        QCOMPARE(model.data(model.index(0, 0, model.index(0, 0)), Qt::DisplayRole).toString(), QString("fileName1"));
    }

    void test_getPath() {
        MockTreeSource source;
        FileTreeModel model(&source);

        model.filter("1/f");

        QCOMPARE(model.getPath(model.index(0, 0, model.index(0, 0))), QString("path1/fileName1"));
    }

    void test_getTitleReturnsSourceTitle() {
        MockTreeSource source;
        FileTreeModel model(&source);

        QCOMPARE(model.getTitle(), QString("test"));
    }

    void test_getRootReturnsSourceCreatedRoot() {
        MockTreeSource source;
        FileTreeModel model(&source);

        TreeBranch *root = model.getRoot();
        QCOMPARE(root->data(0).toString(), QString("name"));
    }
};

}

#endif // ProjectTreeTest_H
