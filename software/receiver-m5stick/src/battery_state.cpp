#include <battery_state.h>

const int datapoints_count = 30;
const double upperVoltage = 4.17;
const double lowerVoltage = 3.5;
const double voltageDiff = upperVoltage - lowerVoltage;
double datapoints[datapoints_count];
double voltage = 0;
int percentage = 100;

void calcBatteryPercentage(double _voltage)
{
    voltage = 0;
    for (int i = datapoints_count - 1; i > 0; i--)
    {
        datapoints[i] = datapoints[i - 1];
        voltage += datapoints[i];
    }
    datapoints[0] = _voltage;
    voltage += datapoints[0];
    voltage = round(voltage / datapoints_count * 100) / 100;

    percentage = (voltage - lowerVoltage) / voltageDiff * 100;
}

int getBatteryPercentage()
{
    return percentage;
}
double getBatteryVoltage()
{
    return voltage;
}
