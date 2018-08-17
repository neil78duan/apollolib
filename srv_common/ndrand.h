/* file ndrand.h
 *
 * math rand sample 
 *
 * create by duan
 *
 * 2015-9-23
 */

#ifndef _ND_RAND_H_
#define _ND_RAND_H_

#include <math.h>
#include <stdlib.h>
#include "nd_common/nd_define.h"

#define SAMPLE_TOTAL_TIMES 100

// return rand [min, max]
template<class T>
T ndrand_range(T lowlimit, T hilimit)
{
	if (lowlimit > hilimit){
		T a = lowlimit;
		lowlimit = hilimit;
		hilimit = a;
	}
	else if (lowlimit == hilimit) {
		return lowlimit;
	}
	return (T)((double)rand() / ((double)RAND_MAX) * (hilimit - lowlimit + 1) + lowlimit);
}

static inline int RandRange2(int range[2])
{
	return ndrand_range(range[0], range[1]);
}

bool ProbSampling1(int rate,int area = SAMPLE_TOTAL_TIMES); // 针对一个概率采样

int ProbSampling( int prob[], int num); //概率抽样 prob概率分布,只抽一次

int ProbSamplingEx( int prob[],int result[], int num); //一次抽样,结果索引放到result中 ,返回抽中个数
int SamplingSpecial(int prob[], int prob_num, int result[], int res_num) ;//从概率中采样指定的个数放到result中

typedef signed short sampleType_t;
class ndSampling
{
public:
	ndSampling();
	~ndSampling();
	void Init(sampleType_t rate, sampleType_t sample_times, sampleType_t success_times);
	sampleType_t getSampleTimes() { return m_sample_times; }
	sampleType_t getSuccessTimes() { return m_sample_times; }
	ndSampling &operator = (const ndSampling &r);
	bool doSampling();
private:
	void reset();
	sampleType_t m_rate;
	sampleType_t m_sample_times;
	sampleType_t m_success_times;
};

//权重累加概率计算,如果概率是30 ,一次抽样没中则概率加30 ,中了减100,每次结束后保存权重
class ndAccumulateWeightProb
{
public:
	ndAccumulateWeightProb();
	ndAccumulateWeightProb(const ndAccumulateWeightProb &r);
	~ndAccumulateWeightProb();

	void Init(sampleType_t rate, sampleType_t curWeight);
	bool doSampling();
	sampleType_t getCurWeight();
	ndAccumulateWeightProb &operator = (const ndAccumulateWeightProb &r);
private:
	void reset();
	sampleType_t m_rate;
	sampleType_t m_curWeight;
};

//test 
int samplingTest();

#endif
