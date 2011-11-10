# Author: David Costa <david@zarel.net>
# turnMeOn project file

# Uncomment next line to build the doc
#CONFIG += documentation 
# Uncomment next line to rebuild the database (if needed)
#CONFIG += rebuild_db
# Uncomment the next line to build a release version
# CONFIG -= debug
# Uncomment next line to build a self-contained setup (for Windows)
CONFIG += installer

unix{
message(Building on Linux)

INST_PREFIX=/usr/local

#
# MAC Database
# (database is already present or should be rebuilt)

# sed must be present and sqlite3 too
rebuild_db:system(which sed &> /dev/null):system(which sqlite3 &> /dev/null){
	message("Database build is ON")
	!exists("res/oui.txt"){
		NO_OUI_TXT="Download oui.txt from http://standards.ieee.org/develop/regauth/oui/oui.txt"\
				   "and put it in the res/ folder"
		error($$NO_OUI_TXT)
	}else{
		macdb.commands="rm -f res/oui.db;"
		macdb.commands+="sed -f res/ouitosql.sed res/oui.txt | sqlite3 res/oui.db;"
		macdb.target=res/oui.db
		QMAKE_EXTRA_TARGETS+=macdb
		QMAKE_CLEAN+=res/oui.db
		PRE_TARGETDEPS+=res/oui.db
	
		
		}
} #if rebuild_db is off then oui.db must be already present
else {
	message("Database build is OFF")
	!exists("res/oui.db"):error("Cannot build. Rebuild the MAC database.")
}

#
# Documentation
#
	# Can be built only if doxygen is available AND 
	# documentation is present in CONFIG
	documentation:system(which doxygen &> /dev/null){
		message("Documentation build is ON")
		# Makefile target
		dox.commands=doxygen doc/Doxyfile;
		dox.target=doc/html
		QMAKE_EXTRA_TARGETS+=dox
		## There is NO WAY to delete this directory
		## but using this bash-only kludge.
		# The result is a Makefile with a target like this:
		# clean: ;rm -f `rm -Rf doc/html`
		# which will do the job.
		QMAKE_CLEAN+= `rm -Rf doc/html`
		#autobuild with the target
		PRE_TARGETDEPS+=doc/html
		# Documentation install set
		CONFIG+=copy_dir_files
		docs.path=$${INST_PREFIX}/doc/$$TARGET
		docs.files=doc/html
		INSTALLS+=docs
	}

#
# Database install set
#
database.path=$${INST_PREFIX}/share/$$TARGET
database.files=res/oui.db
INSTALLS+=database
# Let the application know where to find the database
# In order to obtain -DSYS_MAC_DB="\"something\""
# some strange quotes are needed
DEFINES+=SYS_MAC_DB="\"\\\"$${INST_PREFIX}/share/$$TARGET/oui.db\\\"\""

#
# turnMeOn install set
#
target.path=$${INST_PREFIX}/bin
INSTALLS+=target

} #End of linux configuration

#Windows configuration
#Typically only a release build needs to be done
#and an NSIS installer prepared.
win32{
	message(Building on Windows)
	CONFIG+=release
	CONFIG-=debug

	CONFIG(installer){
		nsis_script=build\\installer\\setup.nsi
		nsis_bin=$$(programFiles)\\NSIS\\makensis.exe
		target.path=build\\installer
		INSTALLS+=target

		database.path=build\\installer
		database.files=res/oui.db
		INSTALLS+=database

		setup.target=build\\installer
		setup.commands=$${nsis_bin} $${nsis_script}
		setup.path=build\\installer

		INSTALLS+=setup
	}
}

exists(build\\defaults.qmake){
	include(build\\defaults.qmake)
}
BUILD_DIR=build
VPATH=src
VPATH+=res
TEMPLATE = app
TARGET=turnMeOn
VERSION = 1.0.0
QT = core gui network sql
SOURCES += main.cpp magicPacket.cpp mainWnd.cpp
HEADERS += mainWnd.h magicPacket.h
FORMS += mainWnd.ui
RESOURCES += data.qrc
TRANSLATIONS += 
OBJECTS_DIR = $${BUILD_DIR}
MOC_DIR = $${BUILD_DIR}
RCC_DIR = $${BUILD_DIR}
UI_DIR = $${BUILD_DIR}


