/*
 * File:   mainWnd.cpp
 * Author: zarel
 *
 * Created on October 22, 2011, 10:58 AM
 */

#include "mainWnd.h"
#include "magicPacket.h"
#include <QPalette>
#include <QString>
#include <QFont>
#include <QSqlError>
#include <QFile>
#include <QMessageBox>

/**
 * default constructor.
 *   -# Instructs the window to reset the fields color
 *    when they're changed.
 *   -# Connects the send button with the wake() slot.
 */
mainWnd::mainWnd() {
	//mandatory since the form is done in QtDesigner
    widget.setupUi(this);
	//the logo should use a monospaced font
	QFont a=widget.logo->font();
	a.setStyleHint(QFont::TypeWriter);
	widget.logo->setFont(a);

#ifdef USE_DEFAULTS
	widget.MACinput->setText(DEFAULT_MAC);
	widget.FQDNInput->setText(DEFAULT_FQDN);	
#endif

	//connect to the MAC database
	macDB=QSqlDatabase::addDatabase("QSQLITE");
	//look for the MAC db
	if(QFile("res/oui.db").exists())
		macDB.setDatabaseName("res/oui.db");
#ifdef SYS_MAC_DB
	if(QFile(SYS_MAC_DB).exists())
		macDB.setDatabaseName(SYS_MAC_DB);
#endif
	if(!macDB.open()){
		qDebug()<<"Cannot open OUI database. Quitting.";
		QMessageBox nodb;
		nodb.setText("Cannot open OUI database. Quitting.");
		nodb.exec();
		nodb.setText(macDB.lastError().text());
		nodb.exec();
		exit(1);
	}


	// prepare the vendor name query
	vendorQ=QSqlQuery();
	vendorQ.prepare(
		"SELECT name FROM "
		"prefix INNER JOIN vendor "
		"ON vendor.id=prefix.vendor WHERE mac LIKE ?");

	//connect send button click event
    connect(widget.sendButton,SIGNAL(clicked(void)),
			this,SLOT(wake(void)));
	//change color on change value
    connect(widget.FQDNInput,
			SIGNAL(textChanged(const QString &)),
			this,SLOT(resetColor()));
    connect(widget.MACinput,
			SIGNAL(textChanged(const QString &)),
			this,SLOT(resetColor()));

	//check the manufacturer on change
	connect(widget.MACinput,
			SIGNAL(textChanged(const QString &)),
			this,SLOT(checkManufacturer(const QString &)));

	//set the about window text
	aboutWnd.setText(ABOUT_TEXT);
	aboutWnd.setWindowTitle("About turnMeOn");
	aboutWnd.setIcon(QMessageBox::Information);	
	aboutWnd.setWindowIcon(QIcon(":/pictures/bell_icon.png"));

	//connect the about label to the about window
	connect(widget.aboutLabel,
			SIGNAL(linkActivated(const QString &)),
			&aboutWnd,SLOT(exec()));
	
	//don't wait to check what vendor is the default MAC
	checkManufacturer(widget.MACinput->text());

	//needed to make it red
	widget.sendButton->setAutoFillBackground(true);

}

//! empty destructor
mainWnd::~mainWnd() {
}

/**
 * prepares a new magic packet, fills in the data and
 * sends it.
 *
 * @note It changes the fields color when magicPacket
 * refuses their data.
 */
void mainWnd::wake(void){
    QByteArray mac=QByteArray();
	QString a=(widget.FQDNInput)->text();
	bool status=true;

	//read the MAC field
	mac.append(widget.MACinput->text());
	//create the magicPacket and fills it with these
	//instance data
    magicPacket w;
    if(!w.setMAC(mac)){
		goRed(widget.MACinput);
		status=false;
	}

	if(a.isEmpty()){
		//empty means LAN
		w.setIP(QString("255.255.255.255"));
	}else{
	    if(!w.setIP(a)){
			goRed(widget.FQDNInput);
			status=false;
		}
	}
    if(!w.setPort(widget.portInput->value())){
		goRed(widget.portInput);
		status=false;
	}

	//if status is false a field is not correct
	//and we shouldn't try to send
	if(!status) return;

	//send the MP as many times as requested
    for(int a=0; a<widget.timesInput->value(); a++){
		//remember if one of the packets gave error
        status=w.send()?status:false;
    }

	if(!status){
		widget.statusLabel->setText("No network connection");
	}else{
		widget.statusLabel->setText("");
	}
}

/**
 * change the widget background in order to
 * higlight an error in that field.
 * @note manipulates the widget palette
 * @param o the widget to be colored
 */
void mainWnd::goRed(QWidget* o){
    QPalette p=o->palette();
    p.setColor(QPalette::Base, errorColor);
    o->setPalette(p);
}

/**
 * reset the widget background to a normal color.
 * The widget is read by the signal emitter.
 */
void mainWnd::resetColor(){
    QWidget* o=(QWidget*) QObject::sender();
    QPalette p=o->palette();
    p.setColor(QPalette::Base, normalColor);
    o->setPalette(p);
}

/**
 * given the mac address it sets the form label
 * to the manufacturer name
 * @param mac The MAC address as a QString
 */
void mainWnd::checkManufacturer(const QString mac){
	QString vendorName;
	//extracts the manufacturer id
	QString vendor=mac.left(8).toUpper();
	//the line edit uses ":" while DB uses "-"
	//to separate bytes
	vendor.replace(":","-");
	//string incomplete if more than 2 dashes
	if(vendor.count("-")>2){
		widget.manufacturerLabel->setText("Not enough data");
	}else{	
		vendorQ.addBindValue(vendor);
		vendorQ.exec();

		if(vendorQ.first()){
			vendorName=vendorQ.value(0).toString();
			widget.manufacturerLabel->setText(vendorName);
		}else{
			widget.manufacturerLabel->setText("Unknown");
		}
	}
}
