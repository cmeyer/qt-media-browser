# Introduction #

The Media Browser project is a set of widgets for Qt that allow the user to browse their media libraries and drag and drop media into the hosting application.

It currently supports Mac OS and Windows.

The Media Browser project is used in [Photo to Movie slideshow software](http://www.lqgraphics.com/software/phototomovie.php).

# Details #

The media browser provides a library view which gives the user a list of libraries such as an iPhoto library or a top level folder. When the user selects a specific library, they will also be presented with the specific items in that library, such as a list of photos or songs. They can drag and drop media from the view to another view as urls.

Internally, media library parsing is threaded. Each parser adds its results to a library tree controller. The library tree controller has a thread-safe "live" view and a non-thread-safe "display" view. Periodically the user interface thread will synchronize the two by playing back the changes on the live view into the display view. This allows the parsers to run in threads but also follows the threading requirements of the widgets involved.

# Build Requirements #

Requires QtMultimediaKit

Requires boost\_1\_46\_1

# Build Instructions (Mac OS) #

Instructions for building on Mac OS with developer tools and Qt installed.

## Prerequsities ##

The source for the media browser uses QtMobility (with the multimedia module) and boost.

## Step by step instructions ##

Using Terminal, navigate to the directory in which you want to build the project. Then fill in the values for QTDIR and BOOSTDIR (if it is installed).

```
DEVDIR=`pwd`
QTDIR=__full_path_to_qt__
BOOSTDIR=__full_path_to_boost__
```

## Instructions for building QtMobility with multimedia module ##

This will install QtMobility and QtMultimediaKit frameworks into your Qt installation.

```
git clone git://gitorious.org/qt-mobility/qt-mobility.git qt-mobility
pushd cd qt-mobility
./configure -prefix $QTDIR -libdir $QTDIR/lib -modules multimedia -qmake-exec $QTDIR/bin/qmake
make
make install
# sudo make install
popd
```

## Get the source for the media browser ##

```
hg clone http://qt-media-browser.googlecode.com/hg/ MediaBrowserProject
cd MediaBrowserProject
```

## Create a symlink for boost (or grab the boost release) ##

```
ln -s $BOOSTDIR boost
# svn co http://svn.boost.org/svn/boost/tags/release/Boost_1_46_1 boost
```

## Configure media browser ##

```
# configure installation directory
echo "MBP_PREFIX = $QTDIR/lib" > config.pri

# choose your build architectures by commenting in one of the lines
echo "CONFIG += x86 ppc x86_64" >> config.pri
# echo "CONFIG += x86 ppc" >> config.pri
# echo "CONFIG += x86" >> config.pri
```

## Build the media browser ##

```
$QTDIR/bin/qmake -r MediaBrowserProject.pro
make
make install
# sudo make install
```

## Run the media browser ##

```
open MediaBrowserViewer/build/debug/MediaBrowserViewer.app
```

## Test media browser in a real application ##

You can also test the media browser by copying the installed framework into an application that utilizes the media browser. Photo to Movie is one such application.

Download Photo to Movie http://www.lqgraphics.com/software/download and install it in Applications.

```
rm -rf /Applications/Photo\ to\ Movie\ 5.0.app/Contents/Frameworks/MediaBrowser.framework
cp $QTDIR/lib/MediaBrowser.framework /Applications/Photo\ to\ Movie\ 5.0.app/Contents/Frameworks
```