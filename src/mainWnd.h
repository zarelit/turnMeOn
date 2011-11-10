/* 
 * File:   mainWnd.h
 * Author: zarel
 *
 * Created on October 22, 2011, 10:58 AM
 */

#ifndef _MAINWND_H
#define	_MAINWND_H

#include "ui_mainWnd.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

#define errorColor QColor(255,80,80)
//!< defines the background color of a wrong field
#define normalColor QColor(255,255,255)
//!< defines the background color of a correct/unkown field
#define ABOUT_TEXT "turnMeOn: a wake on LAN/wake on WAN packet sender\n"\
	"\n Version 1.0"\
	"\n Written by David Costa <david@zarel.net>\n"\
	"Feel free to contact me for bugfixes and feature requests\n"\
	"\n Released under the GNU General Public License\n"\
	"\n http://turnMeOn.sourceforge.net"
//!< defines the text of the about message box

/**
 * @brief It's the main window of turnMeOn GUI.
 * It highlights fields after verification.
 */
class mainWnd : public QDialog {
    Q_OBJECT
public:
    mainWnd();
    virtual ~mainWnd();
public slots:
        void wake(void);
        void resetColor();
		void checkManufacturer(const QString mac);
private:
	//! access to form as a widget collection
    Ui::mainWnd widget;
    void goRed(QWidget* o);
	//! the connection between the MAC database and QT
	QSqlDatabase macDB;
	/**
	 * the query used to obtain the vendor name from
	 * the first three bytes of the MAC
	 */
	QSqlQuery vendorQ;
	//! a simple about window
	QMessageBox aboutWnd;
};

#endif	/* _MAINWND_H */

