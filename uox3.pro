TEMPLATE        = app
win32-msvc:LIBS = ws2_32.lib
win32-msvc:DEFINES =  WIN32 NDEBUG _CONSOLE _MBCS
win32-msvc:LIBS      = ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comdlg32.lib
win32-msvc:TMAKE_CXXFLAGS = /J /nologo /ML /W3 /GX /O2 /YX /FD /c
unix:DEFINES    = __linux__
unix:LIBS       += -lncurses
unix:TMAKE_CXXFLAGS = -funsigned-char
win32-g++:TMAKE_CXXFLAGS   = -funsigned-char
win32-g++:LIBS = wsock32.lib
INCLUDEPATH     = h/
DESTDIR         = bin/
OBJECTS_DIR      = obj/
CONFIG          = console release
HEADERS		= h/admin.h \
		  h/basetype.h \
		  h/boats.h \
		  h/books.h \
		  h/characterhandle.h \
		  h/charstuff.h \
		  h/classes.h \
		  h/cmdtable.h \
		  h/combat.h \
		  h/commands.h \
		  h/craces.h \
		  h/cshop.h \
		  h/cweather.h \
		  h/debug.h \
		  h/defines.h \
		  h/fileio.h \
		  h/guilds.h \
		  h/gump.h \
		  h/im.h \
		  h/item.h \
		  h/itemhandle.h \
		  h/magic.h \
		  h/mapstaticiterator.h \
		  h/mapstuff.h \
		  h/movement.h \
		  h/msgboard.h \
		  h/networkstuff.h \
		  h/regions.h \
		  h/resource.h \
		  h/scriptc.h \
		  h/skills.h \
		  h/structs.h \
		  h/target.h \
		  h/teffect.h \
		  h/townstones.h \
		  h/typedefs.h \
		  h/uox3.h \
		  h/uoxlist.h \
		  h/uoxstruct.h \
		  h/weight.h \
		  h/wholist.h \
		  h/worldmain.h \
		  h/xgm.h
SOURCES		= cpp/admin.cpp \
		  cpp/archive.cpp \
		  cpp/boats.cpp \
		  cpp/books.cpp \
		  cpp/cmdtable.cpp \
		  cpp/combat.cpp \
		  cpp/commands.cpp \
		  cpp/craces.cpp \
		  cpp/cweather.cpp \
		  cpp/dbl_single_click.cpp \
		  cpp/debug.cpp \
		  cpp/fileio.cpp \
		  cpp/globals.cpp \
		  cpp/guildstones.cpp \
		  cpp/gumps.cpp \
		  cpp/house.cpp \
		  cpp/html.cpp \
		  cpp/im.cpp \
		  cpp/items.cpp \
		  cpp/magic.cpp \
		  cpp/mapstuff.cpp \
		  cpp/msgboard.cpp \
		  cpp/necro.cpp \
		  cpp/network.cpp \
		  cpp/newbie.cpp \
		  cpp/npcs.cpp \
		  cpp/p_ai.cpp \
		  cpp/pointer.cpp \
		  cpp/regions.cpp \
		  cpp/scriptc.cpp \
		  cpp/skills.cpp \
		  cpp/speech.cpp \
		  cpp/targeting.cpp \
		  cpp/teffect.cpp \
		  cpp/townstones.cpp \
		  cpp/trigger.cpp \
		  cpp/uox3.cpp \
		  cpp/walking.cpp \
		  cpp/weight.cpp \
		  cpp/wholist.cpp \
		  cpp/worldmain.cpp \
		  cpp/xgm.cpp
INTERFACES	= 
