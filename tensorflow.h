//
// Created by HP on 2021/7/30.
//

#ifndef SNORETENSORFLOW_TENSORFLOW_H
#define SNORETENSORFLOW_TENSORFLOW_H


#endif //SNORETENSORFLOW_TENSORFLOW_H

void init_model(const char *protopath, const char *modelpath, const int number);

void guess(float data[], int &type, float &score);

void release();

