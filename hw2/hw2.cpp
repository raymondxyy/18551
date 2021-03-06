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
    Mat& eigvec;
    Mat& eigval;
    Mat& mean;
    int label;
};

typedef struct pc pc;
typedef struct image_source imsrc;

/* Macros */
#define READING "r"
#define WRITING "w"
#define MAXLEN 40
#define CLASS_POS 6

/* Enable when debugging */
#define DEBUG (1)

/* Function prototypes */
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
    vector<pc>* trained = NULL;
    string train_path = "";
    string test_path = "";
    int error = 0;
    int train_flag = 0;
    int test_flag = 0;
    
    
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
    //trained = IPCAtrain(tain);
    
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