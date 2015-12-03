#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_canvas.h>
#include "cpuplot.h"

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QTime &base ):
        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QTime upTime = baseTime.addSecs( static_cast<int>( v ) );
        return upTime.toString();
    }
private:
    QTime baseTime;
};

class Background: public QwtPlotItem
{
public:
    Background()
    {
        setZ( 0.0 );
    }

    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }

    virtual void draw( QPainter *painter,
        const QwtScaleMap &, const QwtScaleMap &yMap,
        const QRectF &canvasRect ) const
    {
        QColor c( Qt::white );
        QRectF r = canvasRect;

        for ( int i = 100; i > 0; i -= 10 )
        {
            r.setBottom( yMap.transform( i - 10 ) );
            r.setTop( yMap.transform( i ) );
            painter->fillRect( r, c );

            c = c.dark( 110 );
        }
    }
};

class MonitorCurve: public QwtPlotCurve
{
public:
    MonitorCurve( const QString &title ):
        QwtPlotCurve( title )
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );
    }

    void setColor( const QColor &color )
    {
        QColor c = color;
        c.setAlpha( 150 );

        setPen( c );
        setBrush( c );
    }
};

MonitorPlot::MonitorPlot( QWidget *parent ):
    QwtPlot( parent ),
    dataCount( 0 )
{
    setAutoReplot( false );

    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setBorderRadius( 10 );

    setCanvas( canvas );

    plotLayout()->setAlignCanvasToScales( true );

    QwtLegend *legend = new QwtLegend;
    legend->setDefaultItemMode( QwtLegendData::Checkable );
    insertLegend( legend, QwtPlot::TopLegend );

    setAxisTitle( QwtPlot::xBottom, " System Uptime [h:m:s]" );
    setAxisScaleDraw( QwtPlot::xBottom,
        new TimeScaleDraw( cpuStat.upTime() ) );
    setAxisScale( QwtPlot::xBottom, 0, HISTORY );
    setAxisLabelRotation( QwtPlot::xBottom, -50.0 );
    setAxisLabelAlignment( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

    /*
     In situations, when there is a label at the most right position of the
     scale, additional space is needed to display the overlapping part
     of the label would be taken by reducing the width of scale and canvas.
     To avoid this "jumping canvas" effect, we add a permanent margin.
     We don't need to do the same for the left border, because there
     is enough space for the overlapping label below the left scale.
     */

    QwtScaleWidget *scaleWidget = axisWidget( QwtPlot::xBottom );
    const int fmh = QFontMetrics( scaleWidget->font() ).height();
    scaleWidget->setMinBorderDist( 0, fmh / 2 );
    enableAxis(QwtPlot::yRight,TRUE);

    setAxisTitle( QwtPlot::yLeft, "Percentage" );
    setAxisScale( QwtPlot::yLeft, 0, 1 );

    setAxisTitle( QwtPlot::yRight, "Polling Period [msec]" );
    setAxisAutoScale(QwtPlot::yRight,TRUE);

    Background *bg = new Background();
    bg->attach( this );

    MonitorCurve *curve;

    curve = new MonitorCurve( "Polling Period" );
    curve->setColor( Qt::red );
    curve->setAxes(QwtPlot::xBottom,QwtPlot::yRight);
    curve->attach( this );
    data[PollingPeriod].curve = curve;

    curve = new MonitorCurve( "Buffer Utilization" );
    curve->setColor( Qt::blue );
    curve->setAxes(QwtPlot::xBottom,QwtPlot::yLeft);
    curve->setZ( curve->z() - 1 );
    curve->attach( this );
    data[Buffer].curve = curve;

    curve = new MonitorCurve( "Average Polling Period" );
    curve->setColor( Qt::green );
    curve->setAxes(QwtPlot::xBottom,QwtPlot::yRight);
    curve->setZ( curve->z() - 1 );
    curve->attach( this );
    data[AvgPollingPeriod].curve = curve;

    curve = new MonitorCurve( "Coefficient of Variation" );
    curve->setColor( Qt::yellow );
    curve->setAxes(QwtPlot::xBottom,QwtPlot::yLeft);
    curve->setZ( curve->z() - 1 );
    curve->attach( this );
    data[CV].curve = curve;

    showCurve( data[PollingPeriod].curve, true );
    showCurve( data[Buffer].curve, true );
    showCurve( data[AvgPollingPeriod].curve, true );
    showCurve( data[CV].curve, true );

    for ( int i = 0; i < HISTORY; i++ )
        timeData[HISTORY - 1 - i] = i;

    ( void )startTimer( 1000 ); // 1 second

    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
        SLOT( legendChecked( const QVariant &, bool ) ) );
}

void MonitorPlot::timerEvent( QTimerEvent * )
{
    for ( int i = dataCount; i > 0; i-- )
    {
        for ( int c = 0; c < NMonitorData; c++ )
        {
            if ( i < HISTORY )
                data[c].data[i] = data[c].data[i-1];
        }
    }


    cpuStat.statistic( data[PollingPeriod].data[0], data[Buffer].data[0] );

    data[AvgPollingPeriod].data[0] = 0;

    for (int i = 0; i<dataCount; i++)
    {
        data[AvgPollingPeriod].data[0] += data[PollingPeriod].data[i];
    }

    data[AvgPollingPeriod].data[0] /= dataCount;

    data[CV].data[0] = 0;

    for (int i = 0; i<dataCount; i++)
    {
        data[CV].data[0] += pow(data[AvgPollingPeriod].data[0]-data[PollingPeriod].data[i],2);
    }

    data[CV].data[0] = sqrt(data[CV].data[0]/(dataCount-1));
    data[CV].data[0] = data[CV].data[0]/data[AvgPollingPeriod].data[0];

    if ( dataCount < HISTORY )
        dataCount++;

    for ( int j = 0; j < HISTORY; j++ )
        timeData[j]++;

    setAxisScale( QwtPlot::xBottom,
        timeData[HISTORY - 1], timeData[0] );

    for ( int c = 0; c < NMonitorData; c++ )
    {
        data[c].curve->setRawSamples(
            timeData, data[c].data, dataCount );
    }



    replot();
}

void MonitorPlot::legendChecked( const QVariant &itemInfo, bool on )
{
    QwtPlotItem *plotItem = infoToItem( itemInfo );
    if ( plotItem )
        showCurve( plotItem, on );
}

void MonitorPlot::showCurve( QwtPlotItem *item, bool on )
{
    item->setVisible( on );

    QwtLegend *lgd = qobject_cast<QwtLegend *>( legend() );

    QList<QWidget *> legendWidgets = 
        lgd->legendWidgets( itemToInfo( item ) );

    if ( legendWidgets.size() == 1 )
    {
        QwtLegendLabel *legendLabel =
            qobject_cast<QwtLegendLabel *>( legendWidgets[0] );

        if ( legendLabel )
            legendLabel->setChecked( on );
    }

    replot();
}
/*
int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    QWidget vBox;
    vBox.setWindowTitle( "Monitor Plot" );

    MonitorPlot *plot = new MonitorPlot( &vBox );
    plot->setTitle( "History" );

    const int margin = 5;
    plot->setContentsMargins( margin, margin, margin, margin );

    QString info( "Press the legend to en/disable a curve" );

    QLabel *label = new QLabel( info, &vBox );

    QVBoxLayout *layout = new QVBoxLayout( &vBox );
    layout->addWidget( plot );
    layout->addWidget( label );

    vBox.resize( 600, 400 );
    vBox.show();

    return a.exec();
}
*/
