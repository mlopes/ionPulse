/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "structurestorage.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QtDebug>
#include <QVariant>
#include <QSqlError>

#include <stdexcept>

namespace IonPhp {
namespace Private {

StructureStorage::StructureStorage(QString connName)
    : db(QSqlDatabase::database(connName))
{
    createTables();
}

bool StructureStorage::beginTransaction()
{
    return db.transaction();
}

bool StructureStorage::commitTransaction()
{
    return db.commit();
}

bool StructureStorage::rollbackTransaction()
{
    return db.rollback();
}

QSharedPointer<QSqlQuery> StructureStorage::getClasses()
{
    QSharedPointer<QSqlQuery> query(new QSqlQuery(db));
    query->prepare("select classes.id, classname, filename, line_nr from classes left join files on files.id=classes.file_id");
    if (!query->exec()) {
        DEBUG_MSG(query->lastError());
        throw std::runtime_error("sqlite error: "+query->lastError().databaseText().toStdString());
    }
    return query;
}

QSharedPointer<QSqlQuery> StructureStorage::getFile(QString filename)
{
    QSharedPointer<QSqlQuery> query(new QSqlQuery(db));
    query->prepare("select id, timestamp from files where files.filename=:filename limit 1");
    query->bindValue("filename", filename);
    if (!query->exec()) {
        DEBUG_MSG(query->lastError());
        throw std::runtime_error("sqlite error: "+query->lastError().databaseText().toStdString());
    }
    return query;
}

QSharedPointer<QSqlQuery> StructureStorage::getFileClasses(int fileId)
{
    QSharedPointer<QSqlQuery> query(new QSqlQuery(db));
    query->prepare("select classes.id, classname, filename, line_nr from classes left join files on files.id=classes.file_id where files.id=:file_id order by line_nr");
    query->bindValue("file_id", fileId);
    if (!query->exec()) {
        DEBUG_MSG(query->lastError());
        throw std::runtime_error("sqlite error: "+query->lastError().databaseText().toStdString());
    }
    return query;
}

QSharedPointer<QSqlQuery> StructureStorage::getClassMethods(int classId)
{
    QSharedPointer<QSqlQuery> query(new QSqlQuery(db));
    query->prepare("select methods.id, methodname, filename, line_nr from methods left join files on files.id=file_id where class_id=:class_id order by line_nr asc");
    query->bindValue("class_id", classId);
    if (!query->exec()) {
        DEBUG_MSG(query->lastError());
        throw std::runtime_error("sqlite error: "+query->lastError().databaseText().toStdString());
    }
    return query;
}

void StructureStorage::createTables()
{
    QSqlQuery query(db);
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        DEBUG_MSG(query.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+query.lastError().databaseText().toStdString());
    }
    if (!query.exec(
        "create table files ("
            "id INTEGER primary key autoincrement, "
            "filename varchar(2048),"
            "timestamp INTEGER"
        ")"
    )) {
        DEBUG_MSG(query.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+query.lastError().databaseText().toStdString());
    }
    if (!query.exec(
        "create index filename_search on files ("
            "filename"
        ")"
    )) {
        DEBUG_MSG(query.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+query.lastError().databaseText().toStdString());
    }
    if (!query.exec(
        "create table classes ("
            "id INTEGER primary key autoincrement, "
            "file_id int,"
            "line_nr int,"
            "classname varchar(255),"
            "FOREIGN KEY(file_id) REFERENCES files(id) ON DELETE CASCADE"
        ")"
    )) {
        DEBUG_MSG(query.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+query.lastError().databaseText().toStdString());
    }
    if (!query.exec(
        "create table methods ("
            "id INTEGER primary key autoincrement, "
            "file_id int,"
            "line_nr int,"
            "class_id int,"
            "methodname varchar(255),"
            "FOREIGN KEY(file_id) REFERENCES files(id) ON DELETE CASCADE,"
            "FOREIGN KEY(class_id) REFERENCES classes(id) ON DELETE CASCADE"
        ")"
    )) {
        DEBUG_MSG(query.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+query.lastError().databaseText().toStdString());
    }


}

int StructureStorage::addFile(QString path, int timestamp, ASTRoot &astRoot)
{
    QSqlQuery fileInsertQuery(db);
    fileInsertQuery.prepare("insert into files(filename, timestamp) values (:filename, :timestamp)");
    QSqlQuery classInsertQuery(db);
    classInsertQuery.prepare("insert into classes(file_id, line_nr, classname) values(:file_id, :line_nr, :classname)");
    QSqlQuery methodInsertQuery(db);
    methodInsertQuery.prepare("insert into methods(file_id, class_id, line_nr, methodname) values(:file_id, :class_id, :line_nr, :methodname)");

    fileInsertQuery.bindValue("filename", path);
    fileInsertQuery.bindValue("timestamp", timestamp);
    fileInsertQuery.exec();

    int fileId = fileInsertQuery.lastInsertId().toInt();
    classInsertQuery.bindValue("file_id", fileId);
    methodInsertQuery.bindValue("file_id", fileId);

    addClasses(classInsertQuery, methodInsertQuery, astRoot, astRoot.xpath("//class_declaration"));

    return fileId;
}

void StructureStorage::addClasses(QSqlQuery &classInsertQuery, QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &classes)
{
    foreach (pASTNode classDef, classes) {
        pASTNode classLabel = astRoot.xpath("string", classDef).front();

        classInsertQuery.bindValue("line_nr", classLabel->getLine());
        classInsertQuery.bindValue("classname", classLabel->getText());
        if (!classInsertQuery.exec()) {
            DEBUG_MSG(classInsertQuery.lastError());
            throw std::runtime_error("failed to register new class definition");
        }
        int classId = classInsertQuery.lastInsertId().toInt();

        methodInsertQuery.bindValue("class_id", classId);
        addMethods(methodInsertQuery, astRoot, astRoot.xpath("class_statement_list/METHOD", classDef));
    }
}


void StructureStorage::addMethods(QSqlQuery &methodInsertQuery, const ASTRoot & astRoot, const QList<ASTNode *> &methods)
{
    foreach(pASTNode methodDef, methods) {
        pASTNode methodLabel = astRoot.xpath("string", methodDef).front();
        methodInsertQuery.bindValue("line_nr", methodLabel->getLine());
        methodInsertQuery.bindValue("methodname", methodLabel->getText());
        if (!methodInsertQuery.exec()) {
            DEBUG_MSG(methodInsertQuery.lastError() << methodInsertQuery.lastQuery());
            throw std::runtime_error("failed to register new method definition");
        }
    }
}

void StructureStorage::removeFile(int file_id)
{
    QSqlQuery fileQuery(db), classQuery(db), methodQuery(db);

    fileQuery.prepare("delete from files where id=:file_id");
    classQuery.prepare("delete from classes where file_id=:file_id");
    methodQuery.prepare("delete from methods where file_id=:file_id");

    fileQuery.bindValue("file_id", file_id);
    classQuery.bindValue("file_id", file_id);
    methodQuery.bindValue("file_id", file_id);

    if (!fileQuery.exec()) {
        DEBUG_MSG(fileQuery.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+fileQuery.lastError().databaseText().toStdString());
    }
    if (!classQuery.exec()) {
        DEBUG_MSG(classQuery.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+classQuery.lastError().databaseText().toStdString());
    }
    if (!methodQuery.exec()) {
        DEBUG_MSG(methodQuery.lastError().databaseText());
        throw std::runtime_error("sqlite error: "+methodQuery.lastError().databaseText().toStdString());
    }
}

}
}
