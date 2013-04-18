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


double symmetryDetection::detectSymmetryAngle (const cv::Mat &img) {
    
    if ( img.type() != CV_8UC1 ) {
        cvtColor(img, img, CV_BGR2GRAY);
    }
    
    
    return getSymmetricAngle(img);
    
//    angle = (angle/180.0)*PI;
//    
//    Mat temp2 = img.clone();
//    
//    cout<<endl<<tan(angle);
//    
//    
//    infLine(temp2, getCentroid(img), tan(angle));
//    
    
    

    

}


int symmetryDetection::getMinContainingRadius (const cv::Mat &img) {
    
    Mat plot = img.clone();
    Point centroid = getCentroid(img);
    bool touch = false;
    int min_radius = 0;
    
    
    for (int r = INNER_MOST_RADIUS; r < img.rows/2; r ++) {
        
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


double symmetryDetection::getSymmetricAngle (const cv::Mat &img) {
    
    

    
    
    vector<int> angles;
    
    for (int radius = CONCENTRIC_MIN_RADIUS; radius < getMinContainingRadius(img); radius += CONCENTRIC_SAMPLE_STEP) {
        
        vector<Point> circle_pts = getCirclePoint(getCentroid(img), radius);

        Mat circle_pt_mat = Mat((int)circle_pts.size(), 1, CV_8UC1);
        Mat convolved_mat = Mat((int)circle_pts.size(), 1, CV_64FC1);

        
        for (int i = 0; i < circle_pts.size(); i ++) {
            
            circle_pt_mat.at<uchar>(i, 0) = img.at<uchar>(circle_pts[i]);
            
        }
        
        
        for (int i = 0; i < circle_pts.size(); i ++) {
            
            convolved_mat.at<double>(i, 0) = convolveMat(circle_pt_mat);
            shiftMatElement(circle_pt_mat);
            
        }
        
        double max = 0.0;
        int idx = 0;
        
        for (int i = 0; i < convolved_mat.rows; i ++) {
            
            if ( max < convolved_mat.at<double>(i, 0) ) {
                
                max = convolved_mat.at<double>(i, 0);
                idx = i;
            }
        }
        
        int angle = ceil(135 - idx*(360.0 / convolved_mat.rows));
        angle = angle > 0 ? angle : 360 + angle; 
        
        angles.push_back(angle);

        
        /**/
        
        
//        cout<<angle<<endl;
//        angle = (angle/180.0)*PI;
//        
//        Mat temp = img.clone();
//        
//        infLine(temp, getCentroid(img), tan(angle));
//        circle(temp, getCentroid(img), radius, 255);
//        
////        cout<<angle<<endl;
//        showMat(temp);
        
        
        
        
    }
    
    
    
//    cout<<" one step = "<<(360.0 / convolved_mat.rows)<<endl;
//    cout<<" return idx = "<<idx<<"  angle = "<<135 - idx*(360.0 / convolved_mat.rows)<<endl;
//    return 135 - idx*(360.0 / convolved_mat.rows);


    return mode(angles);
    
    

}




double symmetryDetection::convolveMat(const cv::Mat &mat) {
    
    double value = 0.0;
    
    for (int i = 0; i < mat.rows; i ++) {
        
        value += mat.at<uchar>(i, 0)*mat.at<uchar>(mat.rows - 1 - i, 0);
    }
    
    return value;
}


void symmetryDetection::shiftMatElement(cv::Mat &mat) {
    
    uchar temp = mat.at<uchar>(0, 0);
    
    for (int i = 1; i < mat.rows; i ++) {
        
        mat.at<uchar>(i - 1, 0) = mat.at<uchar>(i, 0);
    }
    
    mat.at<uchar>(mat.rows - 1) = temp;
    
}


int symmetryDetection::mode(vector<int> array) {
    
    int *count = new int[357];
    
    for (int i = 0; i < 357; i ++) {
        count[i] = 0;
    }
    
    for (int i = 0; i < array.size(); i ++) {
        
        count[array[i]] ++;
        
    }
    
    int max = 0, max_idx = -1;
    
    for (int i = 0; i < 357; i ++) {
        
//        if ( count[i] != 0 ) {
//            cout<<i<<" : "<<count[i]<<endl;
//        }
        
        if ( count[i] > max ) {
            
            max = count[i];
            max_idx = i;
        }
        
    }
    
    
    delete [] count;
    
    
    return max_idx;
}









