#ifndef PreDataProcess_h
#define PreDataProcess_h
#include <math.h>
#include <stdint.h>

#pragma once

class PreDataProcess
{
public:

	PreDataProcess();
	~PreDataProcess();

	void extract_mfcc_feature( float* in_data, float* out_data );	// ��ȡ���ݵ�mfcc����

private:

	static const int sample_rate = 16000;		// ���ݵĲ���Ƶ��
	static const int data_len = 56000;			// �������ݳ���

	static const int frame_num = 218;			// ���ݷ�֡������֡��
	static const int frame_length = 512;		// ÿ֡���ݵĳ���
	static const int frame_overlap_len = 256;	// ��֡ʱ������֮֡����غϳ���

	static const int mel_filt_num = 40;			// mel�����˲�����ĸ���
	static const int mel_coeff_num = 12;		// ��ȡ12��Melϵ������

private:

	void rfft( float x[], int n );	// ʵ����������fft
	void pre_add_weight( float* in_data, float* out_data );	// ����Ԥ����
	void separate_frame( float* in_data, float out_data[][frame_length] );	// ���ݷ�֡
	void add_window( float in_data[][frame_length] );	// ���ݼӴ�
	void fft_power_frame( float in_data[][frame_length], float out_data[][frame_length / 2 + 1] );	// ����fft������
	void triangle_filter( float in_data[][frame_length / 2 + 1], float out_data[][mel_filt_num] );	// �����˲�
	void dct( float in_data[][mel_filt_num], float out_data[][mel_coeff_num] );	// ��ɢ���ұȱ任
	void sin_liftering( float in_data[][mel_coeff_num] );	// ��������
	void cal_normal( float in_data[][mel_coeff_num], float* out_data);	// ���ݹ�һ��
	void cal_mfcc_delta(float in_data[][mel_coeff_num], float* out_data);	// ����һ�ײ�ֺͶ��ײ��
};

#endif /* PreDataProcess_h */

