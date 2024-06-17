#ifndef WEIGHT_MOVING_AVERAGE_HH
#define WEIGHT_MOVING_AVERAGE_HH

#include <iostream>
#include <vector>
#include <deque>
#include <functional>

// Gradient Descent Class
class WeightMovAvgClass
{
public:
    // Constructor
    WeightMovAvgClass(const std::vector<double>& weights);
    double wma(const std::deque<double>& Vdata);
    static double standard_deviation(const std::vector<double>& WmaValues);
    double adjust_delta(double time, double queuing_delay, const std::deque<double>& RttVector,double Wma, double Step);
    void set_Delta(double Delta);

private:
    std::vector<double> vWeights;
    double dInitialDelta;
    double dStdThreshild;
};

#endif //WEIGHT_MOVING_AVERAGE_HH
