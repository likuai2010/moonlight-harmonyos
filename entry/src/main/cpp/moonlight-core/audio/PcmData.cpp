//
// Created by Gu,Xiuzhong on 2021/4/13.
//

#include "PcmData.h"
#include "malloc.h"
#include "string.h"
PcmData::PcmData(char *data, int size) {
    this->data = (char *) malloc(size);
    memcpy(this->data, data, size);

    this->size = size;
}

PcmData::~PcmData() {

}

int PcmData::getSize() {
    return size;
}

char *PcmData::getData() {
    return data;
}

