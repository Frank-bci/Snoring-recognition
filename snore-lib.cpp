#include <string>
#include <android/log.h>
#include "ncnn/tensorflow.h"
#include "sleep/pch.h"
#include "sleep/sleepinfo.h"
#include <stdint.h>

#include <android/log.h>
#define TAG "llcqh"
#define log(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define DART_API extern "C" __attribute__((visibility("default"))) __attribute__((used))

//======= 鼾声监测 =======
#define AUDIO_LEN 56000            // 语音段长度 16K*3.5
float input[AUDIO_LEN] = {0.f};

//鼾声监测 - Result
typedef struct {
    int type;
    float score;
} Result;

//分钟 - 睡眠结果
typedef struct {
    float DB_Approximate_entropy;/*计算信号的近似熵*/
    float standard_deviation;/*呼吸频率标准差*/
    float All_night_pr;/*整夜心率标准差*/
    float noise;/*数据不为空的DB数据*/
    float DB_len_data_mean;
    float flow_len_data_mean;
    float SPO2_len_data_mean;
    float Spo2_data2_mean;
    float Every_frame_moves;

    int32_t Snore_frequency; /*鼾声次数*/
    float Snore_duration; /*鼾声时长*/
    float Sleep_talking_duration; /*梦话时长*/
    int32_t Sleep_talk_frequency; /*梦话次数*/

    //计算加速度参数
    float Combined_acceleration_10_minutes[16000];
    int32_t Combined_acceleration_10_minutes_len;
    float Combined_acceleration_10_minutes_mean;
    int32_t Combined_acceleration_10_minutes_flag;
    float Every_frame_moves_First_10_data[10];
} MinuteResult;

//总 - 睡眠结果
typedef struct {
    float result_array[HOW_MANY_MINUTES];    /* 睡眠分期结果数组（需要在此结构中添加） */
    int32_t Sleep_result_len;/*睡眠分期数据长度*/
    int32_t Invalid_data_flag;/*睡眠分期有效标志1：有效；0：无效*/
    int32_t score;/*睡眠评分*/

    int32_t Snore_frequency; /*鼾声次数*/
    float Snore_duration; /*鼾声时长*/
    float Sleep_talking_duration; /*梦话时长*/
    int32_t Sleep_talk_frequency; /*梦话次数*/
} GlobalResult;

//====== 鼾声监测 ======
DART_API void init(const char *proto_path, const char *model_path, const int number) {
    init_model(proto_path, model_path, number);
}

DART_API Result *process(const int16_t *byteArray) {
    for (int i = 0; i < AUDIO_LEN; ++i) {
        input[i] = (float) byteArray[i];
    }
    auto *result = (Result *) malloc(sizeof(Result));
    guess(input, result->type, result->score);
    return result;
}

DART_API void destroy() {
    release();
}

//====== 睡眠监测 ======
//计算每分结果
DART_API MinuteResult *
getMinResult(IntermediateParameter_Saving_T2 *output,
             uint32_t *result, int32_t data_len,
             int32_t *types, int32_t *durations, int32_t snore_data_len,
             float *Combined_acceleration_10_minutes, int32_t Combined_acceleration_10_minutes_len,
             float Combined_acceleration_10_minutes_mean,
             int32_t Combined_acceleration_10_minutes_flag) {

    //进入方法 - 打印
//    log("getMinResult=> start");
//    log("snore_data_len：%d", snore_data_len);
//    log("Combined_acceleration_10_minutes_mean：%f", Combined_acceleration_10_minutes_mean);
//    log("Combined_acceleration_10_minutes_flag：%d", Combined_acceleration_10_minutes_flag);
//    log("Combined_acceleration_10_minutes_len：%d", Combined_acceleration_10_minutes_len);

    int32_t Combined_acceleration_1_minutes_len;

    //计算结果
    Data_processing_per_minute(output, result, data_len, types, durations, snore_data_len,
                               Combined_acceleration_10_minutes_mean,
                               Combined_acceleration_10_minutes_flag,
                               &Combined_acceleration_1_minutes_len);
//    log("getMinResult=> end");

    // 返回结果值
    auto minOutput = (MinuteResult *) malloc(sizeof(MinuteResult));

    //结果数据
    minOutput->DB_Approximate_entropy = output->DB_Approximate_entropy;
    minOutput->standard_deviation = output->standard_deviation;
    minOutput->All_night_pr = output->All_night_pr;
    minOutput->noise = output->noise;
    minOutput->DB_len_data_mean = output->DB_len_data_mean;
    minOutput->flow_len_data_mean = output->flow_len_data_mean;
    minOutput->SPO2_len_data_mean = output->SPO2_len_data_mean;
    minOutput->Spo2_data2_mean = output->Spo2_data2_mean;
    minOutput->Every_frame_moves = output->Every_frame_moves;
//    log("DB_Approximate_entropy：%f", minOutput->DB_Approximate_entropy);
//    log("standard_deviation：%f", minOutput->standard_deviation);
//    log("All_night_pr：%f", minOutput->All_night_pr);
//    log("noise：%f", minOutput->noise);
//    log("DB_len_data_mean：%f", minOutput->DB_len_data_mean);
//    log("flow_len_data_mean：%f", minOutput->flow_len_data_mean);
//    log("SPO2_len_data_mean：%f", minOutput->SPO2_len_data_mean);
//    log("Spo2_data2_mean：%f", minOutput->Spo2_data2_mean);
//    log("Every_frame_moves：%f", minOutput->Every_frame_moves);

    //鼾声数据
    minOutput->Snore_frequency = output->Snore_frequency;
    minOutput->Snore_duration = output->Snore_duration;
    minOutput->Sleep_talking_duration = output->Sleep_talking_duration;
    minOutput->Sleep_talk_frequency = output->Sleep_talk_frequency;
//    log("Snore_frequency：%d", minOutput->Snore_frequency);
//    log("Snore_duration：%f", minOutput->Snore_duration);
//    log("Sleep_talking_duration：%f", minOutput->Sleep_talking_duration);
//    log("Sleep_talk_frequency：%d", minOutput->Sleep_talk_frequency);

    //处理前10分钟数据
    minOutput->Combined_acceleration_10_minutes_mean = output->Combined_acceleration_10_minutes_mean;
    minOutput->Combined_acceleration_10_minutes_flag = output->Combined_acceleration_10_minutes_flag;
//    log("10_minutes_mean：%f", minOutput->Combined_acceleration_10_minutes_mean);
//    log("10_minutes_flag：%d", minOutput->Combined_acceleration_10_minutes_flag);
//    log("Combined_acceleration_1_minutes_len：%d", Combined_acceleration_1_minutes_len);

    if (Combined_acceleration_10_minutes_flag == 0) {
        //添加计算结果后数据
        for (int32_t i = 0; i < Combined_acceleration_1_minutes_len; i++) {
            Combined_acceleration_10_minutes[Combined_acceleration_10_minutes_len +i] = output->Combined_acceleration_10_minutes[i];
        }
        //赋值给 - minOutput
        minOutput->Combined_acceleration_10_minutes_len = Combined_acceleration_1_minutes_len + Combined_acceleration_10_minutes_len;
//        log("Combined_acceleration_10_minutes_len：%d",minOutput->Combined_acceleration_10_minutes_len);
        for (int i = 0; i < minOutput->Combined_acceleration_10_minutes_len; i++) {
            minOutput->Combined_acceleration_10_minutes[i] = Combined_acceleration_10_minutes[i];     // 这里可以使用内存拷贝或者手动赋值
        }

//        log("Combined_acceleration_10_minutes_len end");
        float Every_frame_moves_First_10_data[10] = {0};
        //计算数据
        First_10_minutes_of_data_processing(minOutput->Combined_acceleration_10_minutes,
                                            minOutput->Combined_acceleration_10_minutes_len,
                                            &(minOutput->Combined_acceleration_10_minutes_mean),
                                            Every_frame_moves_First_10_data,
                                            &(minOutput->Combined_acceleration_10_minutes_flag));

//        log("minOutput->Combined_acceleration_10_minutes_flag：%d",
//            minOutput->Combined_acceleration_10_minutes_flag);
        //获取到10分钟数据
        if (minOutput->Combined_acceleration_10_minutes_flag == 1) {
            for (int i = 0; i < 10; i++) {
                minOutput->Every_frame_moves_First_10_data[i] = Every_frame_moves_First_10_data[i];
            }
        }
//        log("minOutput->Every_frame_moves_First_10_data end");
    } else {
        //如果已经获取到值
        minOutput->Combined_acceleration_10_minutes_mean = Combined_acceleration_10_minutes_mean;
        minOutput->Combined_acceleration_10_minutes_flag = Combined_acceleration_10_minutes_flag;
    }

//    log("Combined_acceleration_10_minutes_mean2：%f",
//        minOutput->Combined_acceleration_10_minutes_mean);
//    log("Combined_acceleration_10_minutes_flag3：%d",
//        minOutput->Combined_acceleration_10_minutes_flag);
    return minOutput;
}

//计算整体结果
DART_API GlobalResult *
getGlobalResult(IntermediateParameter_Saving_T *savingMin, SleepReault *output) {

//    log("getGlobalResult=> start");
//    log("How_many_minutes：%d", savingMin->How_many_minutes);
//    log("Snore_frequency：%d", savingMin->Snore_frequency);
//    log("Snore_duration：%f", savingMin->Snore_duration);
//    log("Sleep_talking_duration：%f", savingMin->Sleep_talking_duration);
//    log("Sleep_talk_frequency：%d", savingMin->Sleep_talk_frequency);

    ALGO_result(savingMin, output);    //计算总结果
//    log("getGlobalResult=> end");

    // 返回结果值
    auto globalOutput = (GlobalResult *) malloc(sizeof(GlobalResult));
    //结果
    globalOutput->Sleep_result_len = output->Sleep_result_len;//-1
    globalOutput->Invalid_data_flag = output->Invalid_data_flag;
    globalOutput->score = output->score;

//    log("Sleep_result_len：%d", globalOutput->Sleep_result_len);
//    log("Invalid_data_flag：%d", globalOutput->Invalid_data_flag);
//    log("score：%d", globalOutput->score);

    //鼾声数据
    globalOutput->Snore_frequency = output->Snore_frequency;
    globalOutput->Snore_duration = output->Snore_duration;
    globalOutput->Sleep_talking_duration = output->Sleep_talking_duration;
    globalOutput->Sleep_talk_frequency = output->Sleep_talk_frequency;

//    log("Snore_frequency：%d", globalOutput->Snore_frequency);
//    log("Snore_duration：%f", globalOutput->Snore_duration);
//    log("Sleep_talking_duration：%f", globalOutput->Sleep_talking_duration);
//    log("Sleep_talk_frequency：%d", globalOutput->Sleep_talk_frequency);

    //结果列表数据
    for (int i = 0; i <= output->Sleep_result_len; i++) {
        globalOutput->result_array[i] = output->Sleep_result[i];     // 这里可以使用内存拷贝或者手动赋值
    }
    return globalOutput;
}



