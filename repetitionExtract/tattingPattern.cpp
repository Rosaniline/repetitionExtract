//
//  tattingPattern.cpp
//  repetitionExtract
//
//  Created by Rosani Lin on 13/5/7.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "tattingPattern.h"

tattingPattern::tattingPattern(const Mat& img) {
    
    preprocess(img);
    
    centroid = Point(296, 301);
//    setRotMinCentroid();
    
    detectSliceNum sliceNum_detector = detectSliceNum();
    slice_num = 10;//sliceNum_detector.extractSliceNum(pattern, centroid);

    symmetryDetection sym_detector = symmetryDetection();
    sym_angle = 92;//sym_detector.detectSymmetryAngle(pattern, centroid);

    
    extractSlices();
    
    sliceSelection();
    
    max_radius = getMaxContainingRadius();
    
    
//    Mat plot = pattern.clone();
//
//    for (int i = 0; i < slice_num/2; i ++) {
//
//        infLine(plot, centroid, tan((i*(360/slice_num) + sym_angle)/180*PI));
//        
//    }
//    showMat(plot);
    
}


tattingPattern::~tattingPattern() {
    
    pattern.release();
    centroid.~Point_();
    
    vector<Mat>().swap(slices);
    vector<Mat>().swap(slices_mask);
    
}


void tattingPattern::preprocess(const cv::Mat &img) {
    
    pattern = img.clone();
    
    if ( pattern.type() != CV_8UC1 ) {
        cvtColor(pattern, pattern, CV_BGR2GRAY);
    }
    
    threshold(pattern, pattern, 0, 255, THRESH_OTSU);
    
    pattern.convertTo(pattern, CV_64FC1);
    pattern /= 255.0;
    
}


void tattingPattern::setRotMinCentroid() {
    
    vector<Mat> pattern_pyr;
    buildPyramid(pattern, pattern_pyr, ROT_MIN_PYRAMID_LEVEL);
    
    reverse(pattern_pyr.begin(), pattern_pyr.end());
    
    Point centroid_candi(0);
    
    
    for (int level = 0; level <= 5; level ++) {
        
        if ( level == 0 ) {
            
            Moments mo = moments(pattern_pyr[level], true);
            centroid_candi = Point((int)(mo.m10/mo.m00), (int)(mo.m01/mo.m00));
        }
        
        else {
            
            centroid_candi = centroid*2;
        }
        
        
        double min_error = INFINITY;
        
        #pragma omp parallel for
        for (int i = -ROT_MIN_SEARCH_WIDTH; i <= ROT_MIN_SEARCH_WIDTH; i ++) {
            for (int j = -ROT_MIN_SEARCH_WIDTH; j <= ROT_MIN_SEARCH_WIDTH; j ++) {
                
                vector<double> diff;
                double local_error = 0.0;
                
                for (double ang = 0; ang < 360.0; ang += 1) {
                    
                    Mat rotate_mat = getRotationMatrix2D(centroid_candi + Point(j, i), ang, 1.0);
                    
                    Mat rotated_img = Mat::zeros(pattern_pyr[level].size(), CV_8UC1);
                    
                    warpAffine(pattern_pyr[level], rotated_img, rotate_mat, pattern_pyr[level].size());
                
                    Mat rot_diff = Mat::zeros(pattern_pyr[level].size(), CV_8UC1);
                    absdiff(pattern_pyr[level], rotated_img, rot_diff);
                    
                    diff.push_back(sum(rot_diff)[0]);
                    
                }
                
                sort(diff.begin(), diff.end());
                
                local_error = accumulate(diff.begin(), diff.begin() + 10, 0);
                
                vector<double>().swap(diff);
                
                
                #pragma omp critical
                {
                    if ( min_error > local_error ) {
                        
                        centroid = centroid_candi + Point(j, i);
                        min_error = local_error;
                    }
                }
            }
        }
        
        
        //        Mat plot = img_pyr[level].clone();
        //        plot.at<uchar>(centroid_candi) = 255;
        //        showMat(plot, "candi", 1);
        //
        //        plot = img_pyr[level].clone();
        //        plot.at<uchar>(centroid) = 255;
        //        showMat(plot);
    }
    
    vector<Mat>().swap(pattern_pyr);
    
    
}


void tattingPattern::extractSlices() {
    
    
    double ang_one = sym_angle, ang_two = ang_one + 360.0/slice_num;
    
    for (int s = 0; s < slice_num/2; s ++) {
        
        
        Mat mask_one = Mat::zeros(pattern.size(), CV_64FC1), mask_two = Mat::zeros(pattern.size(), CV_64FC1);

        for (int i = 0; i < mask_one.rows; i ++) {
            for (int j = 0; j < mask_one.cols; j ++) {
                
                int map_i = i - centroid.y, map_j = j - centroid.x, reverse_sign = 0;
                
                double radian_one = ang_one*PI/180.0, radian_two = ang_two*PI/180.0;
                
                // to compensate the sign reversion of tan at 90 degree
                reverse_sign = (int)ang_two%180 > 90 && (int)ang_one%180 < 90 ? -1 : 1;
                
                
                Mat mask_local = map_j*tan(radian_one - PI/2) + map_i < 0 ? mask_one : mask_two;
                
                if ( reverse_sign*(map_j*tan(radian_one) + map_i)*(map_j*tan(radian_two) + map_i ) < 0 ) {
                    
                    mask_local.at<double>(i, j) = 1.0;
                    
                }

            }
        }
        
        Mat slice_one = Mat::zeros(pattern.size(), CV_64FC1), slice_two = Mat::zeros(pattern.size(), CV_64FC1);
        
        bitwise_and(pattern, mask_one, slice_one);
        bitwise_and(pattern, mask_two, slice_two);
        
        slices_mask.push_back(mask_one);
        slices_mask.push_back(mask_two);
        
        slices.push_back(slice_one);
        slices.push_back(slice_two);
        
        
        ang_one += 360.0/slice_num;
        ang_two += 360.0/slice_num;
    }
    
    
    

    
//    for (int i = 0; i < slices.size(); i ++) {
//        stringstream file;
//        
//        Mat temp = slices_mask[i].clone();
//        temp *= 255;
//        temp.convertTo(temp, CV_8UC3);
//        
//        file << "/Users/xup6qup3/Dropbox/code/Dev.temp/repetitionExtract/repetitionExtract/slice_mask/mask_" << i <<".bmp";
//        
//        imwrite(file.str().c_str(), temp);
//    }
}


void tattingPattern::sliceSelection() {
    
    
}



int tattingPattern::getMaxContainingRadius () {
    
    int min_radius = 0;
    
    for (int r = INNER_MOST_RADIUS; r < MIN(pattern.rows, pattern.cols)/2; r ++) {
        
        vector<Point>circle_p = getCirclePoints(centroid, r);
        
        for (int i = 0; i < circle_p.size(); i ++) {
            
            if ( pattern.at<double>(circle_p[i]) ) {
                
                min_radius = r;
                break;
            }
            
        }
        
        vector<Point>().swap(circle_p);
    }
    
    
    return min_radius;
}









