#include "aboutdialog.h"

#include "widgets/app_icon.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("About Activity scheduler"));
    setWindowIcon(AppIcon("icon.png"));
    setFixedWidth(600);
    setFixedHeight(280);

    buttonbox = new QDialogButtonBox(QDialogButtonBox::Close);
    text = new QTextEdit;
    logo = new QLabel;
    horizontal = new QHBoxLayout;
    vertical = new QVBoxLayout;

    createLayout();
    createSignals();
}

void AboutDialog::createLayout() {
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

    logo->setPixmap(QPixmap(qApp->applicationDirPath()+"/icons/about_logos.jpg"));

    horizontal->addWidget(logo, 0, Qt::AlignTop);
    horizontal->addSpacing(20);
    horizontal->addWidget(text);
    vertical->addLayout(horizontal);
    vertical->addWidget(buttonbox);

    setStyleSheet("QDialog { background: white }");
    setLayout(vertical);
}

void AboutDialog::createSignals() {
    connect(buttonbox, SIGNAL(rejected()), this, SLOT(close()));
}
