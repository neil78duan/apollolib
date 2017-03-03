/* file pg_misc.cpp
 *
 * misc of pg game
 *
 * create by duan
 *
 * 2012/3/12 14:57:34
 */

#include "srv_common/ndrand.h"


bool ProbSampling1(int rate)
{
	int rval = ndrand_range(0, SAMPLE_TOTAL_TIMES);
	if (rate >= rval) {
		return true;
	}
	return false;
}
//概率抽样
int ProbSampling(int prob[], int num)
{
	int sum = 0;
	for(int i=0; i<num; i++) {
		sum += prob[i] ;
	}
	if (sum <= 0){
		return -1 ;
	}

	int sampled = ndrand_range(0, sum) ;
	int i = 0 ;
	for( i=0; i<num; i++) {
		if (sampled< prob[i]){
			break;
		}
		sampled -= prob[i];
	}
	return i;
}

int ProbSamplingEx( int prob[],int result[], int num)
{
	int sum = 0;
	for(int i=0; i<num; i++) {
		sum += prob[i] ;
	}
	if (sum <= 0){
		return -1 ;
	}
	int ret = 0 ;
	for(int i=0; i<num; i++) {
		int sampled = ndrand_range(0, sum);
		if (sampled< prob[i]){
			result[ret++] = i;
		}
	}
	return ret;
}

int SamplingSpecial(int prob[], int prob_num, int result[], int res_num)
{
	int sum = 0;
	for(int i=0; i<prob_num; i++) {
		sum += prob[i] ;
	}
	if (sum <= 0){
		return -1 ;
	}

	//逐个采样
	int ret = 0 ;
	for(int i=0; i<prob_num; i++) {
		int left_num = prob_num - i ;
		int need_num = res_num - ret;
		//剩余样本少于结果, 为了保证采样个数,后面都100%
		if (left_num <= need_num){
			for(int xx =0 ; xx<left_num; xx++) {
				result[ret] = i+ret;
				++ret;
			}
			return ret ;
		}
		int sampled = ndrand_range(0, sum);
		if (sampled< prob[i]){
			result[ret++] = i;
		}
	}
	return ret;
}

///////////////////////////////////////////////////////////////
// class ndSampling
ndSampling::ndSampling()
{
	reset();
}
void ndSampling::Init(sampleType_t rate, sampleType_t sample_times, sampleType_t success_times)
{
	m_rate = rate;
	m_sample_times = sample_times;
	m_success_times = success_times;
}
ndSampling::~ndSampling()
{

}

ndSampling& ndSampling::operator=(const ndSampling &r)
{
	m_rate = r.m_rate;
	m_sample_times = r.m_sample_times;
	m_success_times = r.m_success_times;
	return *this;
}
bool ndSampling::doSampling()
{
	bool ret = false;
	int left_success = (int)m_rate - (int)m_success_times;
	sampleType_t left_times = (int)SAMPLE_TOTAL_TIMES - (int)m_success_times;
	if (left_success <= 0)	{
		ret = false;
	}
	else if (left_times <= left_success) {
		ret = true;
	}
	else {
		ret = ProbSampling1(m_rate);
		if (ret)	{
			++m_success_times;
		}
	}
	++m_sample_times;
	if (m_sample_times >= SAMPLE_TOTAL_TIMES){
		reset();
	}
	return ret;
}
void ndSampling::reset()
{
	m_rate = 0;
	m_sample_times = 0;
	m_success_times = 0;
}

///////////////////////////////////////////////////////////////////////////

ndAccumulateWeightProb::ndAccumulateWeightProb()
{
	Init(0,0);
}
ndAccumulateWeightProb::~ndAccumulateWeightProb()
{

}


ndAccumulateWeightProb::ndAccumulateWeightProb(const ndAccumulateWeightProb &r)
{
	Init(r.m_rate, r.m_curWeight);
}

void ndAccumulateWeightProb::Init(sampleType_t rate, sampleType_t curWeight)
{
	m_rate = rate; 
	m_curWeight = curWeight;
}
bool ndAccumulateWeightProb::doSampling()
{
	bool ret = false;
	m_curWeight += m_rate;

	if (m_curWeight <= 0){
		ret = false;
	}
	else if (m_curWeight >= SAMPLE_TOTAL_TIMES) {
		ret = true;
	}
	else {
		ret = ProbSampling1(m_curWeight);
	}

	if (ret){
		m_curWeight -= SAMPLE_TOTAL_TIMES;
	}
	return ret;
}
sampleType_t ndAccumulateWeightProb::getCurWeight()
{
	return m_curWeight;
}
ndAccumulateWeightProb &ndAccumulateWeightProb::operator=(const ndAccumulateWeightProb &r)
{
	m_curWeight = r.m_curWeight;
	m_rate = r.m_rate;
	return *this;
}
void ndAccumulateWeightProb::reset()
{
	m_curWeight = m_rate;
}

#include <stdio.h>
int samplingTest()
{
 	for (int i = 0; i < SAMPLE_TOTAL_TIMES * 2; i++){
 		printf("rand(0,%d) = %d\n", SAMPLE_TOTAL_TIMES, ndrand_range(0, SAMPLE_TOTAL_TIMES));
 	}

	int success_num = 0;
	
	ndSampling sampling1;
 	sampling1.Init(30, 0, 0);
 	for (int i = 0; i < SAMPLE_TOTAL_TIMES; i++){
 		bool val = sampling1.doSampling();
 		printf("sampling1.do(30) = %s\n", val ? "true" : "false");
 		if (val){
 			++success_num;
 		}
 	}
 	printf("success number =%d\n", success_num);

// 	sampling1.Init(30, 10, 10);
// 	for (int i = 0; i < SAMPLE_TOTAL_TIMES - 10; i++){
// 		bool val = sampling1.doSampling();
// 		printf("sampling1.do(10) = %s\n", val ? "true" : "false");
// 		if (val){
// 			++success_num;
// 		}
// 	}
// 	printf("success number =%d\n", success_num);

	ndAccumulateWeightProb awp;
	awp.Init(5, 0);

	for (int i = 0; i < SAMPLE_TOTAL_TIMES *10; i++){
		bool val = awp.doSampling();
		printf("accumulateWeightProb.do(5,5) = %s\n", val ? "true" : "false");
		if (val){
			++success_num;
		}
	}
	printf("success number =%d\n", success_num);
	return 0;
}
