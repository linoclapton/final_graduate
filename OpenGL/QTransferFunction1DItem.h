#ifndef QTRANSFER_FUNCTION_1D_ITEM_H_
#define QTRANSFER_FUNCTION_1D_ITEM_H_

#include <vector>
#include <QtCore/QObject>
#include "QPropertyItem.h"

class QTransferFunction1DItem : public QPropertyItem
{
	Q_OBJECT

public:
	QTransferFunction1DItem( std::string item_name, QPropertyItemType item_type );
	~QTransferFunction1DItem();

	void GetTfValues( int& entry_number, std::vector< float >& tf_values );
	int EntryNumber() { return entry_number_; }
	const std::vector< float >* TfValues() { return &tf_values_; }

	public slots:
		void SetTfValue( int entry_number, const std::vector< float >& tf_values );

signals:
	void ValueChanged();

private:
	int entry_number_;
	std::vector< float > tf_values_;
};

#endif