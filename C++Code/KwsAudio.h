#pragma once
#include <ncnn/net.h>	// 请先在Android平台配置ncnn推理库

class KwsAudio
{
public:
	KwsAudio();

	~KwsAudio();

	void run_kwssudio_model(float* in_data, std::vector<float> &cls_scores_list);

private:
	ncnn::Net kwsaduio;

	float score;
	int classes;
	static const int audio_frame_num = 218;
	static const int audio_mel_coeff_num = 36;
};

