/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * (C) 2015 by Karel Patlejch
 *
 * Used libraries:
 *    TagLib - https://github.com/taglib
 *    Qt - http://qt.io
 *
*/

#include "multipletaggingdialog.h"
#include "main.h"
#include "picturefile.h"
#include <attachedpictureframe.h>
#include <QPushButton>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>

MultipleTaggingDialog::MultipleTaggingDialog(QWidget *parent, FileList *list) :
    QDialog(parent) {

    setWindowTitle("Multiple tagging");

    fileList = list;

    filesLabel = new QLabel("Files:", this);
    filesEdit = new QLineEdit(this);
    filesEdit->setReadOnly(true);
    selectFilesButton  = new QPushButton("Select files", this);
    filesGroup = new QGroupBox("Files", this);

    apeCheck = new QCheckBox("APE tags", this);
    apeCheck->setChecked(true);
    asfCheck = new QCheckBox("ASF tags", this);
    asfCheck->setChecked(true);
    id3v1Check = new QCheckBox("ID3v1 tags", this);
    id3v1Check->setChecked(true);
    id3v2Check = new QCheckBox("ID3v2 tags", this);
    id3v2Check->setChecked(true);
    infoTagCheck = new QCheckBox("RIFF Info tags", this);
    infoTagCheck->setChecked(true);
    xiphCommentCheck = new QCheckBox("Xiph Comment", this);
    xiphCommentCheck->setChecked(true);
    tagsGroup = new QGroupBox("Include:", this);

    titleCheck = new QCheckBox("Title:", this);
    titleCheck->setChecked(true);
    trackCheck = new QCheckBox("Track:", this);
    trackCheck->setChecked(true);
    albumCheck = new QCheckBox("Album:", this);
    albumCheck->setChecked(true);
    artistCheck = new QCheckBox("Artist:", this);
    artistCheck->setChecked(true);
    genreCheck = new QCheckBox("Genre:", this);
    genreCheck->setChecked(true);
    yearCheck = new QCheckBox("Year:", this);
    yearCheck->setChecked(true);
    commentCheck = new QCheckBox("Comment:", this);
    commentCheck->setChecked(true);
    coverCheck = new QCheckBox("Cover art:", this);
    coverCheck->setChecked(true);
    titleEdit = new QLineEdit(this);
    trackEdit = new QLineEdit(this);
    albumEdit = new QLineEdit(this);
    artistEdit = new QLineEdit(this);
    genreEdit = new Id3GenreSelection(this);
    yearEdit = new QLineEdit(this);
    yearEdit->setValidator(new QIntValidator(0, 10000, yearEdit));
    commentEdit = new QLineEdit(this);
    coverEdit = new QLineEdit(this);
    coverEdit->setEnabled(false);
    coverEditDefaultText = "<remove cover art>";
    coverEdit->setText(coverEditDefaultText);
    selectCoverButton = new QPushButton("Browse...", this);
    editorGroup = new QGroupBox("Tags", this);

     okButton = new QPushButton("OK", this);
     cancelButton = new QPushButton("Cancel", this);

     QObject::connect(titleCheck, SIGNAL(clicked(bool)), titleEdit, SLOT(setEnabled(bool)));
     QObject::connect(trackCheck, SIGNAL(clicked(bool)), trackEdit, SLOT(setEnabled(bool)));
     QObject::connect(albumCheck, SIGNAL(clicked(bool)), albumEdit, SLOT(setEnabled(bool)));
     QObject::connect(artistCheck, SIGNAL(clicked(bool)), artistEdit, SLOT(setEnabled(bool)));
     QObject::connect(genreCheck, SIGNAL(clicked(bool)), genreEdit, SLOT(setEnabled(bool)));
     QObject::connect(yearCheck, SIGNAL(clicked(bool)), yearEdit, SLOT(setEnabled(bool)));
     QObject::connect(commentCheck, SIGNAL(clicked(bool)), commentEdit, SLOT(setEnabled(bool)));
     QObject::connect(coverCheck, SIGNAL(clicked(bool)), selectCoverButton, SLOT(setEnabled(bool)));

     QObject::connect(selectCoverButton, SIGNAL(clicked()), this, SLOT(selectCover()));
     QObject::connect(selectFilesButton, SIGNAL(clicked()), this, SLOT(openFiles()));
     QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(startTagging()));
     QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

     createLayout();

}

void MultipleTaggingDialog::createLayout() {

    QGridLayout* layout = new QGridLayout(this);
    int i = 0;

    QGridLayout* layout1 = new QGridLayout();
    layout1->addWidget(filesLabel, 0, 0);
    layout1->addWidget(filesEdit, 0, 1);
    layout1->addWidget(selectFilesButton, 0, 2);
    filesGroup->setLayout(layout1);

    QGridLayout* layout2 = new QGridLayout();
    layout2->addWidget(apeCheck, 0, 0);
    layout2->addWidget(asfCheck, 0, 1);
    layout2->addWidget(id3v1Check, 1, 0);
    layout2->addWidget(id3v2Check, 1, 1);
    layout2->addWidget(xiphCommentCheck, 2, 0);
    layout2->addWidget(infoTagCheck, 2, 1);
    tagsGroup->setLayout(layout2);

    QGridLayout* layout3 = new QGridLayout();
    i = 0;
    layout3->addWidget(titleCheck, i, 0);
    layout3->addWidget(titleEdit, i, 1);
    i++;
    layout3->addWidget(trackCheck, i, 0);
    layout3->addWidget(trackEdit, i, 1);
    i++;
    layout3->addWidget(albumCheck, i, 0);
    layout3->addWidget(albumEdit, i, 1);
    i++;
    layout3->addWidget(artistCheck, i, 0);
    layout3->addWidget(artistEdit, i, 1);
    i++;
    layout3->addWidget(genreCheck, i, 0);
    layout3->addWidget(genreEdit, i, 1);
    i++;
    layout3->addWidget(yearCheck, i, 0);
    layout3->addWidget(yearEdit, i, 1);
    i++;
    layout3->addWidget(commentCheck, i, 0);
    layout3->addWidget(commentEdit, i, 1);
    i++;
    layout3->addWidget(coverCheck, i, 0);
    layout3->addWidget(coverEdit, i, 1);
    i++;
    layout3->addWidget(selectCoverButton, i, 0);
    editorGroup->setLayout(layout3);

    QHBoxLayout* layout4 = new QHBoxLayout();
    layout4->addWidget(cancelButton);
    layout4->addWidget(okButton);

    i = layout->rowCount();
    layout->addWidget(filesGroup, i, 0);
    i++;
    layout->addWidget(tagsGroup, i, 0);
    i++;
    layout->addWidget(editorGroup, i, 0);
    i++;
    layout->addLayout(layout4, i, 0);

}

void MultipleTaggingDialog::openFiles() {

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setOption(QFileDialog::DontUseNativeDialog);
#ifdef WIN32
    dialog.setDirectory("C:/");
#else
    dialog.setDirectory("/home");
#endif
    dialog.setNameFilter(
                tr(NAME_FILTER.c_str()));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.show();
    if(dialog.exec()) {

        listOfFiles = dialog.selectedFiles();
        filesEdit->clear();
        filesEdit->insert(listOfFiles.join("; "));

    }

}

void MultipleTaggingDialog::saveTagsTo(QString nameOfTag, QString path) {

    AudioFile* file = NULL;
    for(int i = 0; i < fileList->length(); i++) {
        AudioFile* f = fileList->at(i);
        if(f->getPath() == path) {
            file = f;
            i = fileList->length();
        }
    }
    if(file == NULL) {
        for(int i = 0; i < fileList->closed_length(); i++) {
            AudioFile* f = fileList->closed_at(i);
            if(f->getPath() == path) {
                file = f;
                i = fileList->closed_length();
            }
        }
    }
    if(file == NULL) {
        file = new AudioFile(path, this);
        fileList->addFileToList(path);
    }


    TagLib::Tag* tag = file->getTagByName(nameOfTag);
    if(tag == NULL)
        return;

    if(titleCheck->isChecked()) {
        TagLib::String str(titleEdit->text().toStdString());
        tag->setTitle(str);
    }
    if(trackCheck->isChecked()) {
        tag->setTrack(trackEdit->text().toInt());
    }
    if(albumCheck->isChecked()) {
        TagLib::String str(albumEdit->text().toStdString());
        tag->setAlbum(str);
    }
    if(artistCheck->isChecked()) {
        TagLib::String str(artistEdit->text().toStdString());
        tag->setArtist(str);
    }
    if(commentCheck->isChecked()) {
        TagLib::String str(commentEdit->text().toStdString());
        tag->setComment(str);
    }
    if(genreCheck->isChecked()) {
        TagLib::String str(genreEdit->currentText().toStdString());
        tag->setGenre(str);
    }
    if(yearCheck->isChecked()) {
        tag->setYear(yearEdit->text().toInt());
    }

    QString s(NamesOfTags::ID3V2.c_str());
    if(coverCheck->isChecked() && nameOfTag == s) {

        TagLib::ID3v2::Tag* id3Tag = (TagLib::ID3v2::Tag*) tag;
        id3Tag->removeFrames("APIC");

        if(coverEdit->text() != coverEditDefaultText) {
            TagLib::ID3v2::AttachedPictureFrame* frame = new TagLib::ID3v2::AttachedPictureFrame();
            if(coverEdit->text().endsWith(".jpeg", Qt::CaseInsensitive) ||
                    coverEdit->text().endsWith(".jpg", Qt::CaseInsensitive))
                frame->setMimeType("image/jpeg");
            else if(coverEdit->text().endsWith(".png", Qt::CaseInsensitive))
                frame->setMimeType("image/png");
            PictureFile pictureFile(coverEdit->text().toStdString().c_str());
            frame->setPicture(pictureFile.getData());
            id3Tag->addFrame(frame);
        }

    }

    file->save();

}

void MultipleTaggingDialog::startTagging() {

    QMessageBox* message = new QMessageBox(this);
    message->setWindowTitle(windowTitle());
    message->setText("Please wait...");
    message->setStandardButtons(QMessageBox::NoButton);
    message->show();

    for(int i = 0; i < listOfFiles.length(); i++) {

        QString path = listOfFiles.at(i);
        if(apeCheck->isChecked())
            saveTagsTo(QString::fromStdString(NamesOfTags::APE), path);
        if(asfCheck->isChecked())
            saveTagsTo(QString::fromStdString(NamesOfTags::ASF), path);
        if(id3v1Check->isChecked())
            saveTagsTo(QString::fromStdString(NamesOfTags::ID3V1), path);
        if(id3v2Check->isChecked())
            saveTagsTo(QString::fromStdString(NamesOfTags::ID3V2), path);
        if(infoTagCheck->isChecked())
            saveTagsTo(QString::fromStdString(NamesOfTags::INFO), path);
        if(xiphCommentCheck->isChecked())
            saveTagsTo(QString::fromStdString(NamesOfTags::XIPH), path);

    }

    message->setText("Done.");
    message->setStandardButtons(QMessageBox::Ok);
    message->update();
    close();

}

void MultipleTaggingDialog::selectCover() {

    QFileDialog dialog(this);
    dialog.setOption(QFileDialog::DontUseNativeDialog);
#ifdef WIN32
    dialog.setDirectory("C:/");
#else
    dialog.setDirectory("/home");
#endif
    dialog.setNameFilter("Images (*.jpeg *.jpg *.png)");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.show();
    if(dialog.exec()) {
        QStringList nameList = dialog.selectedFiles();
        QString name = nameList.join("");
        coverEdit->setText(name);
    }

}
