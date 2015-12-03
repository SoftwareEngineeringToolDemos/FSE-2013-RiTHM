#include <qdatetime.h>

class MonitorStat
{
public:
    MonitorStat();
    void statistic( double &user, double &system );
    QTime upTime() const;

    enum Value
    {
        Buffer,
        PollingPeriod,
        NValues
    };

private:
    void lookUp( double[NValues] ) const;
    double procValues[NValues];
};
