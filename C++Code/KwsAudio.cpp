#include "KwsAudio.h"


KwsAudio::KwsAudio()
{
	kwsaduio.load_param("cnn_kws_v2_3.param");	// ע�����·������
	kwsaduio.load_model("cnn_kws_v2_3.bin");
}

KwsAudio::~KwsAudio()
{

}


/************************************************************************
 * ��������run_kwssudio_model
 * ��������: ִ��ģ���������
 * ���������
	@in_data��Ԥ����֮�������
	@cls_scores_list����𼰶�Ӧ�÷ָ���
 * ����ֵ��void
 ************************************************************************/
void KwsAudio::run_kwssudio_model(float* in_data, std::vector<float> &cls_scores_list)
{
	ncnn::Mat in;
	ncnn::Mat tmp = ncnn::Mat(audio_frame_num * audio_mel_coeff_num);	// ģ�������ʽת�� 

	// ģ������ִ�й���
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