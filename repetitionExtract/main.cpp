//
//  main.cpp
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/15.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include <iostream>
#include "Rosaniline.h"

#include "tattingPattern.h"
#include "sliceSynthesis.h"

#include "PatchMatch.h"



int main(int argc, const char * argv[])
{
    r_time time;
    time.start();

    Mat img = imread("/Users/xup6qup3/Dropbox/code/Dev.temp/repetitionExtract/repetitionExtract/tl6.jpg");
    
    tattingPattern tatting = tattingPattern(img);
    
    sliceSynthesis ssynthesis = sliceSynthesis();
    ssynthesis.synthesis(tatting);
    
    

    
//    Mat temp_src = tatting.pattern.clone();
//    temp_src *= 255;
//    temp_src.convertTo(temp_src, CV_8UC1);
//    cvtColor(temp_src, temp_src, CV_GRAY2BGR);
//    resizeMat(temp_src, 0.6);
//    
//    Mat temp_pattern = tatting.pattern.clone();
//    temp_pattern *= 255;
//    temp_pattern.convertTo(temp_pattern, CV_8UC1);
//    cvtColor(temp_pattern, temp_pattern, CV_GRAY2BGR);
//    resizeMat(temp_pattern, 0.5);
//    
//    Mat temp_mask = Mat::zeros(temp_pattern.size(), CV_64FC1);
//    RosMat<Point> temp_prior = RosMat<Point>(temp_pattern.size());
//    
//    
//    for (int i = 0; i < 100; i ++) {
//        for (int j = 0; j < 150; j ++) {
//            temp_mask.at<double>(i, j) = 1.0;
//        }
//    }
//    
//    
//    
//    PatchMatch patch_match = PatchMatch(7, 7);
////    patch_match.matchImage(temp_src, temp_pattern);
//    patch_match.matchPriorWithMask(temp_src, temp_pattern, Mat::ones(temp_src.size(), CV_64FC1), temp_mask, temp_prior, true);
    
    
    
    time.end();
    time.printTime("over all:");
    


    return 0;
}

 