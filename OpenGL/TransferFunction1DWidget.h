#ifndef TRANSFER_FUNCTION_1D_WIDGET_H
#define TRANSFER_FUNCTION_1D_WIDGET_H

#include <QWidget>
#include <QPolygonF>
#include <vector>
#include <list>
#include <fstream>
using std::vector;
using std::list;
using std::ofstream;
using std::ifstream;

class QRenderHoverPoints;
class QTransferFunction1DItem;

class TransferFunction1DWidget : public QWidget
{
	Q_OBJECT

public:
	TransferFunction1DWidget();
	TransferFunction1DWidget( QTransferFunction1DItem* tf_item );
	~TransferFunction1DWidget();

	enum DataValueType { TYPE_UNKNOWN = 0, TYPE_UNSIGNED_BYTE, TYPE_SHORT };
	enum TfLineType { TYPE_LINE = 0, TYPE_CURVE };

	void SetLineType( TfLineType type );

	void SetData( unsigned int data_size, short* data );
	void SetData( unsigned int data_size, unsigned char* data );
	void SetViewWindow( double window_level, double window_width );
	
	void GetTfColorAndAlpha( int tf_size, vector< float >& tf_values );
	void GetTfColor( int tf_size, vector< float >& tf_values );
	void GetTfAlpha( int tf_size, vector< float >& tf_values );

    friend ofstream& operator<<(ofstream &out,TransferFunction1DWidget &widget);
    friend ifstream& operator>>(ifstream &in ,TransferFunction1DWidget &widget);

	public slots:
		void OnViewWindowChanged( double window_level, double window_width );
		void OnHoverPointChanged();

signals:
	void ValueChanged();

protected:
	void paintEvent( QPaintEvent* event );
	void resizeEvent( QResizeEvent* event );

private:
	float min_value_, max_value_, view_min_value_, view_max_value_;
	vector< unsigned int > histogram_values_;
	int histogram_bin_size_;
	unsigned int max_histogram_height_;

	vector< float > tf_value_array_;

	void* data_;
	unsigned int data_size_;
	DataValueType data_type_;

	QRenderHoverPoints* render_hover_points_;
	QTransferFunction1DItem* tf_item_;

	TfLineType line_type_;

	void DrawHistogram();
	void DrawTfColorAndAlphaBar();
	void DrawScalePlate();
	template < typename T >
	void GenerateHistogramValues( unsigned int data_size, T* data );
	void InitWidget();
	void UpdateTfValueArray();
	void GenerateLineColorAndAlphaValue();
	void GenerateCurveColorAndAlphaValue();
};

#endif // TRANSFER_FUNCTION_1D_WIDGET_H
