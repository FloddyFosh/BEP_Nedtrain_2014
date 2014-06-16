#include "widgets/dialogs/aboutdialog.h"
#include "widgets/app_icon.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("About Activity scheduler"));
    setWindowIcon(AppIcon("icon.png"));
    setFixedWidth(600);
    setFixedHeight(280);

    QDialogButtonBox *buttonbox = new QDialogButtonBox(QDialogButtonBox::Close);

    QTextEdit *text = new QTextEdit;
    text->setReadOnly(true);
    text->setStyleSheet("QTextEdit { border: 0; background: white }");
    text->setCursor(Qt::ArrowCursor);
    text->setHtml(tr("<strong>Activity scheduler</strong> has been developed as part of a research \
            project for Delft University of Technology and NedTrain. This version \
            was based on earlier work by Ronald Evers (2010). <br /><br />\
            <strong>Developed by:</strong><br /> \
            E. Ammerlaan, J. Elffers, E. Walraven and W. Wisse.<br /><br /> \
            <strong>Continued by:</strong><br /> \
            C. Bakker, A. Bouter and M. den Hoedt.<br /><br /> \
            <strong>Supervised by:</strong><br /> \
            Prof. dr. C. Witteveen, ir. M. Wilson and ir. B. Huisman.<br /><br />"
            "&copy; 2014")
    );
    QLabel *logo = new QLabel;
    logo->setPixmap(QPixmap(qApp->applicationDirPath()+"/icons/about_logos.jpg"));

    QHBoxLayout *horizontal = new QHBoxLayout;
    QVBoxLayout *vertical = new QVBoxLayout;
    horizontal->addWidget(logo, 0, Qt::AlignTop);
    horizontal->addSpacing(20);
    horizontal->addWidget(text);
    vertical->addLayout(horizontal);
    vertical->addWidget(buttonbox);

    setStyleSheet("QDialog { background: white }");
    setLayout(vertical);
    connect(buttonbox, SIGNAL(rejected()), this, SLOT(close()));
}
