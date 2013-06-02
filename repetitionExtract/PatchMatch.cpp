 //
//  PatchMatch.cpp
//  PatchMatch
//
//  Created by Rosani Lin on 13/4/27.
//  Copyright (c) 2013年 Rosani Lin. All rights reserved.
//

#include "PatchMatch.h"


PatchMatch::PatchMatch (int patch_height, int patch_width) {
    
    PATCHHEIGHT = patch_height, PATCHWIDTH = patch_width;
    
    genRandom = RNG(time(NULL));
    
}

PatchMatch::~PatchMatch () {
    
}


void PatchMatch::matchPriorWithMask (const Mat& img_src, const Mat& img_dst, const Mat& src_mask, const Mat& dst_mask, RosMat<Point>& prior, bool randomInit) {
    
    Mat src = img_src.clone(), dst = img_dst.clone();
    
    this->src_mask = src_mask.clone(), this->dst_mask = dst_mask.clone();
    
    if ( src.type() != CV_64FC3 ) {
        src.convertTo(src, CV_64FC3);
        src /= 255.0;
    }
    
    if ( dst.type() != CV_64FC3 ) {
        dst.convertTo(dst, CV_64FC3);
        dst /= 255.0;
    }
    
    
    
    
    RosMat<offsetElement> NearNei = RosMat<offsetElement>(prior.size());
    
    if ( randomInit == false ) {
        for (int i = 0; i < NearNei.rows; i ++) {
            for (int j = 0; j < NearNei.cols; j ++) {
                
                NearNei.at(i, j).offset = prior.at(i, j);
                
                NearNei.at(i, j).error = patchNorm(src, Point(j, i) + prior(i, j), dst, Point(j, i), PATCHWIDTH);
                
//                patchTool.SSD(src, Point(j, i) + prior.at(i, j), dst, Point(j, i));
            }
        }
    }
    
    else {
        
        // random initialization of Nearest Neighbor Field
        for (int i = 0; i < NearNei.rows; i ++) {
            for (int j = 0; j < NearNei.cols; j ++) {
                
                if ( dst_mask.at<double>(i, j) ) {
                    
                    Point random_temp = Point(genRandom.uniform(0, src.cols) , genRandom.uniform(0, src.rows)) - Point(j, i);
                    
                    if ( src_mask.at<double>(random_temp) ) {
                        
                        NearNei.at(i, j).offset = random_temp;
                    }
                    
                    
                    NearNei.at(i, j).error = patchNorm(src, Point(j, i) + NearNei(i, j).offset, dst, Point(j, i), PATCHWIDTH);
                }
                
                else {
                    
                    NearNei.at(i, j).offset = Point(0);
                    
                    NearNei.at(i, j).error = INFINITY;
                }
                
            }
        }
        
    }
    
    
    
    match(src, dst, NearNei);
    
    for (int i = 0; i < prior.rows; i ++) {
        for (int j = 0; j < prior.cols; j ++) {
            
            prior.at(i, j).x = NearNei.at(i, j).offset.x;
            prior.at(i, j).y = NearNei.at(i, j).offset.y;
            
        }
    }

    
    
}


void PatchMatch::match(const cv::Mat &src, const cv::Mat &dst, RosMat<offsetElement> &NearNei) {
    
    double err = 0, pre_err = 0;
    do {
        
        pre_err = err;
        
        propagation(src, dst, NearNei);
        
        err = errorComputation(dst, coordinateMapping(src, NearNei));
//        cout<<err<<", "<<abs(err - pre_err)<<" : "<<HALTING_PARA*err<<endl;
        
        
    } while ( abs(err - pre_err) > HALTING_PARA*err);
    
    
    showMat(coordinateMapping(src, NearNei), "map", 0);
//    showMat(dst, "d", 1);
//    showMat(src, "s", 0);
    reconstErrorDisplay(dst, coordinateMapping(src, NearNei));
    distanceDisplay(NearNei);
    
    
}


void PatchMatch::propagation(const cv::Mat &src, const cv::Mat &dst, RosMat<offsetElement> &NearNei) {
    
    for (int i = 0; i < NearNei.rows; i ++) {
        for (int j = 0; j < NearNei.cols; j ++) {
            
            if ( dst_mask.at<double>(i, j) ) {
                
                if ( i >= 1 ) {
                    
                    update(src, dst, NearNei(i, j), NearNei(i - 1, j).offset + Point(j, i), Point(j, i));
                    
                }
                
                if ( j >= 1 ) {
                    
                    update(src, dst, NearNei(i, j), NearNei(i, j - 1).offset + Point(j, i), Point(j, i));
                    
                }
                
                randomSearch(src, dst, NearNei(i, j), Point(j, i));

                
            }
            

            
        }
    }
    
    
    for (int i = NearNei.rows - 1; i >= 0; i --) {
        for (int j = NearNei.cols - 1; j >= 0; j --) {
            
            if ( dst_mask.at<double>(i, j)) {
                
                if ( i < NearNei.rows - 1 ) {
                    
                    update(src, dst, NearNei(i, j), NearNei(i + 1, j).offset + Point(j, i), Point(j, i));
                    
                }
                
                if ( j < NearNei.cols - 1 ) {
                    
                    update(src, dst, NearNei(i, j), NearNei(i, j + 1).offset + Point(j, i), Point(j, i));
                    
                }
                
                randomSearch(src, dst, NearNei(i, j), Point(j, i));
                
            }
            

            
        }
    }
    
    
    
}


void PatchMatch::randomSearch (const Mat& src, const Mat& dst, offsetElement& NearNei_pt, const Point loc) {
    
    double alpha = 1.0, w = MAX(dst.rows, src.rows);
    
    while ( alpha * w >= 1 ) {
        
        
        Point random_loc = NearNei_pt.offset + loc + Point(w*alpha*genRandom.uniform(-1.0, 1.0), w*alpha*genRandom.uniform(-1.0, 1.0));
        
        random_loc = Point((random_loc.x + src.cols)%src.cols, (random_loc.y + src.rows)%src.rows);
        
        
        update(src, dst, NearNei_pt, random_loc, loc);
        
        alpha /= 2.0;
    }
    
}


bool PatchMatch::convergenceTest(RosMat<offsetElement> &NearNei_new, RosMat<offsetElement> &NearNei_old, int itr) {
    
    if ( NearNei_new.rows != NearNei_old.rows || NearNei_new.cols != NearNei_old.cols ) {
        
        return false;
    }
    
    
    bool test = true;
    
    for (int i = 0; i < NearNei_new.rows && test; i ++) {
        for (int j = 0; j < NearNei_new.cols && test; j ++) {
            
            test = NearNei_new(i, j) == NearNei_old(i, j) ? test : false;
            
        }
    }
    
    if ( test ) {
        cout<<"PatchMatch converges at iteration #"<<itr<<".\n";
    }
    
//    Mat temp = Mat(NearNei_old.rows, NearNei_old.cols, CV_8UC1).setTo(0);
//    
//    for (int i = 0; i < NearNei_new.rows && test; i ++) {
//        for (int j = 0; j < NearNei_new.cols && test; j ++) {
//
//            if ( !(NearNei_new.at(i, j) == NearNei_old.at(i, j)) ) {
//                
//                temp.at<uchar>(i, j) = 255;
//            }
//
//        }
//    }
//    
//    
//    showMat(temp, "dis", 1);
//    temp.release();
//    test = false;
    
    return test;
}


void PatchMatch::upsample(const cv::Mat &src, const cv::Mat &dst, RosMat<offsetElement> &NearNei) {
    
    RosMat<offsetElement> NearNei_temp = RosMat<offsetElement>(NearNei.rows, NearNei.cols);
    
    NearNei_temp = NearNei;//.copyFrom(NearNei);
    
    NearNei.resize(dst.size());
    
//    cout<<NearNei.rows<<", "<<NearNei.cols<<" : "<<NearNeiError.rows<<", "<<NearNeiError.cols<<endl;
//    cout<<NearNei.rows<<", "<<NearNei.cols<<" : "<<NearNeiError.rows<<", "<<NearNeiError.cols<<endl;
    
    for (int i = 0; i < NearNei.rows; i ++) {
        for (int j = 0; j < NearNei.cols; j ++) {
            
            NearNei(i, j).offset = NearNei_temp(i/2, j/2).offset*2;
            
            NearNei(i, j).error = patchNorm(src, Point(j, i) + NearNei(i, j).offset, dst, Point(j, i), PATCHWIDTH);
            
        }
    }
    
    
}


Mat PatchMatch::coordinateMapping (const Mat& src, const RosMat<offsetElement>& NearNei) {
    
    Mat mapped = Mat::zeros(NearNei.rows, NearNei.cols, src.type());
    
    for (int i = 0; i < mapped.rows; i ++) {
        for (int j = 0; j < mapped.cols; j ++) {
            
            if ( dst_mask.at<double>(i, j)) {
                
                if ( !boundaryTest(src, NearNei(i, j).offset + Point(j, i)) ) {
                    
                    cout<<" OUT OF BOUNDARY  size: "<<src.rows<<", "<<src.cols<<" : "<<NearNei(i, j).offset + Point(j, i)<<endl;
                }
                
                mapped.at<Vec3d>(i, j) = src.at<Vec3d>(NearNei.at(i, j).offset + Point(j, i));
            }

        }
    }

    
    return mapped;
    
}


void PatchMatch::update (const Mat& src, const Mat& dst, offsetElement& local_off, const Point& nex_loc, const Point& loc) {
    
    if ( boundaryTest(src, nex_loc) ) {
        
        double nex_err = patchNorm(src, nex_loc, dst, loc, PATCHWIDTH);
        
        if ( local_off.error > nex_err ) {
            
            local_off.offset = nex_loc - loc;
            local_off.error = nex_err;
        }
        
    }
    
    
}


void PatchMatch::reconstDisplay (const Mat& src, RosMat<offsetElement>& NearNei) {
    
    showMat(coordinateMapping(src, NearNei), "Reconstructed dst", 0);
    
}



void PatchMatch::distanceDisplay (RosMat<offsetElement> &array) {
    
    Mat temp = Mat::zeros(array.rows, array.cols, CV_64FC1);
    
    for (int i = 0; i < array.rows; i ++) {
        for (int j = 0; j < array.cols; j ++) {
            
            temp.at<double>(i, j) = sqrt(array.at(i, j).offset.x*array.at(i, j).offset.x + array.at(i, j).offset.y*array.at(i, j).offset.y);
            
        }
    }
    
    double temp_max = maxMatItem(temp);
    temp /= temp_max;
    
    showMat(temp, "distance", 0);

    temp.release();
    
}


void PatchMatch::reconstErrorDisplay (const Mat& dst, const Mat& mapped) {
    
    Mat temp = Mat::zeros(dst.size(), dst.type());
    
    Mat temp_dst = dst.clone();
    
    for (int i = 0; i < temp_dst.rows; i ++) {
        for (int j = 0; j < temp_dst.cols; j ++) {
            
            temp_dst.at<Vec3d>(i, j) = temp_dst.at<Vec3d>(i, j)*dst_mask.at<double>(i, j);
            
        }
    }
    
    
    absdiff(temp_dst, mapped, temp);
    
    
    showMat(temp, "error", 0);
    
    temp.release();
}


double PatchMatch::errorComputation (const Mat& dst, const Mat& mapped) {
    
    double error = 0.0;
    
    for (int i = 0; i < dst.rows; i ++) {
        for (int j = 0; j < dst.cols; j ++) {
            
            if ( dst_mask.at<double>(i, j) ) {
                
                error += patchNorm(dst, Point(j, i), mapped, Point(j, i), PATCHWIDTH);
            }
        
        }
    }
    
//    return norm(dst, mapped, NORM_L2);
    return error;
    
    
}



RosMat<Point> PatchMatch::elementToPoint (const RosMat<offsetElement> &element) {
    
    RosMat<Point> pt = RosMat<Point>(element.size());
    
    for (int i = 0; i < pt.rows; i ++) {
        for (int j = 0; j < pt.cols; j ++) {
            
            pt.at(i, j) = element.at(i, j).offset;
            
        }
    }
    
    return pt;
}


RosMat<offsetElement> PatchMatch::pointToElement (const Mat& src, const Mat& dst, const RosMat<Point> &pt) {
    
    RosMat<offsetElement> element = RosMat<offsetElement>(pt.size());
    
    for (int i = 0; i < element.rows; i ++) {
        for (int j = 0; j < element.cols; j ++) {
            
            element.at(i, j).offset = pt.at(i, j);
            
            element.at(i, j).error = patchNorm(src, Point(j, i) + pt(i, j), dst, Point(j, i), PATCHWIDTH);
            
        }
    }
    
    return element;
}






