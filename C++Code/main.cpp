#include <ctime>
#include <iostream>
#include <io.h>
#include <vector>
#include "wav_head.h"	
#include "KwsAudio.h"
#include "PreDataProcess.h"

#define FRAME_N 218				// 音频分割帧数
#define MELCOEFICIENT_N	12*3	// 取12个Mel系数,加上一阶和二阶差分
#define AUDIO_LEN 56000			// 语音段长度 16K*3.5	
#define NUM_CLASS 4				// 类别数 4类 


float ou_d[FRAME_N * MELCOEFICIENT_N] = { 0.f };
float wav_16K_data[AUDIO_LEN] = { 0.f };


// 测试用读取音频文件
int16_t buff[AUDIO_LEN+1];

using namespace std;

/************************************************************************
 * 函数名：get_class_score
 * 功能描述: 模型推理结果后处理，得到最大得分的类别及概率
 * 输入参数：
	@cls_scores：模型推理输出结果
	@score：得分最大类别的概率
	@classes：最大得分的类别
 * 返回值：void
 ************************************************************************/
static void get_class_score(const std::vector<float>& cls_scores, float score, int classes)
{
	double sum = 0.f;
	double softmax[NUM_CLASS] = { 0.f };
	int size = cls_scores.size();
	std::vector<std::pair<float, int> > vec;
	vec.resize(size);
	for (int i = 0; i < size; i++)
	{
		vec[i] = std::make_pair(cls_scores[i], i);
	}

	std::partial_sort(vec.begin(), vec.begin() + NUM_CLASS, vec.end(),
		std::greater<std::pair<float, int> >());

	classes = vec[0].second;
	for (int i = 0; i < NUM_CLASS; i++)
	{
		softmax[i] = exp(vec[i].first);
		sum += softmax[i];
	}

	score = softmax[0] / sum;

	const char* format = "%d %f\n";
	printf_s(format, classes, score);	// 类别对应的概率大于0.5则认为正确
}

int main()
{
	float score = 0;
	int classes = 0;

	PreDataProcess *data_prep = new PreDataProcess();	
	KwsAudio *kws_audio = new KwsAudio();

	clock_t startTime, endTime;

	// 以下部分代码是读取音频测试用，音频文件：16KHz 16位 3.5s
	FILE* fp = NULL;

	Wav wav;
	RIFF_t riff;
	FMT_t fmt;
	Data_t data;

	errno_t err;
	err = fopen_s(&fp, "./badSnore/1632604424714-1632604436714_01.wav", "rb");
	if (err) {
		printf("can't open audio file\n");
		exit(1);
	}
	fread(&wav, 1, sizeof(wav), fp);
	fread(&buff, 1, sizeof(buff), fp);  // 采样频率16K，时间为1s，总的点数为16K
	fclose(fp);
	// 以上部分代码仅测试用

	for (int i = 0; i < AUDIO_LEN; i++) {
		wav_16K_data[i] = buff[i+1];	// 将手机采集到音频传入到此数组中!!!
	}

	startTime = clock();//计时开始

	// 数据预处理
	data_prep->extract_mfcc_feature(wav_16K_data, ou_d);

	std::vector<float> cls_scores;
	kws_audio->run_kwssudio_model(ou_d, cls_scores);

	// 模型输出结果后处理，得到得分最高的类别（classes）及概率值（score）
	get_class_score(cls_scores, score, classes);

	endTime = clock();//计时结束
	std::cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;

	delete data_prep;
	delete kws_audio;

	return 0;
}
