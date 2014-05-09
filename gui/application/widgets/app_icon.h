#ifndef APP_ICON_H
#define APP_ICON_H

#include <QApplication>
#include <QIcon>

/**
 * This class represents an icon which uses a file path relative
 * to the path of the application directory to find the icons
 * directory.
 */
class AppIcon : public QIcon
{
public:
    /**
     * Constructs an AppIcon and adds the file specified by the argument.
     * @param fileName filename of the icon file
     */
    AppIcon(const QString &fileName) {
        //Linux
        addFile(QApplication::applicationDirPath() + "/../../gui/application/icons/" + fileName);
        //Windows
        //addFile(QApplication::applicationDirPath() + "/../../../gui/application/icons/" + fileName);
    };

    ~AppIcon() {};
};

#endif // APP_ICON_H
