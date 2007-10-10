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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "mainwindow.h"

static const char* DESC = I18N_NOOP("A simple media player");

int main(int argc, char** argv)
{
    KAboutData aboutData("kmediaplayer", 0, ki18n("KMediaPlayer"), "0.1",
                         ki18n(DESC), KAboutData::License_GPL,
                         ki18n("Copyright (c) 2007 Louai Al-Khanji"),
                         KLocalizedString(), QByteArray(),
                         "louai.khanji@gmail.com");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("+url", ki18n("The media to play"));
    KCmdLineArgs::addCmdLineOptions(options);

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    KApplication app;
    MainWindow* win = new MainWindow;
    win->show();

    if (args->count() > 0) { // just open the first file if passed many
        win->open(args->url(0));
    }

    return app.exec();
}
