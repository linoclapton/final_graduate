#ifndef QPROPERTY_ITEM_H_
#define QPROPERTY_ITEM_H_

#include <string>
#include <QObject>

class QPropertyItem : public QObject
{
	Q_OBJECT 

public:
	enum QPropertyItemType { VALUE, RANGE_VALUE, COLOR, TF1D };

	QPropertyItem( std::string item_name, QPropertyItemType item_type ){
		item_name_ = item_name;
		item_type_ = item_type;
	}

	~QPropertyItem() {}

	QPropertyItemType ItemType() { return item_type_; }
	std::string Name() { return item_name_; }

private:
	QPropertyItemType item_type_;
	std::string item_name_;
};

#endif