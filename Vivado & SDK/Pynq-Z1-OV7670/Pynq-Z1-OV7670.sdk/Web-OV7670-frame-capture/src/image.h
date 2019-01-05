#ifndef SRC_IMAGE_H_
#define SRC_IMAGE_H_
#include "xbasic_types.h"
#include "jpg.h"
#include "vdma_single_step.h"

extern RAW_YCbCr_IMAGE image;

void transformYUV422ImageToYUV();

#endif /* SRC_IMAGE_H_ */
