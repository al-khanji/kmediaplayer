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

#include <QProgressBar>
#include <QEvent>
#include <QMouseEvent>

#include <kdebug.h>
#include <kstatusbar.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kicon.h>
#include <kmenu.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <klocale.h>

#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

#include "mainwindow.h"

static const char* LOADING_MESSAGE = I18N_NOOP("Loading");
static const char* BUFFERING_MESSAGE = I18N_NOOP("Buffering");
static const char* ERROR_MESSAGE = I18N_NOOP("The following error"
                                             " was encountered: <br>"
                                             " <center>%1<center>");

static const int STATUS_MESSAGE_FIELD = 0;
static const int PLAYBACK_TIME_FIELD = 1;

MainWindow::MainWindow() : KMainWindow(),
                           m_mediaObject(0),
                           m_audioOutput(0),
                           m_seekSlider(0),
                           m_progress(0)
{
    setupUi(this);

    menubar->addMenu(helpMenu());

    openAction->setIcon(KIcon("document-open"));
    playAction->setIcon(KIcon("media-playback-start"));
    stopAction->setIcon(KIcon("media-playback-stop"));
    fullscreenAction->setIcon(KIcon("view-fullscreen"));
    quitAction->setIcon(KIcon("application-exit"));

    connect(openAction, SIGNAL(triggered()),
            this, SLOT(open()));
    connect(playAction, SIGNAL(triggered(bool)),
            this, SLOT(playTriggered(bool)));
    connect(stopAction, SIGNAL(triggered()),
            this, SLOT(stop()));
    connect(fullscreenAction, SIGNAL(toggled(bool)),
            this, SLOT(toggleFullScreen(bool)));

    videoWidget->hide();
    videoWidget->installEventFilter(this);
    fullscreenAction->setEnabled(false);

    QAction *a = 0;

    videoWidget->addAction(openAction);
    a = new QAction(this);
    a->setSeparator(true);
    videoWidget->addAction(a);
    videoWidget->addAction(playAction);
    videoWidget->addAction(stopAction);
    videoWidget->addAction(fullscreenAction);
    a = new QAction(this);
    a->setSeparator(true);
    videoWidget->addAction(a);
    videoWidget->addAction(quitAction);

    videoWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_mediaObject = new Phonon::MediaObject(this);
    m_audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory);

    createPath(m_mediaObject, m_audioOutput);
    createPath(m_mediaObject, videoWidget);

    connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(mediaStateChanged(Phonon::State, Phonon::State)));
    connect(m_mediaObject, SIGNAL(metaDataChanged()),
            this, SLOT(mediaMetaDataChanged()));
    connect(m_mediaObject, SIGNAL(hasVideoChanged(bool)),
            this, SLOT(mediaHasVideoChanged(bool)));
    connect(m_mediaObject, SIGNAL(bufferStatus(int)),
            this, SLOT(mediaBufferStatus(int)));
    connect(m_mediaObject, SIGNAL(tick(qint64)),
            this, SLOT(mediaTick(qint64)));

    m_seekSlider = new Phonon::SeekSlider(this);
    m_seekSlider->setMediaObject(m_mediaObject);

    playbackToolbar->addSeparator();
    playbackToolbar->addWidget(m_seekSlider);

    connect(playbackToolbar, SIGNAL(orientationChanged(Qt::Orientation)),
            m_seekSlider, SLOT(setOrientation(Qt::Orientation)));

    statusBar()->insertPermanentItem("", PLAYBACK_TIME_FIELD);
    statusBar()->insertPermanentItem("", STATUS_MESSAGE_FIELD);

    m_progress = new QProgressBar;
    statusBar()->addPermanentWidget(m_progress);
    m_progress->hide();

    resize(width(), 0);
}

void MainWindow::open()
{
    open(KFileDialog::getOpenUrl());
}

void MainWindow::open(KUrl url)
{
    fullscreenAction->setChecked(false);
    setCaption("");
    m_mediaObject->setCurrentSource(url);
    m_mediaObject->play();
}

void MainWindow::mediaStateChanged(Phonon::State newState,
                                   Phonon::State oldState)
{
    Q_UNUSED(oldState);
    switch (newState) {
    case Phonon::LoadingState:
        m_progress->show();
        m_progress->setMinimum(0);
        m_progress->setMaximum(0);
        showStatusMessage(i18n(LOADING_MESSAGE));
        break;
    case Phonon::BufferingState:
        m_progress->show();
        m_progress->setMinimum(0);
        m_progress->setMaximum(100);
        showStatusMessage(i18n(BUFFERING_MESSAGE));
        break;
    case Phonon::StoppedState:
    case Phonon::PausedState:
        playAction->setChecked(false);
        m_progress->hide();
        showStatusMessage("");
        break;
    case Phonon::PlayingState:
        playAction->setChecked(true);
        m_progress->hide();
        showStatusMessage("");
        break;
    case Phonon::ErrorState:
        KMessageBox::error(this,
                           i18n(ERROR_MESSAGE, m_mediaObject->errorString()),
                           i18n("Playback Error"));
        KApplication::kApplication()->quit();
        break;
    default:
        kDebug() << "Unknown Phonon::State with value" << newState;
        break;
    };
}

void MainWindow::mediaMetaDataChanged()
{
    QStringList artist = m_mediaObject->metaData("ARTIST");
    QStringList title = m_mediaObject->metaData("TITLE");
    QString caption;
    if (!artist.isEmpty()) {
        caption += artist[0];
    }
    if (!title.isEmpty()) {
        if (!title[0].isEmpty()) {
            caption += " - ";
            caption += title[0];
        }
    }
    if (!caption.isEmpty())
        setCaption(caption);
}

void MainWindow::mediaHasVideoChanged(bool available)
{
    videoWidget->setVisible(available);
    fullscreenAction->setEnabled(available);
    if (!available)
        resize(width(), 0);
}

void MainWindow::mediaBufferStatus(int percent)
{
    m_progress->setVisible(percent != 100);
    m_progress->setValue(percent);
}

QString MainWindow::formatMilliSeconds(qint64 millisecs)
{
    qint64 secs = millisecs / 1000;
    QString m = QString::number(secs / 60);
    QString s = QString::number(secs % 60);
    return QString("%1:%2").arg(m).arg(s, 2, QLatin1Char('0'));
}

void MainWindow::mediaTick(qint64 tick)
{
    QString currentTime = formatMilliSeconds(tick);
    QString totalTime = formatMilliSeconds(m_mediaObject->totalTime());
    QString result = QString("%1 / %2").arg(currentTime).arg(totalTime);
    statusBar()->changeItem(result, PLAYBACK_TIME_FIELD);
}

void MainWindow::showStatusMessage(QString msg)
{
    statusBar()->changeItem(msg, STATUS_MESSAGE_FIELD);
}

void MainWindow::playTriggered(bool checked)
{
    if (checked) {
        if (m_mediaObject->currentSource().type() != Phonon::MediaSource::Invalid) {
            m_mediaObject->play();
        } else {
            open();
        }
    } else {
        m_mediaObject->pause();
    }
}

void MainWindow::stop()
{
    m_mediaObject->stop();
}

void MainWindow::toggleFullScreen(bool toggle)
{
    if (toggle) {
        m_normalGeometry = saveGeometry();
        videoWidget->enterFullScreen();
        resize(width(), 0);
        m_fullScreenGeometry = saveGeometry();
        hide();
        fullscreenAction->setIcon(KIcon("view-restore"));
    } else {
        show();
        videoWidget->exitFullScreen();
        restoreGeometry(m_normalGeometry);
        fullscreenAction->setIcon(KIcon("view-fullscreen"));
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    bool retval = false;

    Phonon::VideoWidget* widget = qobject_cast<Phonon::VideoWidget*>(obj);
    if (!widget)
        return QMainWindow::eventFilter(obj, event);

    switch (event->type()) {
    case QEvent::LayoutRequest:
        retval = true;
        resize(sizeHint());
        break;
    case QEvent::MouseButtonPress:
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                if (widget->isWindow()) {
                    if (isVisible()) {
                        m_fullScreenGeometry = saveGeometry();
                        hide();
                    } else {
                        show();
                        restoreGeometry(m_fullScreenGeometry);
                        activateWindow();
                    }
                }
            }
            retval = QMainWindow::eventFilter(obj, event);
        }
        break;
    default:
        retval = QMainWindow::eventFilter(obj, event);
        break;
    }

    return retval;
}

#include "mainwindow.moc"
