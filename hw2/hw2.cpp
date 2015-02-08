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


struct image_source{
    vector<Mat>* data; // matrices from all classes in order
    int num_per_class; // number of matrices per class
};

struct pc{
    Mat eigvec;
    Mat eigval;
    Mat mean;
    int label;
};

typedef struct pc pc;
typedef struct image_source imsrc;

/* Macros */
#define READING "r"
#define WRITING "w"
#define MAXLEN 40
#define CLASS_POS 6
#define NUM_CLASS 4
#define EIGENSIZE 16384

/* Enable when debugging */
#define DEBUG (1)

/* Function prototypes */
void IPCAtrain(imsrc* images, vector<pc>* output);
void DisplayMat(Mat MatDisp);
void path_to_data(string p, imsrc* dst);



/* define program argument list, excluding flags */
enum {ARG_PROG, ARG_FILE, ARG_NARGS};
        
int main(int argc, char* argv[])
{

	// Call IPCAtrain to generate a text file containing eigenvectors and means of each clas

	// Call IPCAtest using a test function and see if it returns the correct class label

	/* STAGE 1: variable initialization */
    FILE* ofp = NULL;
    imsrc train = {NULL,0}; 
    imsrc* trainp = NULL;
    imsrc test = {NULL,0};
    imsrc* testp = NULL;
    vector<pc> trained;
    vector<pc>* trainedp = NULL;
    string train_path = "";
    string test_path = "";
    int error = 0;
    int train_flag = 0;
    int test_flag = 0;
    Mat train_eigval = Mat(EIGENSIZE,EIGENSIZE,CV_32FC1);
    Mat train_eigvec = Mat(EIGENSIZE,EIGENSIZE,CV_32FC1);
    Mat train_mean = Mat(EIGENSIZE,1,CV_32FC1);
    pc component = {train_eigvec,train_eigval,train_mean,0};
    
    /* STAGE 2: input processing (if any) */
    printf("hw2: train data from [train-path] and test data from [test-path].\n");
    
    
	/* one optional flag: -wN */
	if(argc > 1){
		char flag;
		while(argv[1][0] == '-'){
			flag = argv[1][1];
			switch(flag){
			/*TODO: handle any  flag arguments here */
			case('\0'):{
				printf("Error: missing flag name\n");
				return 1;
			}
			case('r'):{// training path
			    train_path = string(&argv[1][2]);
                train_flag = 1;
                break;
			} 

            case('e'):{ // testing path
                test_path = string(&argv[1][2]);
                test_flag = 1;
                break;
            }
			default:
				break;
			}
			argc--;
			argv++;
		}
	}
    
    if (argc < ARG_NARGS){
		printf("insufficient arguments.\n"
			"usage: hw2 [-tTRAINING_PATH][-eTESTING_PATH] FILENAME\n"
            "\tTRAINING_PATH : full path to the training directory"
                " e.g. ./Train_Files/\n"
			"\tTESTING_PATH  : full path to the testing directory"
                " e.g. ./Test_Files/\n"
			"\tFILENAME      : output filename in case of training; "
            "input filename in case of testing\n"
			);
		return 1;
    }
    
#if DEBUG
    cout << "train_path = " << train_path << "\n";
    cout << "test_path = " << test_path << "\n";
#endif    
    
    ofp = fopen(argv[ARG_FILE],WRITING);
    if (ofp == NULL){
		printf("Error: unable to open %s\n", argv[ARG_FILE]);
		error++;
		goto exit;
    }
    //train_path = "./Train_Files/"; //TODO: get path from command line
    if (train_flag){
        trainp = &train;
         path_to_data(train_path,trainp);
    }
       
    //test_path = "./Test_Files"; //TODO: get path from command line
    if (test_flag){
        testp = &test;
        path_to_data(test_path,testp);
    }
        
#if DEBUG
    if (trainp != NULL){
        printf ("train has %lu samples total.\n",trainp->data->size());
        printf("Each class has %d samples.\n", trainp->num_per_class);
    }
    if (testp != NULL){
        printf ("test has %lu samples total.\n",testp->data->size());
        printf("Each class has %d samples.\n", testp->num_per_class);
    }
    else {
        printf("error!\n");
        goto exit;
    }
#endif
    
	/* STAGE 3: actual processing */
    //Mat train_eigval, train_eigvec, train_mean;
    for (int i = 0; i < NUM_CLASS; i++){
        //pc component = {train_eigvec,train_eigval,train_mean,0};
        trained.push_back(component);
    }
    //trained = (NUM_CLASS,{train_eigvec,train_eigval,train_mean,0});
    trainedp = &trained;
    IPCAtrain(trainp, trainedp);
    
    /* STAGE 4: output processing (if any) */



	/* STAGE 5: cleanup and quit */
exit:
    printf("hw2 quitting...\n");
    /* close any file descripter */
    if (ofp != NULL)
        fclose(ofp);
    /* free any occupied memory */
    if ((trainp != NULL) && (trainp->data != NULL))
        trainp->data->clear();
    if ((testp != NULL) && (testp->data != NULL))
        testp->data->clear();
    if (trainedp != NULL) {
        for (int i = 0; i < trainedp->size(); i++){
            trainedp->at(i).eigvec.release();
            trainedp->at(i).eigval.release();
            trainedp->at(i).mean.release();
        }
    }
	return error;
}

void IPCAtrain(imsrc* images, vector<pc>* output)
{
	/* trainFolderPath is the path to the folder containing the training images
	   numTrain is the number of training images per class */
  
  int data_size = images->data->size();
  
  if((data_size % images->num_per_class) != 0){
      output = NULL;
      return;
  }
  int num_classes = data_size / images->num_per_class;
  vector<Mat> data_mat;
  vector<Mat> covar_mat;
  vector<Mat> mean_mat;
  for (int i = 0; i < num_classes; i++){
    mean_mat.push_back(Mat::zeros(EIGENSIZE,1,CV_32FC1));
      data_matp.push_back(Mat(EIGENSIZE,images->num_per_class,CV_32FC1));
      covar_mat.push_back(Mat(EIGENSIZE,EIGENSIZE,CV_32FC1));
  }
			
  // Run a loop to iterate over classes (people)
  for(int i = 0; i < num_classes; i++){
    //Run a loop to iterate over images of same person and generate the data matrix for the class
    //i.e. a matrix in which each column is a vectorized version of the face matrix under consideration
    for(int j = 0; j < images->num_per_class; j++){
      data_mat[i].row(j) = images->data->at((i * num_classes)+j).\
	reshape(1,EIGENSIZE);
      mean_mat[i] = mean_mat[i]+(data_mat[i].row(j)/images->num_per_class);
    }
    /* calcCovarMatrix(data_matp[i],images->num_per_class, covar_mat[i], 
       output->at(i).mean, CV_COVAR_NORMAL | CV_COVAR_COLS, CV_32FC1);*/
    for(int k = 0; k < images->num_per_class; k++){
      data_mat[i].row(j) = data_mat[i].row(j) - mean_mat[i];
    }
    Mat temp_transpose;
    transpose(data_mat[i],temp_transpose);
    covar_mat[i] = data_mat[i]*temp_transpose;
    eigen(covar_mat[i],1,output->at(i).eigvec,output->at(i).eigval);
    output->at(i).label = i;
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
void path_to_data(string p, imsrc* dst){
	DIR* dir = NULL;
	struct dirent *entry = NULL;
    string* paths = new string[MAXLEN];
    int size = 0;
    int num_per_class = 0;
    int flag = 1;
    int plen = p.length();
    int offset = plen + CLASS_POS-1; // index of class name in full path
    if((dir = opendir(p.c_str())) != NULL){
        while((entry = readdir(dir)) != NULL){
            if( strcmp(entry->d_name, ".") != 0 
                    && strcmp(entry->d_name, "..") != 0){
                paths[size] = p + string(entry->d_name);
                if (flag && ((!size) || 
                    (paths[size][offset] == paths[size-1][offset]))){
                    num_per_class++;
                }
                else
                    flag = 0;
                size++;
            }
        }
        vector<Mat>* d = new vector<Mat>(size);
        for (int i = 0; i < size; i++){
            d->at(i) = imread(paths[i]);
        }
        dst->data = d;
        dst->num_per_class = num_per_class;
		closedir(dir);
        return;
	}
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
