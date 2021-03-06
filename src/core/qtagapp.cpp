/*
 *    Qtag is an advanced but simple to use audio metadata editor. Runs on Linux
 *    and Windows. Supports many formats, including MPEG, Ogg Vorbis, Wave and FLAC
 *
 *    Copyright (C) 2015 - 2016  Karel Patlejch <kaja.patlejch@gmail.com>
 *
 *    Used libraries:
 *      TagLib - http://taglib.github.io/
 *      Qt - http://qt.io
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "core/qtagapp.h"
#include "core/settings.h"

QtagApp::QtagApp(QApplication *app) : QObject(app) {

    app->setWindowIcon(QIcon(":/images/Qtag.png"));
    QCoreApplication::setApplicationName("Qtag");
    QCoreApplication::setOrganizationName("Qtag");
    QApplication::setStyle(getStyleFromSettings());

    QStringList files = app->arguments();
    files.removeFirst();
    fileList = new FileList(this, &files);

    QObject::connect(fileList, SIGNAL(fileListChanged()), this, SIGNAL(fileListChanged()));

    if(Settings::getOpenLastSession()) {
        openLastSession();
    }

    QObject::connect(this, SIGNAL(fileListChanged()), this, SLOT(saveSession()));

    window = new MainWindow(this);

}

void QtagApp::start() {
    window->show();
}

QStyle* QtagApp::getStyleFromSettings() {

    return QStyleFactory::create(Settings::getApplicationTheme());

}

FileList* QtagApp::getFileList() {
    return fileList;
}

void QtagApp::openFile(QString path, bool update) {
    fileList->addFileToList(path);
    if(update)
        emit fileListChanged();
}

void QtagApp::closeFile(int i, bool update) {
    fileList->closeFile(i);
    if(update)
        emit fileListChanged();
}

void QtagApp::closeFile(QString path, bool update) {
    fileList->closeFile(path);
    if(update)
        emit fileListChanged();
}

void QtagApp::openFilesFromArguments(QStringList files) {
    for(int i = 0; i < files.length(); i++) {
        QFileInfo f(files.at(i));
        if(f.exists()) {
            openFile(files.at(i), false);
        }
    }
}

void QtagApp::saveSession() {

    QList<QVariant> list;
    for(int i = 0; i < fileList->length(); i++) {
        QVariant member(fileList->at(i)->getPath());
        list.append(member);
    }

    QSettings settings;
    settings.setValue("lastfiles", list);
    settings.sync();

}

void QtagApp::openLastSession() {
    QSettings settings;
    QList<QVariant> list = settings.value("lastfiles").toList();
    for(int i = 0; i < list.length(); i++) {
        openFile(list.at(i).toString(), false);
    }
    emit fileListChanged();
}
