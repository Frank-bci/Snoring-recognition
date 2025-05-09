#include <android/log.h>
#include "PreDataProcess.h"
#include "KwsAudio.h"

#define FRAME_N 218                // 音频分割帧数
#define MELCOEFICIENT_N    12*3    // 取12个Mel系数,加上一阶和二阶差分


float ou_d[FRAME_N * MELCOEFICIENT_N] = {0.f};

#define TAG "llcqh"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
static int NUM_CLASS = 5;

static PreDataProcess *data_prep;
static KwsAudio *kws_audio;

void init_model(const char *protopath, const char *modelpath, const int number) {
    data_prep = new PreDataProcess();
    kws_audio = new KwsAudio();
    NUM_CLASS = number;
    kws_audio->init(protopath, modelpath);
}

/************************************************************************
 * 函数名：get_class_score
 * 功能描述: 模型推理结果后处理，得到最大得分的类别及概率
 * 输入参数：
	@cls_scores：模型推理输出结果
	@score：得分最大类别的概率
	@classes：最大得分的类别
 * 返回值：void
 ************************************************************************/
static void get_class_score(const std::vector<float> &cls_scores, float &score, int &classes) {
    double sum = 0.f;
    auto *softmax = (double *) malloc(sizeof(double) * NUM_CLASS);
    int size = cls_scores.size();
    std::vector<std::pair<float, int> > vec;
    vec.resize(size);
    for (int i = 0; i < size; i++) {
        vec[i] = std::make_pair(cls_scores[i], i);
    }

    std::partial_sort(vec.begin(), vec.begin() + NUM_CLASS, vec.end(),
                      std::greater<std::pair<float, int> >());

    classes = vec[0].second;
    for (int i = 0; i < NUM_CLASS; i++) {
        softmax[i] = exp(vec[i].first);
        sum += softmax[i];
    }

    score = softmax[0] / sum;
}


void guess(float data[], int &type, float &score) {
    // 数据预处理
    data_prep->extract_mfcc_feature(data, ou_d);

    // 模型推理
    std::vector<float> cls_scores;
    kws_audio->run_kwssudio_model(ou_d, cls_scores);

    // 模型输出结果后处理，得到得分最高的类别（classes）及概率值（score）
    get_class_score(cls_scores, score, type);

//    LOGD("score:%f , classes: %d", score, classes);
}

void release() {
    delete data_prep;
    delete kws_audio;
}
