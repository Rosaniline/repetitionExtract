//
//  symmetryDetection.h
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/16.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#ifndef __repetitionExtract__symmetryDetection__
#define __repetitionExtract__symmetryDetection__

#include <iostream>
#include "RosaniTools.h"


class symmetryDetection {

    
public:
    
    symmetryDetection();
    ~symmetryDetection();
    
    void detect(const Mat& img);
    
    
private:
    
    Mat img;
    
    
    // return the min radius that contains whole object in the input image
    int getMinContainingRadius (const Mat& img);
    
    // reture the centroid of input image using firsr order image moment
    Point getCentroid (const Mat& img);
    
    // return the vector contains all circle points of given redius w.r.t the given centroid
    vector<Point> getCirclePoint (const Point& centroid, int radius);
    
    
};


#endif /* defined(__repetitionExtract__symmetryDetection__) */
