#include "KwsAudio.h"


KwsAudio::KwsAudio()
{
	kwsaduio.load_param("cnn_kws_v2_3.param");	// 注意加载路径设置
	kwsaduio.load_model("cnn_kws_v2_3.bin");
}

KwsAudio::~KwsAudio()
{

}


/************************************************************************
 * 函数名：run_kwssudio_model
 * 功能描述: 执行模型推理过程
 * 输入参数：
	@in_data：预处理之后的数据
	@cls_scores_list：类别及对应得分概率
 * 返回值：void
 ************************************************************************/
void KwsAudio::run_kwssudio_model(float* in_data, std::vector<float> &cls_scores_list)
{
	ncnn::Mat in;
	ncnn::Mat tmp = ncnn::Mat(audio_frame_num * audio_mel_coeff_num);	// 模型输入格式转换 

	// 模型推理执行过程
	for (int i = 0; i < (audio_frame_num * audio_mel_coeff_num); i++) {
		tmp[i] = in_data[i];
	}
	in = tmp.reshape(audio_mel_coeff_num, audio_frame_num, 1);

	ncnn::Extractor ex = kwsaduio.create_extractor();
	ex.input("input_1_blob", in);

	ncnn::Mat feat;
	ex.extract("dense_1_blob", feat);

	feat = feat.reshape(feat.w * feat.h * feat.c);

	cls_scores_list.resize(feat.w);
	for (int j = 0; j < feat.w; j++)
	{
		cls_scores_list[j] = feat[j];
	}

}