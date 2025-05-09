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

	void extract_mfcc_feature( float* in_data, float* out_data );	// 提取数据的mfcc特征

private:

	static const int sample_rate = 16000;		// 数据的采样频率
	static const int data_len = 56000;			// 输入数据长度

	static const int frame_num = 218;			// 数据分帧操作的帧数
	static const int frame_length = 512;		// 每帧数据的长度
	static const int frame_overlap_len = 256;	// 分帧时相邻两帧之间的重合长度

	static const int mel_filt_num = 40;			// mel三角滤波器组的个数
	static const int mel_coeff_num = 12;		// 提取12个Mel系数特征

private:

	void rfft( float x[], int n );	// 实数序列数据fft
	void pre_add_weight( float* in_data, float* out_data );	// 数据预加重
	void separate_frame( float* in_data, float out_data[][frame_length] );	// 数据分帧
	void add_window( float in_data[][frame_length] );	// 数据加窗
	void fft_power_frame( float in_data[][frame_length], float out_data[][frame_length / 2 + 1] );	// 计算fft及能量
	void triangle_filter( float in_data[][frame_length / 2 + 1], float out_data[][mel_filt_num] );	// 三角滤波
	void dct( float in_data[][mel_filt_num], float out_data[][mel_coeff_num] );	// 离散余弦比变换
	void sin_liftering( float in_data[][mel_coeff_num] );	// 正弦提升
	void cal_normal( float in_data[][mel_coeff_num], float* out_data);	// 数据归一化
	void cal_mfcc_delta(float in_data[][mel_coeff_num], float* out_data);	// 计算一阶差分和二阶差分
};

#endif /* PreDataProcess_h */

