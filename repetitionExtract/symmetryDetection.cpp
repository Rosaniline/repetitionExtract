//
//  symmetryDetection.cpp
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/16.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "symmetryDetection.h"

symmetryDetection::symmetryDetection() {
    
}


symmetryDetection::~symmetryDetection() {
    
}


void symmetryDetection::detect(const cv::Mat &img) {
    
    if ( img.type() != CV_8UC1 ) {
        cvtColor(img, img, CV_BGR2GRAY);
    }
    
    int minRadius = getMinContainingRadius(img);
    
    cout<<minRadius;
    

    
    
}


int symmetryDetection::getMinContainingRadius (const cv::Mat &img) {
    
    Mat plot = img.clone();
    Point centroid = getCentroid(img);
    bool touch = false;
    int min_radius = 0;
    
    
    for (int r = 50; r < img.rows/2; r ++) {
        
        vector<Point>circle_p = getCirclePoint(centroid, r);
        touch = false;
        
        for (int i = 0; i < circle_p.size(); i ++) {
            
            if ( plot.at<uchar>(circle_p[i]) != 0 ) {
                touch = true;
                break;
            }
            
        }
        
        if ( !touch ) {
            
            min_radius = r;
            break;
        }
    }
    
    return min_radius;
    
}


Point symmetryDetection::getCentroid(const cv::Mat &img) {

    Moments mo = moments(img);
    
    return Point(mo.m10/mo.m00, mo.m01/mo.m00);
    
}


vector<Point> symmetryDetection::getCirclePoint(const Point &centroid, int radius) {
    
    vector<Point> growing_circle;
    int boundary = radius/sqrt(2);
    
    for (int i = -boundary; i <= boundary; i ++) {
        
        int j = sqrt(radius*radius - i*i);
        growing_circle.push_back(centroid + Point(i, -j));
        
    }
    
    for (int j = -boundary ; j <= boundary; j ++) {
        
        int i = sqrt(radius*radius - j*j);
        growing_circle.push_back(centroid + Point(i, j));
        
    }
    
    for (int i = boundary; i >= -boundary; i --) {
        
        int j = sqrt(radius*radius - i*i);
        growing_circle.push_back(centroid + Point(i, j));
        
    }
    
    for (int j = boundary ; j >= -boundary; j --) {
        
        int i = sqrt(radius*radius - j*j);
        growing_circle.push_back(centroid + Point(-i, j));
        
    }
    
    
    return growing_circle;
    
}







