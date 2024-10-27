## Dependencies
- [Qt 6.8.x](https://www.qt.io/download)
- [make](https://www.gnu.org/software/make/)
- [g++](https://gcc.gnu.org/)
- [git](https://git-scm.com/)

### installing QT
1. Download the Qt Online Installer:

    - Go to https://www.qt.io/download-qt-installer
   - Click "Download Qt Online Installer"
   
2. Run the installer:

   - Create a free Qt account if you don't have one (required for downloading)
   - Sign in during installation process


3. In the installer, select:

   - Qt 6.6.x (latest stable version)
   Under Qt 6.6.x, select:

     - MinGW 64-bit (includes the g++ compiler)
     - Qt Debug Information Files (optional)

   - Under Developer and Designer Tools, select:

        - Qt Creator (IDE)
        - MinGW (compiler)
        - Qt Debug Information Files (optional)
        - Ninja




Add Qt and MinGW to your system PATH:

Open Windows Settings → System → About → Advanced System Settings
Click "Environment Variables"
Under "System Variables" find "Path"
Click "Edit" and add these paths (adjust based on your installation):
CopyC:\Qt\6.6.1\mingw_64\bin
C:\Qt\Tools\mingw1120_64\bin



Verify installation in Command Prompt:

bashCopy# Check if Qt is installed
qmake --version

### Check if g++ is installed
    g++ --version

### Check if moc is available
    moc --version
For your Makefile to work, update these paths:
makefileCopy# Windows-specific settings
QTDIR = C:/Qt/6.6.1/mingw_64  # Adjust version number if different
QT_LIBS = -L$(QTDIR)/lib -lQt6Widgets -lQt6Core -lQt6Gui
