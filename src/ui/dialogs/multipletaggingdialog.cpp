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


#include "ui/dialogs/multipletaggingdialog.h"
#include "core/main.h"
#include "core/picturefile.h"
#include "core/audiotag.h"
#include "actions/actions.h"
#include <QPushButton>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>

MultipleTaggingDialog::MultipleTaggingDialog(QWidget *parent, FileList *list, QList<AudioFile*>* selectedFiles) :
    QDialog(parent) {

    setWindowTitle("Multiple tagging");

    this->selectedFiles = selectedFiles;
    fileList = list;

    filesLabel = new QLabel("Files:", this);
    useNewFilesButton = new QRadioButton("Open files", this);
    useSelectedFilesButton = new QRadioButton("Use selected files", this);
    filesEdit = new QLineEdit(this);
    filesEdit->setReadOnly(true);
    selectFilesButton  = new QPushButton("Select files", this);
    filesGroup = new QGroupBox("Files", this);

    multipleTaggingButton = new QRadioButton("Multiple tagging", this);
    duplicateTagsButton = new QRadioButton("Duplicate tags", this);
    operationGroup = new QGroupBox("Operation:", this);

    sourceTagLabel = new QLabel("Source tag:", this);
    sourceTagBox = new QComboBox(this);
    sourceTagBox->addItem(QString(TagFormats::ID3V1.c_str()));
    sourceTagBox->addItem(QString(TagFormats::ID3V2.c_str()));
    sourceTagBox->addItem(QString(TagFormats::APE.c_str()));
    sourceTagBox->addItem(QString(TagFormats::ASF.c_str()));
    sourceTagBox->addItem(QString(TagFormats::XIPH.c_str()));
    sourceTagBox->addItem(QString(TagFormats::INFO.c_str()));
    duplicateTagsGroup = new QGroupBox("Duplicate tags", this);

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

    titleCheck = new QCheckBox("Title", this);
    titleCheck->setChecked(true);
    trackCheck = new QCheckBox("Track", this);
    trackCheck->setChecked(true);
    albumCheck = new QCheckBox("Album", this);
    albumCheck->setChecked(true);
    artistCheck = new QCheckBox("Artist", this);
    artistCheck->setChecked(true);
    genreCheck = new QCheckBox("Genre", this);
    genreCheck->setChecked(true);
    yearCheck = new QCheckBox("Year", this);
    yearCheck->setChecked(true);
    commentCheck = new QCheckBox("Comment", this);
    commentCheck->setChecked(true);
    coverCheck = new QCheckBox("Cover art", this);
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

    multipleTaggingButton->setChecked(true);
    selectOperation();

    QObject::connect(titleCheck, SIGNAL(clicked(bool)), titleEdit, SLOT(setEnabled(bool)));
    QObject::connect(trackCheck, SIGNAL(clicked(bool)), trackEdit, SLOT(setEnabled(bool)));
    QObject::connect(albumCheck, SIGNAL(clicked(bool)), albumEdit, SLOT(setEnabled(bool)));
    QObject::connect(artistCheck, SIGNAL(clicked(bool)), artistEdit, SLOT(setEnabled(bool)));
    QObject::connect(genreCheck, SIGNAL(clicked(bool)), genreEdit, SLOT(setEnabled(bool)));
    QObject::connect(yearCheck, SIGNAL(clicked(bool)), yearEdit, SLOT(setEnabled(bool)));
    QObject::connect(commentCheck, SIGNAL(clicked(bool)), commentEdit, SLOT(setEnabled(bool)));
    QObject::connect(coverCheck, SIGNAL(clicked(bool)), selectCoverButton, SLOT(setEnabled(bool)));

    QObject::connect(multipleTaggingButton, SIGNAL(toggled(bool)), this, SLOT(selectOperation()));
    QObject::connect(duplicateTagsButton, SIGNAL(toggled(bool)), this, SLOT(selectOperation()));
    QObject::connect(selectCoverButton, SIGNAL(clicked()), this, SLOT(selectCover()));
    QObject::connect(selectFilesButton, SIGNAL(clicked()), this, SLOT(openFiles()));
    QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(startTagging()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(useNewFilesButton, SIGNAL(toggled(bool)), filesEdit, SLOT(setEnabled(bool)));
    QObject::connect(useNewFilesButton, SIGNAL(toggled(bool)), selectFilesButton, SLOT(setEnabled(bool)));
    QObject::connect(useNewFilesButton, SIGNAL(toggled(bool)), filesLabel, SLOT(setEnabled(bool)));

    createLayout();

    useSelectedFilesButton->toggle();
    filesEdit->setEnabled(false);
    selectFilesButton->setEnabled(false);
    filesLabel->setEnabled(false);

}

void MultipleTaggingDialog::createLayout() {

    QGridLayout* layout = new QGridLayout(this);
    int i = 0;

    QGridLayout* layout1 = new QGridLayout();
    layout1->addWidget(useNewFilesButton, 0, 0);
    layout1->addWidget(useSelectedFilesButton, 0, 1);
    layout1->addWidget(filesLabel, 1, 0);
    layout1->addWidget(filesEdit, 1, 1);
    layout1->addWidget(selectFilesButton, 1, 2);
    filesGroup->setLayout(layout1);

    QHBoxLayout* layout2 = new QHBoxLayout();
    layout2->addWidget(multipleTaggingButton);
    layout2->addWidget(duplicateTagsButton);
    operationGroup->setLayout(layout2);

    QHBoxLayout* layout3 = new QHBoxLayout();
    layout3->addWidget(sourceTagLabel);
    layout3->addWidget(sourceTagBox);
    duplicateTagsGroup->setLayout(layout3);

    QGridLayout* layout4 = new QGridLayout();
    layout4->addWidget(apeCheck, 0, 0);
    layout4->addWidget(asfCheck, 0, 1);
    layout4->addWidget(id3v1Check, 1, 0);
    layout4->addWidget(id3v2Check, 1, 1);
    layout4->addWidget(xiphCommentCheck, 2, 0);
    layout4->addWidget(infoTagCheck, 2, 1);
    tagsGroup->setLayout(layout4);

    QGridLayout* layout5 = new QGridLayout();
    i = 0;
    layout5->addWidget(titleCheck, i, 0);
    layout5->addWidget(titleEdit, i, 1);
    i++;
    layout5->addWidget(trackCheck, i, 0);
    layout5->addWidget(trackEdit, i, 1);
    i++;
    layout5->addWidget(albumCheck, i, 0);
    layout5->addWidget(albumEdit, i, 1);
    i++;
    layout5->addWidget(artistCheck, i, 0);
    layout5->addWidget(artistEdit, i, 1);
    i++;
    layout5->addWidget(genreCheck, i, 0);
    layout5->addWidget(genreEdit, i, 1);
    i++;
    layout5->addWidget(yearCheck, i, 0);
    layout5->addWidget(yearEdit, i, 1);
    i++;
    layout5->addWidget(commentCheck, i, 0);
    layout5->addWidget(commentEdit, i, 1);
    i++;
    layout5->addWidget(coverCheck, i, 0);
    layout5->addWidget(coverEdit, i, 1);
    i++;
    layout5->addWidget(selectCoverButton, i, 0);
    editorGroup->setLayout(layout5);

    QHBoxLayout* layout6 = new QHBoxLayout();
    layout6->addWidget(okButton);
    layout6->addWidget(cancelButton);

    i = layout->rowCount();
    int j = 0;
    layout->addWidget(filesGroup, i, j);
    i++;
    layout->addWidget(operationGroup, i, j);
    i++;
    layout->addWidget(duplicateTagsGroup, i, j);
    i++;
    layout->addWidget(tagsGroup, i, j);
    i = 0;
    j++;
    layout->addWidget(editorGroup, i, j, 5, 1);
    i = layout->rowCount();
    layout->addLayout(layout6, i, j);

}

void MultipleTaggingDialog::openFiles() {

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
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

void MultipleTaggingDialog::startTagging() {

    QMessageBox* message = new QMessageBox(this);
    message->setWindowTitle(windowTitle());
    message->setText("Please wait...");
    message->setStandardButtons(QMessageBox::NoButton);
    message->show();

    QList<Actions::TagKeyAndValue> listOfValues;


    Actions::TagKeyAndValue value;
    if(titleCheck->isChecked()) {
        value.key = TagKeys::TITLE;
        value.value = titleEdit->text();
        listOfValues.append(value);
    }
    if(trackCheck->isChecked()) {
        value.key = TagKeys::TRACK;
        value.value = trackEdit->text();
        listOfValues.append(value);
    }
    if(albumCheck->isChecked()) {
        value.key = TagKeys::ALBUM;
        value.value = albumEdit->text();
        listOfValues.append(value);
    }
    if(artistCheck->isChecked()) {
        value.key = TagKeys::ARTIST;
        value.value = artistEdit->text();
        listOfValues.append(value);
    }
    if(genreCheck->isChecked()) {
        value.key = TagKeys::GENRE;
        value.value = genreEdit->currentText();
        listOfValues.append(value);
    }
    if(commentCheck->isChecked()) {
        value.key = TagKeys::COMMENT;
        value.value = commentEdit->text();
        listOfValues.append(value);
    }
    if(yearCheck->isChecked()) {
        value.key = TagKeys::YEAR;
        value.value = yearEdit->text();
        listOfValues.append(value);
    }

    QList<AudioFile*>* list;

    if(useNewFilesButton->isChecked()) {

        list = new QList<AudioFile*>();
        for(int i = 0; i < listOfFiles.length(); i++) {

            QString path = listOfFiles.at(i);
            AudioFile* file = fileList->getFileByPath(path);
            if(file == NULL) {
                fileList->addFileToList(path);
                file = fileList->getFileByPath(path);
            }

            list->append(file);

        }

    } else {
        list = selectedFiles;
    }

    QList<TagFormat> formats;
    if(id3v1Check->isChecked())
        formats.append(TagFormats::ID3V1);
    if(id3v2Check->isChecked())
        formats.append(TagFormats::ID3V2);
    if(apeCheck->isChecked())
        formats.append(TagFormats::APE);
    if(asfCheck->isChecked())
        formats.append(TagFormats::ASF);
    if(xiphCommentCheck->isChecked())
        formats.append(TagFormats::XIPH);
    if(infoTagCheck->isChecked())
        formats.append(TagFormats::INFO);


    bool coverArt = coverCheck->isChecked();
    QString coverArtPath = coverEdit->text();
    if(coverEdit->text() == coverEditDefaultText)
        coverArtPath = "";
    if(multipleTaggingButton->isChecked()) {
        Actions::tagMultipleFiles(list, formats, listOfValues, coverArt, coverArtPath);
    } else {
        TagFormat sourceFormat = sourceTagBox->currentText().toStdString();
        QList<TagKey> listOfKeys;
        for(int i = 0; i < listOfValues.length(); i++) {
            listOfKeys.append(listOfValues.at(i).key);
        }
        Actions::duplicateTags(list, sourceFormat, formats, listOfKeys, coverArt);
    }

    message->close();
    delete message;
    close();

}

void MultipleTaggingDialog::selectCover() {

    QFileDialog dialog(this);
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

void MultipleTaggingDialog::selectOperation() {

    if(multipleTaggingButton->isChecked()) {

        duplicateTagsGroup->setVisible(false);

        titleEdit->setVisible(true);
        trackEdit->setVisible(true);
        albumEdit->setVisible(true);
        artistEdit->setVisible(true);
        genreEdit->setVisible(true);
        commentEdit->setVisible(true);
        yearEdit->setVisible(true);
        coverEdit->setVisible(true);
        selectCoverButton->setVisible(true);

    } else {

        duplicateTagsGroup->setVisible(true);

        titleEdit->setVisible(false);
        trackEdit->setVisible(false);
        albumEdit->setVisible(false);
        artistEdit->setVisible(false);
        genreEdit->setVisible(false);
        commentEdit->setVisible(false);
        yearEdit->setVisible(false);
        coverEdit->setVisible(false);
        selectCoverButton->setVisible(false);

    }

}
