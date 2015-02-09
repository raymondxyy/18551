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
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
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
#define LINESIZE 1024

/* Enable when debugging */
#define DEBUG (0)

/* Function prototypes */
void IPCAtrain(imsrc* images, vector<pc>* output);
void IPCAtest(imsrc* images, vector<pc>* trained, vector<int>* testedp);
void write_file(pc component, FILE* fp);
void read_file(vector<pc>* trained, char* filename);
void path_to_data(string p, imsrc* dst);


/* define program argument list, excluding flags */
enum {ARG_PROG, ARG_FILE, ARG_NARGS};
        
int main(int argc, char* argv[])
{
	/* STAGE 1: variable initialization */
    FILE* ofp = NULL;
    imsrc train = {NULL,0}; 
    imsrc* trainp = NULL;
    imsrc test = {NULL,0};
    imsrc* testp = NULL;
    vector<pc> trained;
    vector<pc>* trainedp = NULL;
    vector<int> tested;
    vector<int>* testedp = NULL;
    string train_path = "";
    string test_path = "";
    char* mode = NULL;
    int error = 0;
    int train_flag = 0;
    int test_flag = 0;
    Mat train_eigvec;
    Mat train_mean;
    
    
    /* STAGE 2: input processing (if any) */
    printf("hw2: train data from [train-path]"
        " and test data from [test-path].\n");
    
    
    
	/* two optional flags: -e[test_path] -r[train_path] */
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
    
    printf("INPUT PROCESSING...\n");
    
    cout << "\ttrain_path = " << train_path << "\n";
    if (train_flag){
        ofp = fopen(argv[ARG_FILE],WRITING);
        if (ofp == NULL){
		    printf("Error: unable to open %s\n", argv[ARG_FILE]);
		    error++;
		    goto exit;
        }
        trainp = &train;
        path_to_data(train_path,trainp);
    }
    if (trainp != NULL){
        printf ("\ttrain has %lu samples total.\n",trainp->data->size());
        printf("\tEach class has %d samples.\n", trainp->num_per_class);
    }
    
    cout << "\ttest_path = " << test_path << "\n";   
    if (test_flag){
        testp = &test;
        path_to_data(test_path,testp);
    }
    if (testp != NULL){
        printf ("\ttest has %lu samples total.\n",testp->data->size());
        printf("\tEach class has %d samples.\n", testp->num_per_class);
    }
    
    if ((testp == NULL) && (trainp == NULL)) {
        printf("\terror!\n");
        goto exit;
    }
    
    
	/* STAGE 3: actual processing */
    for (int i = 0; i < NUM_CLASS; i++){
        pc component = {train_eigvec,train_mean,0};
        trained.push_back(component);
    }
    
    trainedp = &trained;
    if (train_flag){ // TRAINING ONLY
        printf("TRAINING...\n");
        IPCAtrain(trainp, trainedp);
    
        if (trainedp != NULL){
            printf("\tTraining complete without error.\n");
        }
        else{
            printf("\tError encountered when training.\n");
            goto exit;
        }
    }
    
    if (test_flag){ // TESTING
        printf("TESTING...\n");
        if (!train_flag){ //TESTING WITHOUT ACTUAL TRAINING
            printf("\tREADING TRAINING DATA FROM FILE...\n");
            read_file(trainedp,argv[ARG_FILE]);
        }
        testedp = &tested;
        IPCAtest(testp, trainedp, testedp);
        if (testedp != NULL){
            printf("\tTesting complete without error.\n");
            printf("\tPRINTING RESULT...\n");
            for (int i = 0; i < testedp->size(); i++){
                printf("\tSample is classified to Class %d.\n",\
                testedp->at(i));
            }
        }
        else{
            printf("\tError encountered when training.\n");
            goto exit;
        }
    }
    
    /* STAGE 4: output processing (if any) */
    printf("OUTPUT PROCESSING...\n");
    if (train_flag && (trainedp != NULL)){
        printf("\tWriting trained data to %s...\n",argv[ARG_FILE]);
        for (int i = 0; i < trainedp->size(); i++){
            write_file(trainedp->at(i),ofp);
        }
    }
    


	/* STAGE 5: cleanup and quit */
exit:
    printf("hw2 quitting...\n");
    /* close any file descripter */
    if (ofp != NULL)
        fclose(ofp);
    /* free any occupied memory */

	return error;
}

void IPCAtrain(imsrc* images, vector<pc>* output){
    int data_size = images->data->size();
  
    if((data_size % images->num_per_class) != 0){
        output = NULL;
        return;
    }
    int num_classes = data_size / images->num_per_class;
    vector<Mat> data_mat;
    vector<Mat> mean_mat;
    for (int i = 0; i < num_classes; i++){
        data_mat.push_back\
            (Mat::zeros(EIGENSIZE,images->num_per_class,CV_32FC1));
    }
			
    // Run a loop to iterate over classes (people)
    for(int i = 0; i < num_classes; i++){
        /* Run a loop to iterate over images of same person 
           and generate the data matrix for the class
           i.e. a matrix in which each column is a 
           vectorized version of the face matrix under consideration
        */
        for(int j = 0; j < images->num_per_class; j++){
            int idx = (i * images->num_per_class)+j;
            Mat temp = images->data->at(idx).reshape(1,EIGENSIZE);
            temp.copyTo(data_mat[i].col(j));
        }
    
        /* calculate mean */
        Mat cur_mean = Mat::zeros(EIGENSIZE,1,CV_32FC1);
        for(int j = 0; j < images->num_per_class; j++){
            cur_mean += data_mat[i].col(j);
        }
        cur_mean /= images->num_per_class;
    
        /* subtract mean from each column of X */
        for(int j = 0; j < images->num_per_class; j++){
            Mat temp = data_mat[i].col(j) - cur_mean;
            temp.copyTo(data_mat[i].col(j));
        }
    
        /* calculate eigenvectors using Gram's trick */
        Mat temp_transpose;
        transpose(data_mat[i],temp_transpose);
        Mat covar_mat = temp_transpose*data_mat[i];
        Mat val, vec;
        eigen(covar_mat,val,vec);
        transpose(vec,vec);
        // keep only 7 biggest eigenvectors
        vec = vec.colRange(0,images->num_per_class-1); 
        Mat eigvec = data_mat[i]*vec;
        /* normalize */
        for(int j = 0; j < images->num_per_class-1; j++){
            double cur_norm = norm(eigvec.col(j));
            eigvec.col(j) /= cur_norm;
        }
        output->at(i).eigvec = eigvec;
        output->at(i).mean = cur_mean;
        output->at(i).label = i+1;
    }   
}

void IPCAtest(imsrc* images, vector<pc>* trained, vector<int>* testedp){
    int num_class = trained->size();
    int data_size = images->data->size();
  
    if((data_size % images->num_per_class) != 0){
        testedp = NULL;
        return;
    }
    
    for (int i = 0; i < data_size; i++){
        Mat current_img = images->data->at(i);
        Mat flattened;
        current_img.convertTo(flattened, CV_32FC1);
        flattened = flattened.reshape(1,EIGENSIZE);
        double min_norm = -1.0;
        int min_label = -1;
        for (int j = 0; j < num_class; j++){
            pc component = trained->at(j);
            Mat eigvec_trans;
            transpose(component.eigvec, eigvec_trans);
            // projection
            Mat coeff = eigvec_trans * (flattened-component.mean); 
            Mat reconstruct = component.eigvec*coeff;
            reconstruct += component.mean;
            // find euclidean distance
            double cur_norm = norm(reconstruct,flattened,NORM_L2);
            if ((min_norm < 0) || (cur_norm < min_norm)){
                min_norm = cur_norm;
                min_label = component.label;
            }
#if DEBUG
            printf("Class %d has norm %f.\n",j+1,cur_norm);
#endif
        }
        testedp->push_back(min_label);
    }
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
#if DEBUG
                        cout << "\t" << paths[size] << "\n";
#endif
                        num_per_class++;
                }
                else
                    flag = 0;
                size++;
            }
        }
        vector<Mat>* d = new vector<Mat>(size);
        for (int i = 0; i < size; i++){
            d->at(i) = imread(paths[i],CV_LOAD_IMAGE_GRAYSCALE);
        }
        dst->data = d;
        dst->num_per_class = num_per_class;
		closedir(dir);
        return;
	}
}


void write_file(pc component, FILE* fp){
    int rows = component.eigvec.rows+component.mean.rows;
    int cols = component.eigvec.cols+component.mean.cols;
    fprintf(fp,"CLASS: %d\n",component.label);
    fprintf(fp,"EIGENVECTORS: %d * %d\n",\
        component.eigvec.rows,component.eigvec.cols);
	for (int i = 0; i < component.eigvec.rows; i++){
		for (int j = 0; j < component.eigvec.cols; j++){
			fprintf(fp,"%f ",component.eigvec.at<float>(i,j));
		}
		fprintf(fp,"\n");
	}
    fprintf(fp,"MEAN: %d * %d\n",component.mean.rows,component.mean.cols);
	for (int i = 0; i < component.mean.rows; i++){
		for (int j = 0; j < component.mean.cols; j++){
			fprintf(fp,"%f ",component.mean.at<float>(i,j));
		}
		fprintf(fp,"\n");
	}
}

void read_file(vector<pc>* trained, char* filename){
    int flag = 0;
    int class_label = -1;
    int eig_row = 0; int eig_col = 0;
    int mean_row = 0; int mean_col = 0;
    ifstream infile(filename);
    while (infile.good()){
        string line = "";
        if (!getline( infile,line)) break;
        if (line[0] == 'C'){
            stringstream s(line);
            string dummy;
            s >> dummy >> class_label;
            class_label--; // for correct indexing
            trained->at(class_label).label = class_label+1;
#if DEBUG
            printf("\tGet CLASS:%d\n",class_label+1);
#endif
            continue;
        }
        if (line[0] == 'E'){
            stringstream s(line);
            string dummy; char c;
            s >> dummy >> eig_row >> c >> eig_col;
            flag = 1;
#if DEBUG
            printf("\tGet EIGENVECTORS:%d*%d\n",eig_row,eig_col);
#endif
            Mat eig_vec = Mat::zeros(eig_row,eig_col,CV_32FC1);
            for (int i = 0; i < eig_row; i++){         
                if (!getline( infile,line)){
                    printf("Error!\n");
                    break;
                }     
                stringstream s(line);
                for(int j = 0; j < eig_col; j++){
                    s >> eig_vec.at<float>(i,j);
                }
            } 
            trained->at(class_label).eigvec = eig_vec;
            continue;
        }
        else if (line[0] == 'M'){
            stringstream s(line);
            string dummy; char c;
            s >> dummy >> mean_row >> c >> mean_col;
            flag = 2;
#if DEBUG
            printf("\tGet MEAN:%d*%d\n",mean_row,mean_col);
#endif      
            Mat mean = Mat::zeros(mean_row,mean_col,CV_32FC1);
            for (int i = 0; i < mean_row; i++){         
                if (!getline( infile,line)){
                    printf("Error!\n");
                    break;
                }     
                stringstream s(line);
                for(int j = 0; j < mean_col; j++){
                    s >> mean.at<float>(i,j);
                }
            } 
            trained->at(class_label).mean = mean;
            continue;
            
        }
        else{
            printf("Error! should not get here!\n");
        }
    }
    if (infile.is_open())
        infile.close();
}

