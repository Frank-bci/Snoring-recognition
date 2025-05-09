#include "PreDataProcess.h"
#include "KwsAudio.h"

#define FRAME_N 218				// ��Ƶ�ָ�֡��
#define MELCOEFICIENT_N	12*3	// ȡ12��Melϵ��,����һ�׺Ͷ��ײ��
#define AUDIO_LEN 56000			// �����γ��� 16K*3.5	


float ou_d[FRAME_N * MELCOEFICIENT_N] = { 0.f };
float wav_16K_data[AUDIO_LEN] = { 0.f };

/************************************************************************
 * ��������get_class_score
 * ��������: ģ�������������õ����÷ֵ���𼰸���
 * ���������
	@cls_scores��ģ������������
	@score���÷�������ĸ���
	@classes�����÷ֵ����
 * ����ֵ��void
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
		wav_16K_data[i] = ...;	// ���ֻ��ɼ�����Ƶ���뵽��������!!!
	}

	// ����Ԥ����
	data_prep->extract_mfcc_feature(wav_16K_data, ou_d);
	
	// ģ������
	std::vector<float> cls_scores;
	kws_audio->run_kwssudio_model(ou_d, cls_scores);

	// ģ�������������õ��÷���ߵ����classes��������ֵ��score��
	get_class_score(cls_scores, score, classes);

	delete data_prep;
	delete kws_audio;
}
