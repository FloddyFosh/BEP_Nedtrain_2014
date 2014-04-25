#include "comparedialog.h"
#include "widgets/comparewindow.h"
#include "controller/exceptions.h"

CompareDialog::CompareDialog(Controller *controller, QWidget *parent) :
    FormDialog(parent), controller(controller)
{
	setWindowTitle(tr("Compare instances"));

	first = new QComboBox();
	second = new QComboBox();

	foreach (Instance *i, controller->getAllInstances()) {
		first->addItem(i->getFileName()=="" ? tr("untitled") : i->getFileName(), qVariantFromValue((void *) i));
	    second->addItem(i->getFileName()=="" ? tr("untitled") : i->getFileName(), qVariantFromValue((void *) i));
	}

    addFormField(new QLabel(tr("Select two instances to compare."), this));
	addFormField(tr("Instance 1"), first);
	addFormField(tr("Instance 2"), second);
	setUpLayout();
}

void CompareDialog::apply() {
	Instance* i1 = (Instance *) first->itemData(first->currentIndex()).value<void *>();
	Instance* i2 = (Instance *) second->itemData(second->currentIndex()).value<void *>();

	bool close = true;

	try {
	    Comparison *comp = new Comparison(i1, i2);
	    CompareWindow *compareWindow = new CompareWindow(comp);
	    compareWindow->setGeometry(controller->getMainWindow()->geometry());
	    compareWindow->show();
	}
	catch(ComparisonException  const& e) {
	    e.showErrorMessage(controller->getMainWindow());
	    close = false;
	}

	if(close) accept();
}
