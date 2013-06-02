//
//  sliceSynthesis.h
//  repetitionExtract
//
//  Created by Rosani Lin on 13/5/28.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __repetitionExtract__sliceSynthesis__
#define __repetitionExtract__sliceSynthesis__

#include <iostream>
#include "Rosaniline.h"
#include "tattingPattern.h"

//#define PI 3.14159265359


class sliceSynthesis {
    
public:
    
    sliceSynthesis();
    ~sliceSynthesis();
    
    
    Mat synthesis (const tattingPattern& pattern);
    
    
private:
    
    Mat circularGaussianRG (double ratio, double sigma, const Point& centroid, Size canvas_size, int max_radius);
    
    Mat sliceGaussianRG (int slice_num, double sigma, const Point& centroid, Size canvas_size, int max_radius, double sym_angle);
    
};




#endif /* defined(__repetitionExtract__sliceSynthesis__) */
