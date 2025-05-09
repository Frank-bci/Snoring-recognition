#include <ctime>
#include <iostream>
#include <io.h>
#include <vector>
#include "wav_head.h"	
#include "KwsAudio.h"
#include "PreDataProcess.h"

#define FRAME_N 218				// ��Ƶ�ָ�֡��
#define MELCOEFICIENT_N	12*3	// ȡ12��Melϵ��,����һ�׺Ͷ��ײ��
#define AUDIO_LEN 56000			// �����γ��� 16K*3.5	
#define NUM_CLASS 4				// ����� 4�� 


float ou_d[FRAME_N * MELCOEFICIENT_N] = { 0.f };
float wav_16K_data[AUDIO_LEN] = { 0.f };


// �����ö�ȡ��Ƶ�ļ�
int16_t buff[AUDIO_LEN+1];

using namespace std;

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
	printf_s(format, classes, score);	// ����Ӧ�ĸ��ʴ���0.5����Ϊ��ȷ
}

int main()
{
	float score = 0;
	int classes = 0;

	PreDataProcess *data_prep = new PreDataProcess();	
	KwsAudio *kws_audio = new KwsAudio();

	clock_t startTime, endTime;

	// ���²��ִ����Ƕ�ȡ��Ƶ�����ã���Ƶ�ļ���16KHz 16λ 3.5s
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
	fread(&buff, 1, sizeof(buff), fp);  // ����Ƶ��16K��ʱ��Ϊ1s���ܵĵ���Ϊ16K
	fclose(fp);
	// ���ϲ��ִ����������

	for (int i = 0; i < AUDIO_LEN; i++) {
		wav_16K_data[i] = buff[i+1];	// ���ֻ��ɼ�����Ƶ���뵽��������!!!
	}

	startTime = clock();//��ʱ��ʼ

	// ����Ԥ����
	data_prep->extract_mfcc_feature(wav_16K_data, ou_d);

	std::vector<float> cls_scores;
	kws_audio->run_kwssudio_model(ou_d, cls_scores);

	// ģ�������������õ��÷���ߵ����classes��������ֵ��score��
	get_class_score(cls_scores, score, classes);

	endTime = clock();//��ʱ����
	std::cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;

	delete data_prep;
	delete kws_audio;

	return 0;
}
