#include "resourcedecrease.h"
#include "controller/exceptions.h"

ResourceDecrease::ResourceDecrease(int start, int end, int amount, int id, QObject *) :
	start(start), end(end), amount(amount), id(id)
{ resource = 0;}

int ResourceDecrease::getFrom(){
	return start;
}
int ResourceDecrease::getTill(){
	return end;
}
int ResourceDecrease::getAmount(){
	return amount;
}
int ResourceDecrease::getId(){
	return id;
}

int ResourceDecrease::getResourceId(){
	if(resource)
		return resource->id();
	else
		throw InstanceManipulationException("No resource was set in resource decrease");
}
void ResourceDecrease::setAmount(int a){
	amount = a;
}

void ResourceDecrease::setResource(Resource *r){
	resource = r;
}

QString ResourceDecrease::toString(){
	QString output = QString("D %1 %2 %3 \"D\"").arg(id).arg(start).arg(end);
	output+= "\n" + QString("A %1 0 %2 \"d\"").arg(id).arg(end-start);
	output+= "\n" + QString("Q %1 0 %2 %3").arg(id).arg(resource->id()).arg(amount);
	return output;
}


