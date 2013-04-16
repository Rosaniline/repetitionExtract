//
//  repetitionExtract.h
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/15.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __repetitionExtract__repetitionExtract__
#define __repetitionExtract__repetitionExtract__

#include <iostream>
#include "RosaniTools.h"
#include "symmetryDetection.h"


class repetitionExtract {
    
    
public:
    
    repetitionExtract();
    ~repetitionExtract();
    
    void extract(const string& pattern_path);
    
private:
    
    
    void findBarycentre ();
    
    int extractNumSlice ();
    
    
    
    vector<double> rotatePattern (Mat& pattern , Point centroid);
    
    void plotSlice (int slice_num);
    
    
    
    
    Mat input_pattern, pattern;
    Point centroid;
    
    const static int REFINEMENT_WIDTH       =  0;
    
    
};

#endif /* defined(__repetitionExtract__repetitionExtract__) */
