#include "PreDataProcess.h"

#define M_2PI  6.28318530718

PreDataProcess::PreDataProcess()
{
	
}

PreDataProcess::~PreDataProcess()
{

}


/************************************************************************
 * ��������rfft
 * ��������: ����ʵ������FFT�任
 * ���������
 * @x[]���������ݣ�FFT�任��ļ�����Ҳ�����ڴ�����
 * @n���������ݳ��ȣ�һ��Ϊ2��n�η�
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::rfft( float x[], int n )
{
	int i, j, k, m, i1, i2, i3, i4, n1, n2, n4;
	float a, e, cc, ss, xt, t1, t2;
	for (j = 1, i = 1; i < 16; i++)
	{
		m = i;
		j = 2 * j;
		if (j == n)
			break;
	}
	n1 = n - 1;
	for (j = 0, i = 0; i < n1; i++)
	{
		if (i < j)
		{
			xt = x[j];
			x[j] = x[i];
			x[i] = xt;
		}
		k = n / 2;
		while (k < (j + 1))
		{
			j = j - k;
			k = k / 2;
		}
		j = j + k;
	}
	for (i = 0; i < n; i += 2)
	{
		xt = x[i];
		x[i] = xt + x[i + 1];
		x[i + 1] = xt - x[i + 1];
	}
	n2 = 1;
	for (k = 2; k <= m; k++)
	{
		n4 = n2;
		n2 = 2 * n4;
		n1 = 2 * n2;
		e = M_2PI / n1;
		for (i = 0; i < n; i += n1)
		{
			xt = x[i];
			x[i] = xt + x[i + n2];
			x[i + n2] = xt - x[i + n2];
			x[i + n2 + n4] = -x[i + n2 + n4];
			a = e;
			for (j = 1; j <= (n4 - 1); j++)
			{
				i1 = i + j;
				i2 = i - j + n2;
				i3 = i + j + n2;
				i4 = i - j + n1;
				cc = cos(a);
				ss = sin(a);
				a = a + e;
				t1 = cc * x[i3] + ss * x[i4];
				t2 = ss * x[i3] - cc * x[i4];
				x[i4] = x[i2] - t2;
				x[i3] = -x[i2] - t2;
				x[i2] = x[i1] - t1;
				x[i1] = x[i1] + t1;
			}
		}
	}
}


/************************************************************************
* ��������pre_add_weight
* ��������: �����ݽ���Ԥ���ز���
* ���������
   @in_data����������
   @out_data������Ԥ���ز���֮���������������
* ����ֵ��void
************************************************************************/
void PreDataProcess::pre_add_weight( float* in_data, float* out_data )
{
	out_data[0] = in_data[0] / 32768;
	for ( int i = 1; i < data_len; i++ )
	{
		out_data[i] = ( in_data[i] - 0.97 * in_data[i - 1] ) / 32768;
	}
}


/************************************************************************
 * ��������separate_frame
 * ��������: �����ݽ��з�֡����
 * ���������
	@in_data����������
	@out_data�������֡����֮���������������
	@overlap_len����֡����������֡���غ����ݳ���
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::separate_frame( float* in_data, float out_data[][frame_length] )
{
	int cnt = 0;

	for ( int i = 0; i < frame_num; i++ )
	{
		for ( int j = 0; j < frame_length; j++ )
		{
			if ( cnt >= data_len )
			{
				out_data[i][j] = 0.0;
			}
			else
			{
				out_data[i][j] = in_data[cnt++];
			}
		}
		cnt = cnt - frame_overlap_len;
	}
}


/************************************************************************
 * ��������add_window
 * ��������: �����ݽ��мӴ��������˴��Ǽ�hamming��
 * ���������
	@in_data���������ݣ�Ҳ���ڱ���Ӵ��������
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::add_window( float in_data[][frame_length] )
{
	float window_func[frame_length] = { 0.f };

	for (int i = 0; i < frame_length; i++)
		window_func[i] = 0.54 - 0.46 * cos(M_2PI * ((float)i) / ((float)frame_length - 1));	
	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < frame_length; j++)
		{
			in_data[i][j] *= window_func[j];
		}
	}
}


/************************************************************************
 * ��������fft_power_frame
 * ��������: FFT�任������������ֵ
 * ���������
	@in_data����������
	@out_data���������
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::fft_power_frame(float in_data[][frame_length], float out_data[][frame_length / 2 + 1])
{
	int n = frame_length;
	float fft_x[frame_length] = { 0.f };

	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < frame_length; j++)
		{
			fft_x[j] = in_data[i][j];
		}

		rfft(fft_x, n);

		out_data[i][0] = (1.0 / n) * ((fft_x[0]) * (fft_x[0]));
		out_data[i][1] = (1.0 / n) * ((fft_x[1]) * (fft_x[1]) + (fft_x[n - 1]) * (fft_x[n - 1]));
		for (int j = 2; j < n / 2; j += 2)
		{
			out_data[i][j] = (1.0 / n) * ((fft_x[j]) * (fft_x[j]) + (fft_x[n - j]) * (fft_x[n - j]));
			out_data[i][j + 1] = (1.0 / n) * ((fft_x[j + 1]) * (fft_x[j + 1]) + (fft_x[n - j - 1]) * (fft_x[n - j - 1]));
		}
		out_data[i][n / 2] = (1.0 / n) * ((fft_x[n / 2]) * (fft_x[n / 2]));

	}
}


/************************************************************************
 * ��������triangle_filter
 * ��������: �����ݽ��������˲�����
 * ���������
	@in_data����������
	@out_data���������
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::triangle_filter(float in_data[][frame_length / 2 + 1], float out_data[][mel_filt_num])
{
	float f_m_minus = 0.f;
	float f_m = 0.f;
	float f_m_plus = 0.f;
	int low_freq_mel = 0;
	float delata = 0.f;
	float high_freq_mel = 0.f;
	float mel_points[mel_filt_num + 2] = { 0.f };	// 40��mel�˲�������Ҫ42��Ƶ�ʵ�
	float hz_points[mel_filt_num + 2] = { 0.f };
	int bin[mel_filt_num + 2] = { 0 };
	float fbank[mel_filt_num][frame_length / 2 + 1] = { 0.f };

	high_freq_mel = 2595 * log10f(12.42857142857143);	// ���ݲ���Ƶ�ʶ� 1 + (sample_rate / 2) / 700) �ź�ʵ��Ƶ��(sample_rate / 2)
	delata = (high_freq_mel - low_freq_mel) / (mel_filt_num + 1);

	for (int i = 0; i < (mel_filt_num + 2); i++)
	{
		mel_points[i] = i * delata;
	}

	for (int i = 0; i < (mel_filt_num + 2); i++)
	{
		hz_points[i] = 700 * (powf(10, (mel_points[i] / 2595)) - 1);
	}

	for (int i = 0; i < (mel_filt_num + 2); i++)
	{
		bin[i] = floor((frame_length + 1) * hz_points[i] / sample_rate);	// ����Ƶ��
	}

	for (int i = 1; i < (mel_filt_num + 1); i++)
	{
		f_m_minus = bin[i - 1];
		f_m = bin[i];
		f_m_plus = bin[i + 1];
		for (int j = f_m_minus; j < f_m; j++)
		{
			fbank[i - 1][j] = (float)(j - bin[i - 1]) / (float)(bin[i] - bin[i - 1]);
		}
		for (int j = f_m; j < f_m_plus; j++)
		{
			fbank[i - 1][j] = (float)(bin[i + 1] - j) / (float)(bin[i + 1] - bin[i]);
		}
	}

	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < mel_filt_num; j++)
		{
			float sum = 0.f;
			for (int k = 0; k < (frame_length / 2 + 1); k++)	// FFT֮��ĵ���
			{
				sum += in_data[i][k] * fbank[j][k];
			}
			out_data[i][j] = sum;
		}
	}
	double eps = 1e-12; //10��-12�η�
	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < mel_filt_num; j++)
		{
			out_data[i][j] = 20 * log10f(out_data[i][j] + eps);
		}
	}
}


/************************************************************************
 * ��������dct
 * ��������: ��ɢ���ұ任
 * ���������
	@in_data����������
	@out_data���������
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::dct(float in_data[][mel_filt_num], float out_data[][mel_coeff_num])
{
	float normlize = 0.2236068;
	float dct_buff[mel_coeff_num][mel_filt_num] = { 0.f };

	for (int i = 1; i < 13; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			dct_buff[i - 1][j] = normlize * cos(((double)M_2PI) / 2 / 40 * (j + 0.5) * i);	
		}
	}

	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < mel_coeff_num; j++)
		{
			float sum = 0.f;
			for (int k = 0; k < mel_filt_num; k++)
			{
				sum += in_data[i][k] * dct_buff[j][k];
			}
			out_data[i][j] = sum;
		}
	}
}


/************************************************************************
 * ��������sin_liftering
 * ��������: ��������
 * ���������
	@in_data����������
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::sin_liftering(float in_data[][mel_coeff_num])
{
	float lift_buff[12] = { 0.f };
	uint8_t cep_lifter = 22;
	for (int i = 0; i < mel_coeff_num; i++)
	{
		lift_buff[i] = 1 + ((float)cep_lifter / 2) * sin((double)M_2PI / 2 * i / 22);	// 2PI = 6.2831853
	}

	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < mel_coeff_num; j++)
		{
			in_data[i][j] = in_data[i][j] * lift_buff[j];
		}
	}
}


/************************************************************************
 * ��������cal_normal
 * ��������: �����ݽ��й�һ��
 * ���������
	@in_data����������
	@out_data��������
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::cal_normal(float in_data[][mel_coeff_num], float* out_data)
{
	float a_min = 0.f;
	float a_max = 0.f;
	int cnt = 0;

	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < mel_coeff_num; j++)
		{
			if (in_data[i][j] > a_max)
			{
				a_max = in_data[i][j];
			}

			if (in_data[i][j] < a_min)
			{
				a_min = in_data[i][j];
			}
		}
	}

	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < mel_coeff_num; j++)
		{
			out_data[cnt++] = (in_data[i][j] - a_min) / (a_max - a_min);
		}
	}
}

/************************************************************************
 * ��������cal_mfcc_delta
 * ��������: ����mfcc��һ�ײ�ֺͶ��ײ��
 * ���������
	@in_data����������
	@out_data��������
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::cal_mfcc_delta(float in_data[][mel_coeff_num], float* out_data)
{
	int cnt = 0;
	float delat1[frame_num][mel_coeff_num] = { 0.f };
	float delat2[frame_num][mel_coeff_num] = { 0.f };

	// һ�ײ��
	for (int i = 0; i < mel_coeff_num; i++) {
		delat1[0][i] = (in_data[1][i] - in_data[0][i]) / 2;
	}

	for (int i = 2; i < frame_num; i++) {
		for (int j = 0; j < mel_coeff_num; j++) {
			delat1[i-1][j] = (in_data[i][j] - in_data[i-2][j]) / 2;
		}
	}

	for (int i = 0; i < mel_coeff_num; i++) {
		delat1[frame_num-1][i] = (in_data[frame_num-1][i] - in_data[frame_num-2][i]) / 2;
	}

	// ���ײ��
	for (int i = 0; i < mel_coeff_num; i++) {
		delat2[0][i] = ( (in_data[1][i] - in_data[0][i]) + 2 * (in_data[2][i] - in_data[0][i]) )/ 10;
		delat2[1][i] = ((in_data[2][i] - in_data[0][i]) + 2 * (in_data[3][i] - in_data[0][i])) / 10;
	}

	for (int i = 2; i < (frame_num - 2); i++) {
		for (int j = 0; j < mel_coeff_num; j++) {
			delat2[i][j] = ((in_data[i+1][j] - in_data[i-1][j]) + 2 * (in_data[i+2][j] - in_data[i-2][j])) / 10;
		}
	}

	for (int i = 0; i < mel_coeff_num; i++) {
		delat2[frame_num - 2][i] = ((in_data[frame_num - 1][i] - in_data[frame_num - 3][i]) + 2 * (in_data[frame_num - 1][i] - in_data[frame_num - 4][i])) / 10;
		delat2[frame_num - 1][i] = ((in_data[frame_num - 1][i] - in_data[frame_num - 2][i]) + 2 * (in_data[frame_num - 1][i] - in_data[frame_num - 3][i])) / 10;
	}

	// ƴ�ӽ����Ϊģ������
	for (int i = 0; i < frame_num; i++)
	{
		for (int j = 0; j < (3 * mel_coeff_num); j++)
		{
			if (j < mel_coeff_num)
				out_data[cnt++] = in_data[i][j];
			else if ( j >= (2 * mel_coeff_num) )
				out_data[cnt++] = delat2[i][j - 2 * mel_coeff_num];
			else 
				out_data[cnt++] = delat1[i][j - mel_coeff_num];
		}
	}

}

/************************************************************************
 * ��������extract_mfcc_feature
 * ��������: ��ȡ��Ƶ��MFCC����
 * ���������
	@in_data������ԭʼ��Ƶ����
	@out_data����ȡ��mfcc��Ƶ����
 * ����ֵ��void
 ************************************************************************/
void PreDataProcess::extract_mfcc_feature(float* in_data, float* out_data)
{
	float empha_data[data_len] = { 0.f };
	float frames[frame_num][frame_length] = { 0.f };
	float pow_frames[frame_num][frame_length / 2 + 1] = { 0.f };
	float filter_banks[frame_num][mel_filt_num] = { 0.f };
	float mfcc[frame_num][mel_coeff_num] = { 0.f };

	// 1. Ԥ���أ�ʵ��Ϊ��ͨ�˲���ͻ�������ź��еĸ�Ƶ�����y(t)=x(t) -��* x(t-1)
	pre_add_weight(in_data, empha_data);

	// 2. ��֡
	separate_frame(empha_data, frames);

	// 3.�Ӵ�
	add_window(frames);

	// 4. fft
	fft_power_frame(frames, pow_frames);

	// 5. �����˲�
	triangle_filter(pow_frames, filter_banks);

	// 6. DCT
	dct(filter_banks, mfcc);

	// 7. ��������Ӧ
	sin_liftering(mfcc);

	// 8. ����һ�ײ�ֺͶ��ײ��
	cal_mfcc_delta(mfcc, out_data);	// �˴�������ɾ��ԭ�ȵĹ�һ������

}

