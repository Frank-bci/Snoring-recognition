#include "PreDataProcess.h"
#include "KwsAudio.h"

#define FRAME_N 218				// 音频分割帧数
#define MELCOEFICIENT_N	12*3	// 取12个Mel系数,加上一阶和二阶差分
#define AUDIO_LEN 56000			// 语音段长度 16K*3.5	


float ou_d[FRAME_N * MELCOEFICIENT_N] = { 0.f };
float wav_16K_data[AUDIO_LEN] = { 0.f };

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
	double softmax[3] = { 0.f };
	int size = cls_scores.size();
	std::vector<std::pair<float, int> > vec;
	vec.resize(size);
	for (int i = 0; i < size; i++)
	{
		vec[i] = std::make_pair(cls_scores[i], i);
	}

	std::partial_sort(vec.begin(), vec.begin() + 3, vec.end(),
		std::greater<std::pair<float, int> >());

	classes = vec[0].second;
	for (int i = 0; i < 3; i++)
	{
		softmax[i] = exp(vec[i].first);
		sum += softmax[i];
	}

	score = softmax[0] / sum;
}

int main()
{
	float score = 0;
	int classes = 0;
	
	PreDataProcess *data_prep = new PreDataProcess();
	KwsAudio *kws_audio = new KwsAudio();
	

	for (int i = 0; i < AUDIO_LEN; i++) {
		wav_16K_data[i] = ...;	// 将手机采集到音频传入到此数组中!!!
	}

	// 数据预处理
	data_prep->extract_mfcc_feature(wav_16K_data, ou_d);
	
	// 模型推理
	std::vector<float> cls_scores;
	kws_audio->run_kwssudio_model(ou_d, cls_scores);

	// 模型输出结果后处理，得到得分最高的类别（classes）及概率值（score）
	get_class_score(cls_scores, score, classes);

	delete data_prep;
	delete kws_audio;
}
