/*
    Copyright (c) 2007 Louai Al-Khanji

    This file is part of KMediaPlayer.

    KMediaPlayer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published
    by the Free Software Foundation.

    KMediaPlayer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with KMediaPlayer; see the file COPYING. If not, see
    <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QByteArray>

#include <kurl.h>
#include <phonon/phononnamespace.h>

#include <kmainwindow.h>
#include "ui_mainwindow.h"

namespace Phonon {
    class MediaObject;
    class AudioOutput;
    class SeekSlider;
}

class QProgressBar;

class MainWindow : public KMainWindow, public Ui_MainWindow
{
    Q_OBJECT

    public:
        MainWindow();
        void open(KUrl url);

    public slots:
        void open();
        void stop();

    private slots:
        void mediaStateChanged(Phonon::State newState, Phonon::State oldState);
        void mediaMetaDataChanged();
        void mediaHasVideoChanged(bool available);
        void mediaBufferStatus(int percent);
        void mediaTick(qint64 time);

        void toggleFullScreen(bool toggle);
        void playTriggered(bool checked);

    protected:
        bool eventFilter(QObject* obj, QEvent* event);

    private:
        Phonon::MediaObject* m_mediaObject;
        Phonon::AudioOutput* m_audioOutput;
        Phonon::SeekSlider* m_seekSlider;
        QProgressBar* m_progress;

        void showStatusMessage(QString msg);
        QString formatMilliSeconds(qint64 millisecs);

        QByteArray m_normalGeometry;
        QByteArray m_fullScreenGeometry;
};

#endif // MAINWINDOW_H
