#include "TransferFunction1DWidget.h"

#include <limits.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <iostream>

#include "QRenderHoverPoints.h"
#include "QTransferFunction1DItem.h"

#define TF_MARGIN 0.1
#define SCALEPLATE_SIZE 5
#define SCALEPLATE_TEXT_WIDTH 70
#define SCALEPLATE_TEXT_HEIGHT 20

TransferFunction1DWidget::TransferFunction1DWidget(){
	histogram_bin_size_ = 256;

	data_ = NULL;
	data_size_ = 0;
	data_type_ = TYPE_UNKNOWN;
	line_type_ = TYPE_LINE;
	tf_item_ = NULL;

	this->setFocusPolicy(Qt::ClickFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setMinimumHeight(200);
	this->setMinimumWidth(500);

	InitWidget();
}

TransferFunction1DWidget::TransferFunction1DWidget( QTransferFunction1DItem* tf_item ){
	histogram_bin_size_ = 256;

	data_ = NULL;
	data_size_ = 0;
	data_type_ = TYPE_UNKNOWN;
	line_type_ = TYPE_LINE;
	tf_item_ = tf_item;

	this->setFocusPolicy(Qt::ClickFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->setMinimumHeight(200);
	this->setMinimumWidth(500);

	InitWidget();
}

TransferFunction1DWidget::~TransferFunction1DWidget(){
	delete render_hover_points_;
}

void TransferFunction1DWidget::InitWidget(){

	QPolygonF points;
	points << QPointF(0.1 * this->width(), 0.9 * this->height()) << QPointF(0.9 * this->width(), 0.1 * this->height());
	render_hover_points_ = new QRenderHoverPoints(this, QRenderHoverPoints::CircleShape);
	render_hover_points_->setConnectionType(QRenderHoverPoints::LineConnection);
	render_hover_points_->setPoints(points);
	render_hover_points_->setPointLock(0, QRenderHoverPoints::LockToLeft);
	render_hover_points_->setPointLock(1, QRenderHoverPoints::LockToRight);
	render_hover_points_->setSortType(QRenderHoverPoints::XSort);
	histogram_values_.resize(histogram_bin_size_, 0);
	for (int i = 0; i < histogram_bin_size_; ++i){
		histogram_values_[i] = i;
	}
	max_histogram_height_ = histogram_bin_size_ - 1;
	max_value_ = 255;
	min_value_ = 0;
	view_max_value_ = 255;
	view_min_value_ = 0;
}

void TransferFunction1DWidget::SetLineType( TfLineType type ){
	line_type_ = type;
	if ( line_type_ == TYPE_CURVE )
		render_hover_points_->setConnectionType(QRenderHoverPoints::CurveConnection);
	else
		render_hover_points_->setConnectionType(QRenderHoverPoints::LineConnection);
}

void TransferFunction1DWidget::SetData( unsigned int data_size, short* data ){
	data_type_ = TYPE_SHORT;
	data_ = data;
	data_size_ = data_size;
	histogram_bin_size_ = 256;
	histogram_values_.clear();
	tf_value_array_.resize(histogram_bin_size_ * 4, 0.0f);
	min_value_ = SHRT_MIN - 50;
	max_value_ = SHRT_MAX + 50;

	GenerateHistogramValues(data_size, data);
}

void TransferFunction1DWidget::SetData( unsigned int data_size, unsigned char* data ){
	data_type_ = TYPE_UNSIGNED_BYTE;
	data_ = data;
	data_size_ = data_size;
	histogram_bin_size_ = 256;
	histogram_values_.clear();
	tf_value_array_.resize(histogram_bin_size_ * 4, 0.0f);
	min_value_ = 0 - 50;
	max_value_ = UCHAR_MAX + 50;

	GenerateHistogramValues(data_size, data);
}

void TransferFunction1DWidget::SetViewWindow( double window_level, double window_width ){
	view_min_value_ = window_level - window_width / 2;
	view_max_value_ = window_level + window_width / 2;

	if ( view_max_value_ > max_value_ ) view_max_value_ = max_value_;
	if ( view_min_value_ < min_value_ ) view_min_value_ = min_value_;

	switch ( data_type_ ){
	case TYPE_UNSIGNED_BYTE:
		GenerateHistogramValues(data_size_, (unsigned char*)data_);
		break;
	case TYPE_SHORT:
		GenerateHistogramValues(data_size_, (short*)data_);
		break;
	default:
		break;
	}
	this->update();
}

void TransferFunction1DWidget::GetTfColorAndAlpha( int tf_size, vector< float >& tf_values ){
	tf_values.resize(tf_size * 4, 0);
	float step_size = histogram_bin_size_ / tf_size;
	float temp_index = 0;
	int index;
	for ( int i = 0; i < tf_size; ++i ){
		index = (int)temp_index;
		tf_values[i * 4] = tf_value_array_[index * 4];
		tf_values[i * 4 + 1] = tf_value_array_[index * 4 + 1];
		tf_values[i * 4 + 2] = tf_value_array_[index * 4 + 2];
		tf_values[i * 4 + 3] = tf_value_array_[index * 4 + 3];

		temp_index += step_size;
	}
}

void TransferFunction1DWidget::GetTfColor( int tf_size, vector< float >& tf_values ){
	tf_values.resize(3 * tf_size, 0);
	float step_size = histogram_bin_size_ / tf_size;
	float temp_index = 0;
	int index;
	for ( int i = 0; i < tf_size; ++i ){
		index = (int)temp_index;
		tf_values[i * 3] = tf_value_array_[index * 4];
		tf_values[i * 3 + 1] = tf_value_array_[index * 4 + 1];
		tf_values[i * 3 + 1] = tf_value_array_[index * 4 + 2];
		temp_index += step_size;
	}
}

void TransferFunction1DWidget::GetTfAlpha( int tf_size, vector< float >& tf_values ){
	tf_values.resize(tf_size, 0);
	float step_size = histogram_bin_size_ / tf_size;
	float temp_index = 0;
	int index;
	for ( int i = 0; i < tf_size; ++i ){
		index = (int)temp_index;
		tf_values[i] = tf_value_array_[index * 4 + 3];
		temp_index += step_size;
	}
}

void TransferFunction1DWidget::OnViewWindowChanged( double window_level, double window_width ){
	SetViewWindow(window_level, window_width);
}

void TransferFunction1DWidget::OnHoverPointChanged(){
	UpdateTfValueArray();
	
	if ( tf_item_ != NULL ){
		tf_item_->SetTfValue(tf_value_array_.size() / 4, tf_value_array_);
	}

	emit ValueChanged();
}

void TransferFunction1DWidget::paintEvent( QPaintEvent* event ){
	DrawHistogram();
	DrawTfColorAndAlphaBar();
	DrawScalePlate();
}

void TransferFunction1DWidget::resizeEvent( QResizeEvent* event ){

}

void TransferFunction1DWidget::UpdateTfValueArray(){
	tf_value_array_.resize(histogram_bin_size_ * 4, 0);
	tf_value_array_.assign(histogram_bin_size_ * 4, 0);
	
	switch ( line_type_ ){
	case TYPE_CURVE:
		GenerateCurveColorAndAlphaValue();
		break;
	default:
		GenerateLineColorAndAlphaValue();
		break;
	}
}

void TransferFunction1DWidget::GenerateLineColorAndAlphaValue(){
	QPolygonF points = render_hover_points_->points();
	QVector<QColor> colors = render_hover_points_->getRepresentColor();

	float width = (this->width() * 0.8 - 0.5) / histogram_bin_size_;
	float x = width * 0.5f + this->width() * 0.1;
	float normalValue = exp(0.95) - 1.0;
	int index = 0;
	for ( int i = 0; i < histogram_bin_size_; ++i )
	{
		while ( points.at(index + 1).x() <= x ) index++;
		float rate = float(x - points.at(index).x()) / (points.at(index + 1).x() - points.at(index).x());
        float tmp = colors.at(index).redF();
		tf_value_array_[i * 4 + 0] = colors.at(index).redF()   * (1 - rate) + colors.at(index + 1).redF()   * rate;
		tf_value_array_[i * 4 + 1] = colors.at(index).greenF() * (1 - rate) + colors.at(index + 1).greenF() * rate;
		tf_value_array_[i * 4 + 2] = colors.at(index).blueF()  * (1 - rate) + colors.at(index + 1).blueF()  * rate;
		tf_value_array_[i * 4 + 3] = ((0.9 - (double)points.at(index).y() / this->height()) * (1.0 - rate) + (0.9 - (double)points.at(index + 1).y() / this->height()) * rate) / 0.8;
		if ( tf_value_array_[i * 4 + 3] > 1.0 ) tf_value_array_[i * 4 + 3] = 1.0;
		x += width;
	}
}

void TransferFunction1DWidget::GenerateCurveColorAndAlphaValue(){

	QPolygonF points = render_hover_points_->points();
	QVector<QColor> colors = render_hover_points_->getRepresentColor();

	float width = (this->width() * 0.8 - 0.5) / histogram_bin_size_;
	float x = width * 0.5f + this->width() * 0.1;
	float normalValue = exp(0.95) - 1.0;
	int index = 0;
	for ( int i = 0; i < histogram_bin_size_; ++i )
	{
		while ( points.at(index + 1).x() <= x ) index++;
		float rate = float(x - points.at(index).x()) / (points.at(index + 1).x() - points.at(index).x());
		tf_value_array_[i * 4 + 0] = colors.at(index).redF()   * (1 - rate) + colors.at(index + 1).redF()   * rate;
		tf_value_array_[i * 4 + 1] = colors.at(index).greenF() * (1 - rate) + colors.at(index + 1).greenF() * rate;
		tf_value_array_[i * 4 + 2] = colors.at(index).blueF()  * (1 - rate) + colors.at(index + 1).blueF()  * rate;

		float p = 0.5f * rate - 0.25f;
		float q = std::sqrt(p * p + 0.015625f);
		float t = 0.5f + (p >= q ? std::pow(p - q, 0.3333333f) : -std::pow(q - p, 0.3333333f)) + std::pow(p + q, 0.3333333f);

		float alpha1 = 0.9 - (double)points.at(index).y() / this->height();
		float alpha2 = 0.9 - (double)points.at(index + 1).y() / this->height();
		tf_value_array_[i * 4 + 3] = alpha1 + (alpha2 - alpha1) * (3 * rate - 2 * t);
		tf_value_array_[i * 4 + 3] /= 0.8;
		if ( tf_value_array_[i * 4 + 3] > 1.0 ) tf_value_array_[i * 4 + 3] = 1.0;
		x += width;
	}
}

void TransferFunction1DWidget::DrawHistogram(){
	if ( histogram_bin_size_ == 0 ) return;

	float height = this->height();
	float width = this->width();
	float stepx = (float)this->width() * 0.8 / histogram_bin_size_;

	QPainterPath* histogram_outline_path = new QPainterPath;
	float preX, preY, curX, curY, distance;
	histogram_outline_path->moveTo(0.1 * width, 0.9 * height );
	preX = 0.1 * width + 0.5 * stepx;
	preY = 0.9 * height - log((double)histogram_values_[0] + 1) / log((double)max_histogram_height_ + 1) * 0.7 * height;
	histogram_outline_path->lineTo(0.1 * width, preY );
	histogram_outline_path->lineTo(preX, preY);
	for( int i = 1; i < histogram_bin_size_; ++i )
	{
		curX = (i + 0.5) * stepx + 0.1 * width;
		curY = 0.9 * height - log((double)histogram_values_[i] + 1) / log((double)max_histogram_height_ + 1) * 0.7 * height;
		distance = curX - preX;
		histogram_outline_path->cubicTo(preX + distance / 2, preY, curX + distance / 2, curY, curX, curY);

		preX = curX;
		preY = curY;
	}
	histogram_outline_path->lineTo( 0.9 * width, curY );
	histogram_outline_path->lineTo( 0.9 * width, 0.9 * height );
	histogram_outline_path->lineTo( 0.1 * width, 0.9 * height );
	histogram_outline_path->lineTo( 0.1 * width, 0.9 * height - log((double)histogram_values_[0] + 1) / log((double)max_histogram_height_ + 1) * 0.8 * height );

	QPainter temp_painter(this);
	QColor color(200, 200, 200, 125);
	temp_painter.setBrush(color);
	temp_painter.setPen(Qt::gray);
	temp_painter.setRenderHints(QPainter::Antialiasing);
	temp_painter.drawPath(*histogram_outline_path);
	temp_painter.end();
}

void TransferFunction1DWidget::DrawTfColorAndAlphaBar(){
	if ( render_hover_points_->points().size() < 2 ) return;

	QPainterPath* hover_point_path = new QPainterPath();
	hover_point_path->moveTo(0.1 * this->width(), 0.9 * this->height());
	QPolygonF polygon = render_hover_points_->points();
	switch ( line_type_ ){
	case TYPE_CURVE:{
		float preX, preY, curX, curY, distance;
		QPointF temp_point;
		temp_point = polygon.at(0);
		preX = temp_point.rx();
		preY = temp_point.ry();
		hover_point_path->lineTo(preX, preY);
		for ( int i = 0;  i < render_hover_points_->points().size(); ++i ){
			temp_point = polygon.at(i);
			curX = temp_point.rx();
			curY = temp_point.ry();
			distance = curX - preX;
			hover_point_path->cubicTo(preX + distance / 2, preY, preX + distance / 2, curY, curX, curY);
			preX = curX;
			preY = curY;
		}
					}
		break;
	default:
		hover_point_path->lineTo(polygon.at(0));
		for ( int i = 1;  i < polygon.size(); ++i )
			hover_point_path->lineTo(polygon.at(i));
		break;
	}
	hover_point_path->lineTo(0.9 * this->width(), 0.9 * this->height());

	//create the linear gradient.
	QLinearGradient linear_gradient;
	float width = this->width();
	linear_gradient.setStart( 0.00, 0.00 );
	linear_gradient.setFinalStop(this->width(), 0.00);

	for (int i = 0; i < render_hover_points_->points().size(); ++i){
		linear_gradient.setColorAt(render_hover_points_->points().at(i).x() / width, render_hover_points_->getRepresentColor().at(i));
	}

	QPainter temp_painter(this);
	temp_painter.setBrush(linear_gradient);
	temp_painter.setPen(Qt::darkGray);
	temp_painter.setRenderHints(QPainter::Antialiasing);
	temp_painter.drawPath(*hover_point_path);
	temp_painter.end();
}

void TransferFunction1DWidget::DrawScalePlate(){
	QPainter temp_painter(this);
	temp_painter.setPen(QPen(QBrush(Qt::black), 2));

	float height = this->height();
	float width = this->width();

	/// Draw x axis
	temp_painter.drawLine(QLine(0.1 * width, 0.9 *height, 0.95 * width, 0.9 * height));
	int temp_order = 1;
	while ( (view_max_value_ - view_min_value_) / temp_order > 10 ) temp_order *= 10;
	int temp_index_value = ((int)view_min_value_ / temp_order + 1) * temp_order;
	while ( temp_index_value < view_max_value_ ){
		float temp_x = 0.1 * width + (temp_index_value - view_min_value_) / ( view_max_value_ - view_min_value_) * 0.8 * width;
		float temp_y = 0.9 * height;
		temp_painter.drawLine(QLine(temp_x, temp_y, temp_x, temp_y - SCALEPLATE_SIZE));
		temp_painter.drawText(temp_x - SCALEPLATE_TEXT_WIDTH / 2, temp_y + 2, SCALEPLATE_TEXT_WIDTH, SCALEPLATE_TEXT_HEIGHT, Qt::AlignCenter | Qt::AlignTop, QString("%0").arg(temp_index_value));
		temp_index_value += temp_order;
	}
	temp_painter.drawLine(QLine(0.95 * width, 0.9 * height, 0.95 * width - SCALEPLATE_SIZE, 0.9 * height - SCALEPLATE_SIZE));
	temp_painter.drawLine(QLine(0.95 * width, 0.9 * height, 0.95 * width - SCALEPLATE_SIZE, 0.9 * height + SCALEPLATE_SIZE));
	temp_painter.drawText(0.95 * width - SCALEPLATE_TEXT_WIDTH / 2, 0.9 * height + 5, SCALEPLATE_TEXT_WIDTH, SCALEPLATE_TEXT_HEIGHT, Qt::AlignCenter | Qt::AlignTop, QString(tr("Scalar")));
	temp_painter.drawText(0.1 * width - SCALEPLATE_TEXT_WIDTH / 2, 0.9 * height + 5, SCALEPLATE_TEXT_WIDTH, SCALEPLATE_TEXT_HEIGHT, Qt::AlignCenter | Qt::AlignTop, QString(tr("%0")).arg(view_min_value_));
	temp_painter.drawText(0.9 * width - SCALEPLATE_TEXT_WIDTH / 2, 0.9 * height + 5, SCALEPLATE_TEXT_WIDTH, SCALEPLATE_TEXT_HEIGHT, Qt::AlignCenter | Qt::AlignTop, QString(tr("%0")).arg(view_max_value_));

	/// Draw y axis
	temp_painter.drawLine(QLine(0.1 * width, 0.9 * height, 0.1 * width, 0.05 * height));
	for ( int i = 1; i <= 10; ++i ){
		temp_painter.drawLine(QLine(0.1 * width, ((1 - i * 0.1) * 0.8 + 0.1) * height, 0.1 * width + SCALEPLATE_SIZE, ((1 - i * 0.1) * 0.8 + 0.1) * height));
		temp_painter.drawText(0.1 * width - SCALEPLATE_TEXT_WIDTH - 5, ((1 - i * 0.1) * 0.8 + 0.1) * height - SCALEPLATE_TEXT_HEIGHT / 2, SCALEPLATE_TEXT_WIDTH, SCALEPLATE_TEXT_HEIGHT, Qt::AlignRight | Qt::AlignCenter, QString("%0").arg(i * 0.1));
	}
	temp_painter.drawLine(QLine(0.1 * width, 0.05 * height, 0.1 * width - SCALEPLATE_SIZE, 0.05 * height + SCALEPLATE_SIZE));
	temp_painter.drawLine(QLine(0.1 * width, 0.05 * height, 0.1 * width + SCALEPLATE_SIZE, 0.05 * height + SCALEPLATE_SIZE));
	temp_painter.drawText(0.1 * width - SCALEPLATE_TEXT_WIDTH - 15, 0.05 * height, SCALEPLATE_TEXT_WIDTH, SCALEPLATE_TEXT_HEIGHT, Qt::AlignRight, QString("Opacity"));

	temp_painter.setPen(QPen(QBrush(Qt::gray), 1.0, Qt::DashLine));
	temp_painter.drawLine(QLine(0.1 * width, 0.1 * height, 0.9 * width, 0.1 * height));
}

template < typename T >
void TransferFunction1DWidget::GenerateHistogramValues( unsigned int data_size, T* data ){
	if ( data == NULL || histogram_bin_size_ == 0 ) return;

	histogram_values_.resize(histogram_bin_size_, 0);
	histogram_values_.assign(histogram_bin_size_, 0);

	for ( unsigned int i = 0; i < data_size; ++i ){
		int bin_index = (int)((data[i] - view_min_value_) / (view_max_value_ - view_min_value_) * histogram_bin_size_);
		if ( bin_index < 0 ) bin_index = 0;
		if ( bin_index >= histogram_bin_size_ ) bin_index = histogram_bin_size_ - 1;

		histogram_values_[bin_index]++;
	}

	max_histogram_height_ = 0;
	for ( int i = 0; i < histogram_bin_size_; ++i ) 
		if ( histogram_values_[i] > max_histogram_height_ ) max_histogram_height_ = histogram_values_[i];
}

ofstream& operator<<(ofstream& out, TransferFunction1DWidget& data){
    QPolygonF polygon = data.render_hover_points_->points();
    out<<polygon.size()<<'\n';
    for(int i=0;i<polygon.size();i++)
        out<<polygon.at(i).x()<<' '<<polygon.at(i).y()<<'\n';
    QVector<QColor> colors = data.render_hover_points_->getRepresentColor();
    for(int i=0;i<colors.size();i++)
        out<<colors[i].red()<<' '<<colors[i].green()<<' '<<colors[i].blue()<<' '<<colors[i].alpha()<<'\n';
    return out;        
}

void TransferFunction1DWidget::save(QPolygonF &p,QVector<QColor> &c) {
    p = render_hover_points_->points();
	c = render_hover_points_->getRepresentColor();
}

void TransferFunction1DWidget::load(QPolygonF polygon,QVector<QColor> c) {
	int size = polygon.size();
    render_hover_points_->setPoints(polygon);
	render_hover_points_->setPointLock(0, QRenderHoverPoints::LockToLeft);
	render_hover_points_->setPointLock(size-1, QRenderHoverPoints::LockToRight);
    for(int i=0;i<size;i++)
	render_hover_points_->setColor(i,c[i]);
    OnHoverPointChanged();
}

ifstream& operator>>(ifstream& in, TransferFunction1DWidget& data){
    int size,r,g,b,a;
    double x,y;
    in>>size;
    QPolygonF polygon(size);
    for(int i=0;i<size;i++){
        in>>x;
        in>>y;
        polygon[i].setX(x);
        polygon[i].setY(y);
    }
    data.render_hover_points_->setPoints(polygon);
	data.render_hover_points_->setPointLock(0, QRenderHoverPoints::LockToLeft);
	data.render_hover_points_->setPointLock(size-1, QRenderHoverPoints::LockToRight);
    for(int i=0;i<size;i++)
    {
        in>>r>>g>>b>>a;
        data.render_hover_points_->setColor(i,QColor(r,g,b,a));
    }
    data.OnHoverPointChanged();
    
        //in>>colors[i].red()>>colors[i].green()>>colors[i].blue();
    return in;        

}
