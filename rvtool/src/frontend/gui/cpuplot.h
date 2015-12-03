#include <qwt_plot.h>
#include "cpustat.h"
#include <QVarLengthArray>

#define HISTORY 60 // seconds

class QwtPlotCurve;

class MonitorPlot : public QwtPlot
{
    Q_OBJECT
public:
    enum MonitorData
    {
        Buffer,
        PollingPeriod,
        AvgPollingPeriod,
        CV,
        NMonitorData
    };

    MonitorPlot( QWidget * = 0 );
    const QwtPlotCurve *cpuCurve( int id ) const
    {
        return data[id].curve;
    }

protected:
    void timerEvent( QTimerEvent *e );

private Q_SLOTS:
    void legendChecked( const QVariant &, bool on );

private:
    void showCurve( QwtPlotItem *, bool on );

    struct
    {
        QwtPlotCurve *curve;
        double data[HISTORY];
    } data[NMonitorData];

    double timeData[HISTORY];

    int dataCount;
    QVarLengthArray<int,1024> dataArray;
    MonitorStat cpuStat;
};
