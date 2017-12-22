#pragma once

#if defined WITH_CAFFE || WITH_FREDNET
#	define HAS_DETECT
#endif

#include "Detect_Caffe.h"
#include "Detect_Frednet.h"