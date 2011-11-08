/*
 * File:   main.cpp
 * Author: zarel
 *
 * Created on October 20, 2011, 12:48 PM
 */

/**
 * @todo
 *   -# nsis installer
 *   -# add menus (also rebuild DB)
 */

#include <QApplication>
#include <QPushButton>
#include "mainWnd.h"

//! entry point
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    mainWnd w;
    w.show();
   
    return app.exec();
}

