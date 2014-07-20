#include <iostream>
#include <fstream>
#include <vector>
#include "cv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;
using namespace cv;

/**
 * A: input measurement matrix
 * b: input observations
 * k: sparsity dimensions
 * x: output 
 */ 
void OthogonalMatchingPursuit(const Mat_<double>& A,                              
                              const Mat_<double>& b,
                              int k,
                              Mat_<double>& x){
  
    // initialize
    Mat_<double> r = b.clone(); 
    double norm_r = norm(r);
    
    Mat_<double> A_transpose;
    transpose(A,A_transpose);
    
    Mat_<double> A_r = A_transpose * r; 
    int N = A_r.rows;
    int M = r.rows;

    if(k > M){
        cout<<"K cannot be larger than the dimension of the atoms"<<endl;
        exit(-1); 
    } 
    Mat_<double> unit_vector = Mat::zeros(N,1,CV_64FC1); 
    x = Mat::zeros(N,1,CV_64FC1);
    
    vector<int> index_set;
    index_set.resize(k);
    Mat_<double> A_T = Mat::zeros(M,k,CV_64FC1);
    Mat_<double> A_T_nonorth = Mat::zeros(M,k,CV_64FC1);
    Mat_<double> x_T;

    for(int i = 0;i < k;i++){
        int new_index = 0;
        double max_ele = -1; 
        // find new index and atom to add
        for(int j = 0;j < A_r.rows;j++){
            if(abs(A_r(j)) > max_ele){
                max_ele  = abs(A_r(j));
                new_index = j;
            } 
        }
        // add new index to index set
        index_set[i] = new_index;

        // get new atom
        Mat_<double> atom_new = A(Range::all(),cv::Range(new_index,new_index+1)).clone();
        
        
        atom_new.copyTo(A_T_nonorth(Range::all(),Range(i,i+1)));

        for(int j = 0;j < i;j++){
            Mat_<double> temp = A_T(Range::all(),cv::Range(j,j+1));
            Mat_<double> temp_transpose;
            transpose(temp,temp_transpose);
            temp_transpose = temp_transpose * atom_new;
            atom_new = atom_new - temp_transpose(0) * temp; 
        } 
        
        atom_new = atom_new / norm(atom_new);
        atom_new.copyTo(A_T(Range::all(),cv::Range(i,i+1)));
       
        transpose(A_T(Range::all(),Range(0,i+1)),x_T);
        x_T = x_T * b;
        for(int j = 0;j < i+1;j++){
            int index = index_set[j];
            x(index) = x_T(j); 
        } 
        r = b - A_T(Range::all(),Range(0,i+1)) * x_T;
        A_r = A_transpose * r;
    }
    cv::solve(A_T_nonorth(Range::all(), Range(0,k)), b, x_T, DECOMP_QR);
    for(int i = 0;i < k;i++){
        int index = index_set[i];
        x(index) = x_T(i); 
    } 
}







