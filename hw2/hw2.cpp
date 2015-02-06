/*
    Course: 18551 Digital Communication and Signal Processing
    Homework 2: Face Recognition
    Authors: Lucas Crandall, Raymond Xia
    Due date: February 9, 2015

    usage: hw2 train-path test-path outfile
        train-path : path of training files relative to hw2.cpp
        test-path  : path of test files relative to hw2.cpp
        outfile    : [file name].txt containing training result
*/

#include <iostream>
#include <opencv.hpp>
#include <string>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <dirent.h>

using namespace cv;
using namespace std;

/* Macros */
#define READING "r"
#define WRITING "w"
#define MAXLEN 40

/* Enable when debugging */
#define DEBUG (1)

/* Function prototypes */
void DisplayMat(Mat MatDisp);
vector<Mat>* path_to_data(string p);



/* define program argument list, excluding flags */
enum {ARG_PROG, ARG_TRAINPATH, ARG_TESTPATH, ARG_OUTFILE, ARG_NARGS};
        
int main()
{

	// Call IPCAtrain to generate a text file containing eigenvectors and means of each clas

	// Call IPCAtest using a test function and see if it returns the correct class label

	/* STAGE 1: variable initialization */
    int error = 0;
    vector<Mat>* train = NULL;
    int train_size = 0;
    
    
    /* STAGE 2: input processing (if any) */
    string p = "./Train_Files/";
    train = path_to_data(p);
#if DEBUG
    if (train != NULL)
        printf ("train has size %lu\n",train->size());
    else
        printf("error!\n");
#endif
	/* STAGE 3: actual processing */
    /* STAGE 4: output processing (if any) */



	/* STAGE 5: cleanup and quit */
exit:
    printf("hw2 quitting...\n");
    /* close any file descripter */
    
    /* free any occupied memory */
    if (train != NULL)
        train->clear();
	return error;
}
void IPCAtrain(char* trainFolderPath, int numTrain)
{
	/* trainFolderPath is the path to the folder containing the training images
	   numTrain is the number of training images per class */

	// Run a loop to iterate over classes (people)
	for(;;)
	{
		//Run a loop to iterate over images of same person and generate the data matrix for the class 
		//i.e. a matrix in which each column is a vectorized version of the face matrix under consideration

		// Subtract the mean vector from each vector of the data matrix

		// Compute the covariance matrix and generate the eigen vectors using the Gram trick
		
		// Store the eigen vectors and the mean vector in a file, which will be accessed by the IPCAtest function
	
	}

}

void IPCAtest(char *imgName)
{
	/* imgName is the path and filename of the test image */

	// Read the eigen vectors and means for each class from file
	
	// Project the input test image onto each eigen space and reconstruct

	// Compute the reconstruction error between the input test image and the reconstructed image
	// You can use euclidean distance (or any other appropriate distance measure)

	// return the class label corresponding to the eigen space which showed minimum reconstruction error 

}

/* find the paths of all the files in p, 
   and construct dst from all images in p */
vector<Mat>* path_to_data(string p){
	DIR* dir = NULL;
	struct dirent *entry = NULL;
    string* paths = new string[MAXLEN];
    int size = 0;
    if((dir = opendir(p.c_str())) != NULL){
        while((entry = readdir(dir)) != NULL){
            if( strcmp(entry->d_name, ".") != 0 
                    && strcmp(entry->d_name, "..") != 0){
                paths[size] = p + string(entry->d_name);
                size += 1;
            }
        }
#if DEBUG
        printf("size:%d\n",size);
#endif
        vector<Mat>* data = new vector<Mat>(size);
        for (int i = 0; i < size; i++){
            data->push_back(imread(paths[i]));
        }
		closedir(dir);
        return data;
	}
    return NULL;
}

/* brought from Sample_Code.cpp */
void DisplayMat(Mat MatDisp){
	for (int i = 0; i < MatDisp.rows; i++){
		for (int j = 0; j < MatDisp.cols; j++){
			printf("%f ",MatDisp.at<float>(i,j));
		}
		printf("\n");
	}
}