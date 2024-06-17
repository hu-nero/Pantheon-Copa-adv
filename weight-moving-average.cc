#include "weight-moving-average.hh"
#include <cmath>
#include <numeric>
#include <fstream>

using namespace std;

WeightMovAvgClass::WeightMovAvgClass(const std::vector<double>& weights)
    : vWeights(weights), dInitialDelta(0.5), dStdThreshild(0.1)
{}

//加权移动平均
//返回平滑后的向量数据
double
WeightMovAvgClass::wma(const std::deque<double>& data)
{
    if (data.empty())
        return 0.0; // 确保数据不为空

    double weightSum = 0.0;
    double weightedSum = 0.0;
    int n = data.size();

    // 计算加权和与权重和
    for (int i = 0; i < n; ++i)
    {
        double weight = i + 1; // 线性增加的权重，你可以根据需要调整权重分配策略
        weightedSum += data[i] * weight;
        weightSum += weight;
    }

    return weightSum > 0 ? weightedSum / weightSum : 0.0; // 避免除以零
}

//标准差
double
WeightMovAvgClass::standard_deviation(const std::vector<double>& WmaValues)
{
    double mean = std::accumulate(WmaValues.begin(), WmaValues.end(), 0.0) / WmaValues.size();
    double variance = 0.0;
    for (double value : WmaValues) {
        variance += std::pow(value - mean, 2);
    }
    variance /= WmaValues.size();
    return std::sqrt(variance);
}

double
WeightMovAvgClass::adjust_delta(double time, double queuing_delay, const std::deque<double>& RttVector,double Wma, double Step)
{
    (void) time;
    (void) queuing_delay;

    static int countThresholdLower = 0; // 追踪(dThreshold < 2.0)条件连续满足的次数
    static int countThresholdHigher = 0; // 追踪(dThreshold > -2.0)条件连续满足的次数
    double dStdRtt = Wma;
    double dCurRtt = RttVector.back();
    double dThreshold = dStdRtt - dCurRtt;

    // 判断条件并更新计数器
    if (dThreshold < 2.0)
    {
        ++countThresholdLower;
        countThresholdHigher = 0; // 重置另一个条件的计数器
    } else if (dThreshold > -2.0)
    {
        ++countThresholdHigher;
        countThresholdLower = 0; // 重置另一个条件的计数器
    } else
    {
        // 如果都不满足，重置计数器
        countThresholdLower = 0;
        countThresholdHigher = 0;
    }

    // 只有当任一条件连续满足3次时，才进行调整
    if (countThresholdLower >= 3)
    {
        dInitialDelta -= Step;
        countThresholdLower = 0; // 调整后重置计数器
    } else if (countThresholdHigher >= 3)
    {
        dInitialDelta += Step;
        countThresholdHigher = 0; // 调整后重置计数器
    }

    // 保证delta在[0.05, 0.5]范围内
    dInitialDelta = std::max(0.05, std::min(0.5, dInitialDelta));

    return dInitialDelta;
}

void
WeightMovAvgClass::set_Delta(double Delta)
{
    dInitialDelta = Delta;
}
