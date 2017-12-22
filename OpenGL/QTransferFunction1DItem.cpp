#include "QTransferFunction1DItem.h"

QTransferFunction1DItem::QTransferFunction1DItem( std::string item_name, QPropertyItemType item_type )
	: QPropertyItem(item_name, item_type){
	entry_number_ = 256;
	tf_values_.resize(1024, 0.0f);

	for ( int i = 0; i < entry_number_; ++i ){
		float temp_value = (float)i / entry_number_;
		/*tf_values_[i * 4] = temp_value;
		tf_values_[i * 4 + 1] = temp_value;
		tf_values_[i * 4 + 2] = temp_value;*/

		tf_values_[i * 4] = 1.0;
		tf_values_[i * 4 + 1] = 1.0;
		tf_values_[i * 4 + 2] = 1.0;

		tf_values_[i * 4 + 3] = temp_value;
	}
}

QTransferFunction1DItem::~QTransferFunction1DItem(){
	entry_number_ = 0;
	tf_values_.clear();
}

void QTransferFunction1DItem::SetTfValue( int entry_number, const std::vector< float >& tf_values ){
	entry_number_ = entry_number;
	tf_values_.resize(tf_values.size());
	tf_values_.assign(tf_values.begin(), tf_values.end());

	emit ValueChanged();
}

void QTransferFunction1DItem::GetTfValues( int& entry_number, std::vector< float >& tf_values ){
	entry_number = entry_number_;
	tf_values.resize(tf_values_.size());
	tf_values.assign(tf_values_.begin(), tf_values_.end());
}