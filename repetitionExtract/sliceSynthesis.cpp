//
//  sliceSynthesis.cpp
//  repetitionExtract
//
//  Created by Rosani Lin on 13/5/28.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "sliceSynthesis.h"


sliceSynthesis::sliceSynthesis() {}

sliceSynthesis::~sliceSynthesis() {}


Mat sliceSynthesis::synthesis(const tattingPattern &pattern) {
    
    Mat slice_seed = pattern.slices[4].clone();
    Mat synthesized = pattern.slices[4].clone();


    for (int i = 0; i < pattern.slice_num; i ++) {
        
        Mat rot_mat = getRotationMatrix2D(pattern.centroid, (360.0/pattern.slice_num)*i, 1.0);
        Mat temp = Mat::zeros(synthesized.size(), synthesized.type());
        warpAffine(slice_seed, temp, rot_mat, slice_seed.size(), INTER_CUBIC);
        
        synthesized += temp;
        
    }
    
    Mat one = circularGaussianRG(0.3, 20.0, pattern.centroid, slice_seed.size(), pattern.max_radius);
    
    Mat two = circularGaussianRG(0.7, 30.0, pattern.centroid, slice_seed.size(), pattern.max_radius);
    
    one = one + two;
    
    showMat(sliceGaussianRG(pattern.slice_num, 30.0, pattern.centroid, pattern.pattern.size(), pattern.max_radius, pattern.sym_angle), 0);

    
    
    
    return synthesized;
}


Mat sliceSynthesis::circularGaussianRG (double ratio, double sigma, const Point& centroid, Size canvas_size, int max_radius) {
    
    Mat canvas = Mat::zeros(canvas_size, CV_64FC1);
    
    
    int mu = (int)(ratio*MIN(canvas_size.height, canvas_size.width)/2);
    
    double denominator_out = 1.0/(sigma*sqrt(2*PI)), denominator_in = (2*pow(sigma, 2.0));
    
    
    for (int i = 0; i < canvas.rows; i ++) {
        for (int j = 0; j < canvas.cols; j ++) {
            
            int r = (int)sqrt(pow(i - centroid.y, 2.0) + pow(j - centroid.x, 2.0));
            
            if ( r <= max_radius ) {
                
                canvas.at<double>(i, j) = denominator_out*exp(-pow(r - mu, 2.0)/denominator_in);
            }
            
        }
    }
    
    
    
    double max = 0.0;
    
    minMaxIdx(canvas, NULL, &max);
    
    canvas = canvas/max;
    
    
    
    
//    showMat(canvas, "canvas", 0);
    
    return canvas;
}


Mat sliceSynthesis::sliceGaussianRG (int slice_num, double sigma, const Point &centroid, Size canvas_size, int max_radius, double sym_angle) {
    
    Mat canvas = Mat::zeros(canvas_size, CV_64FC1);

    // Create a Gaussian distribution sample according to the max containing radius
    Mat gaussian_sample = Mat::zeros((int)(max_radius*(PI/slice_num)), 1, CV_64FC1);
    
    
    double denominator_in = 2.0*pow(sigma, 2.0);
    for (int i = 0; i < gaussian_sample.rows; i ++) {
        for (int j = 0; j < gaussian_sample.cols; j ++) {
            
            gaussian_sample.at<double>(i, j) = exp(-pow(i, 2.0)/denominator_in);
            
        }
    }
    
    double sita_seed = (sym_angle/180.0)*PI;
    vector<double> divide_sita;
    for (int i = 0; i < slice_num; i ++) {
        divide_sita.push_back(sita_seed);
        
        sita_seed += 2.0*PI/slice_num;
        
        if ( sita_seed > 2.0*PI ) {
            sita_seed -= 2.0*PI;
        }
        
    }
    

    
    for (int i = 0; i < canvas.rows; i ++) {
        for (int j = 0; j < canvas.cols; j ++) {
            
            double r = sqrt( pow(i - centroid.y, 2.0) + pow(j - centroid.x, 2.0));
            
            double sita = atan2(i - centroid.y, j - centroid.x) + PI;
            
            double closest_dist = INFINITY;
            for (int k = 0; k < divide_sita.size(); k ++) {
                
                if( closest_dist > abs(sita - divide_sita[k])) {
                    closest_dist = abs(sita - divide_sita[k]);
                }
                
                if( closest_dist > abs(sita - (divide_sita[k] + 2.0*PI)) ) {
                    closest_dist = abs(sita - (divide_sita[k] + 2.0*PI));
                }
                
                if( closest_dist > abs(sita + 2.0*PI - divide_sita[k])) {
                    closest_dist = abs(sita + 2.0*PI - divide_sita[k]);
                }
            }
            
            if ( r < max_radius ) {
                
                canvas.at<double>(i, j) = gaussian_sample.at<double>((int)(closest_dist/(PI/slice_num)*gaussian_sample.rows), 0);
                
            }
            
            
        }
    }


    vector<double>().swap(divide_sita);
    gaussian_sample.release();
    
    return canvas;
}






















