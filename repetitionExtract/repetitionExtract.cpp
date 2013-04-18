//
//  repetitionExtract.cpp
//  repetitionExtract
//
//  Created by Rosani Lin on 13/4/15.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "repetitionExtract.h"


repetitionExtract::repetitionExtract () {
    
}

repetitionExtract::~repetitionExtract () {
    
}


void repetitionExtract::extract (const string &pattern_path) {
    
    input_pattern = imread(pattern_path.c_str());
    cvtColor(input_pattern, pattern, CV_BGR2GRAY);
    
    resizeMat(pattern, 0.4);
    
    
    findBarycentre();
    
    
    int slices = extractNumSlice();
    
    
    symmetryDetection symDetect = symmetryDetection();
    double sym_angle = symDetect.detectSymmetryAngle(pattern);

    
    
    for (int i = 0; i < slices/2; i ++) {

        infLine(pattern, centroid, tan((i*(360/slices) + sym_angle)/180*PI));
    }
    
    showMat(pattern);
    
    
    
//    cout<<sym_angle;
    
    
    
}


void repetitionExtract::findBarycentre() {
    
    
    Moments mo = moments(pattern);
    
    centroid = Point(mo.m10/mo.m00, mo.m01/mo.m00);

}





int repetitionExtract::extractNumSlice() {
    
    
    vector<double> opt_diff;
    double opt_diff_val = INFINITY;
    Point opt_centroid = centroid;
    
    // Centroid Refinement
    for (int i = -REFINEMENT_WIDTH; i <= REFINEMENT_WIDTH; i ++) {
        for (int j = -REFINEMENT_WIDTH; j <= REFINEMENT_WIDTH; j ++) {
            
            vector<double> local_diff = rotatePattern(pattern, centroid + Point(i, j));
            
            
            double local_diff_val = 0.0;
            accumulate(local_diff.begin(), local_diff.end(), local_diff_val);
            
            
            if ( opt_diff_val > local_diff_val ) {
                
                opt_diff_val = local_diff_val;
                opt_diff.swap(local_diff);
                opt_centroid = centroid + Point(i, j);
            }
        }
    }
    
    
    
    double max = *max_element(opt_diff.begin(), opt_diff.end()), min = *min_element(opt_diff.begin(), opt_diff.end());
    
    for_each(opt_diff.begin(), opt_diff.end(), [&](double &x) {

        x = (x - min)*(255.0 / (max - min));

    });
    
    Mat diff_map = Mat((int)opt_diff.size(), 200, CV_8UC1);
    

    for (int i = 0; i < diff_map.rows; i ++) {

        int k = opt_diff[i];


        for (int j = 0; j < diff_map.cols; j ++) {

            diff_map.at<uchar>(i, j) = k;
        }
    }
    
//    diff_map = diff_map.t();
//    imwrite("/Users/xup6qup3/Google Drive/code/Dev.temp/repetitionExtract/repetitionExtract/tl_2_diff.jpg", diff_map);
//    showMat(diff_map);
    
    threshold(diff_map, diff_map, 0, 255, THRESH_OTSU);
    
//    imwrite("/Users/xup6qup3/Google Drive/code/Dev.temp/repetitionExtract/repetitionExtract/tl_2_diff_th.jpg", diff_map);
    
    int slices = 0;
    for (int i = 1; i < diff_map.rows; i ++) {
        
        if ( diff_map.at<uchar>(i - 1, 0) < diff_map.at<uchar>(i, 0) ) {
            slices ++;
        }
    }
    
//    cout<<slices;
//    showMat(diff_map);

    
    return slices;
}


vector<double> repetitionExtract::rotatePattern(cv::Mat &pattern, Point centroid) {
    
    vector<double> ang_diff;
    
    for (double ang = 1; ang < 360.0; ang += 0.5) {
        
        Mat rotate_mat = getRotationMatrix2D(Point(centroid), ang, 1.0);
        
        Mat rotated_pattern = pattern.clone().setTo(0);
        
        warpAffine(pattern, rotated_pattern, rotate_mat, pattern.size());
        
        
        Mat diff = pattern.clone().setTo(0);
        absdiff(pattern, rotated_pattern, diff);
        
        ang_diff.push_back(sum(diff)[0]);
        
    }
    
    return ang_diff;
    
    
    
}


void repetitionExtract::plotSlice(int slice_num) {
    
    for (double i = 0.0; i < 5; i += 0.01) {
        infLine(input_pattern, centroid, -i);
    }
    
    
    
    
    
}







