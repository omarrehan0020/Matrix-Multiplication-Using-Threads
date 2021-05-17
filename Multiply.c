#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

FILE * matA;
FILE * matB;
FILE * matC;

long unsigned int bufferLength = 1000000;

int **matrixA ;
int **matrixB;
int **matrixC;

int rowA;
int rowB;
int colA;
int colB;
int v=0;

struct file_req{
    char *file_name;
    char type;
};

void* read_array(struct file_req* arg){
    struct file_req *f = (struct file_req *)arg;
    int h,g,d;
    if(f->type == 'a'){
        matA = fopen(f->file_name,"r");
        if(matA == NULL){
            printf("No such file!\n");
            v=1;
            return NULL;
        }
        fscanf(matA,"row=%d col=%d",&rowA,&colA);
        matrixA = malloc(rowA * sizeof(*matrixA));
        	if(!matrixA) /* If `malloc` failed */
        	{
        	    exit(-1);
        	}
        		for(int i = 0; i < rowA; i++)
        		{
        		    matrixA[i] = malloc(colA * sizeof(**matrixA));
        		    if(!matrixA[i]) /* If `malloc` failed */
        		    {
        		        for(int j = 0; j < i; j++) /* free previously allocated memory */
        		        {
        		            free(matrixA[j]);
        		        }
        		        free(matrixA);

        		        exit(-1);
        		    }
        		}
        		char* line = (char*) malloc(bufferLength);
               int n=0 ;
               getline(&line,&bufferLength,matA);
        		while(getline(&line,&bufferLength,matA) != -1){
        					for (int j=0;j<colA;j++){
        						  matrixA[n][j] = strtol(line,&line,0) ;
        		}
        					n++ ;

        			}
        fclose(matA);
    }
    else if(f->type == 'b'){
        matB = fopen(f->file_name,"r");
        if(matB == NULL){
            printf("No such file!\n");
            v=1;
            return NULL;
        }
        fscanf(matB,"row=%d col=%d",&rowB,&colB);
                matrixB = malloc(rowB * sizeof(*matrixB));
                	if(!matrixB) /* If `malloc` failed */
                	{
                	    exit(-1);
                	}
                		for(int i = 0; i < rowB; i++)
                		{
                		    matrixB[i] = malloc(colB * sizeof(**matrixB));
                		    if(!matrixB[i]) /* If `malloc` failed */
                		    {
                		        for(int j = 0; j < i; j++) /* free previously allocated memory */
                		        {
                		            free(matrixB[j]);
                		        }
                		        free(matrixB);

                		        exit(-1);
                		    }
                		}
                for(h=0;h<rowB;h++){
                    for(g=0;g<colB;g++){
                    	matrixB[h][g]=0 ;
                        if(fscanf(matB,"%d ",&matrixB[h][g]) < 1){
                            printf("Error occured while parsing values! in A at i=%d j=%d\n",h,g);
                            fclose(matB);
                            v=1;
                            return NULL;
                        }
                    }
                }
                fclose(matB);
    }
    else{
        printf("Error occurred!");
        v=1;
        return NULL;
    }
    return NULL;
}

void mallocC()
{
	 matrixC = malloc(rowA * sizeof(*matrixC));
	        	if(!matrixC) /* If `malloc` failed */
	        	{
	        	    exit(-1);
	        	}
	        		for(int i = 0; i < rowA; i++)
	        		{
	        		    matrixC[i] = malloc(colB * sizeof(**matrixC));
	        		    if(!matrixC[i]) /* If `malloc` failed */
	        		    {
	        		        for(int j = 0; j < i; j++) /* free previously allocated memory */
	        		        {
	        		            free(matrixC[j]);
	        		        }
	        		        free(matrixC);

	        		        exit(-1);
	        		    }
	        		}
}

void method1(){
	 for (int i = 0; i < rowA; ++i) {
	      for (int j = 0; j < colB; ++j) {
	    	  matrixC[i][j]=0 ;
	         for (int k = 0; k < colA; ++k) {
	            matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
	         }
	      }
	   }
}
void* multi_2(void* arg)
{
    long int core = (long int) arg ;

    for (int i = core ; i < core + 1; i++)
        for (int j = 0; j < colB; j++)
        {
        	 matrixC[i][j]=0 ;
            for (int k = 0; k < colA; k++)
            	matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
        }
    pthread_exit(NULL) ;
}
void method2(){
    // declaring threads
    pthread_t threads[rowA];

    // Creating  threads, each evaluating its own part
    for (long int i = 0; i < rowA; i++) {
        pthread_create(&threads[i], NULL, multi_2, (void*)(i));
    }

    // joining and waiting for all threads to complete
    for (int i = 0; i < rowA; i++)
        pthread_join(threads[i], NULL);
}
void* multi_3(void* arg)
{
    long int core = (long int) arg ;
    long int r = core%rowA ;
    long int c = core/rowA ;
    matrixC[r][c]=0 ;
            for (int k = 0; k < colA; k++)
            	matrixC[r][c] += matrixA[r][k] * matrixB[k][c];
    pthread_exit(NULL);
}
void method3(){
    // declaring threads
    pthread_t threads[rowA*colB];

    // Creating  threads, each evaluating its own part
    for (long int i = 0; i < rowA*colB; i++) {
        pthread_create(&threads[i], NULL, multi_3, (void*)(i));
    }
    // joining and waiting for all threads to complete
    for (int i = 0; i < rowA*colB; i++)
        pthread_join(threads[i], NULL);
}





int main() {


	 struct timeval stop, start;
	 struct file_req fa,fb,fc;
	 fa.file_name ="a.txt";
	 fa.type='a' ;
	 fb.file_name ="b.txt";
	 fb.type='b' ;
	 fc.file_name ="c.txt";
	 fc.type='c' ;


	 read_array(&fa) ;
	 read_array(&fb) ;
	 if(colA != rowB)
	 {
		 matC = fopen(fc.file_name,"w");
			    if(matC == NULL){
			        printf("output file does not exist!");
			        return 0;
			    }
			    fprintf(matC,"Can not multiply these matrices\n");
			    fclose(matC);
			    return 0;
	 }

	 mallocC();


	 gettimeofday(&start,NULL);
	 method1();
	 gettimeofday(&stop,NULL);

	    matC = fopen(fc.file_name,"w");
	    if(matC == NULL){
	        printf("output file does not exist!");
	        return 0;
	    }
	    fprintf(matC,"Output file:\n");
	    fprintf(matC,"\n");
	    fclose(matC);

	    long time1inMicro = stop.tv_usec - start.tv_usec;

	       matC = fopen(fc.file_name,"a+");
	       fprintf(matC,"Method 1 output:\n");
	       fprintf(matC,"\n");
	       for(int p=0;p<rowA;p++){
	           for(int q=0;q<colB;q++){
	               fprintf(matC,"%d ",matrixC[p][q]);
	           }
	           fprintf(matC,"\n");
	       }
	       fprintf(matC,"\n");
	       fprintf(matC,"Time token in micro-seconds: %lu\n",time1inMicro);
	       fprintf(matC,"\n");
	       fprintf(matC,"Number of Threads : Only main thread");

	       fprintf(matC,"\n");
	       fprintf(matC,"******************************************************\n");

	       gettimeofday(&start,NULL);
	       method2();
	       gettimeofday(&stop,NULL);
	        time1inMicro = stop.tv_usec - start.tv_usec;

	       	       matC = fopen(fc.file_name,"a+");
	       	       fprintf(matC,"Method 2 output:\n");
	       	       fprintf(matC,"\n");
	       	       for(int p=0;p<rowA;p++){
	       	           for(int q=0;q<colB;q++){
	       	               fprintf(matC,"%d ",matrixC[p][q]);
	       	           }
	       	           fprintf(matC,"\n");
	       	       }
	       	       fprintf(matC,"\n");
	       	       fprintf(matC,"Time token in micro-seconds: %lu\n",time1inMicro);
	       	       fprintf(matC,"\n");
	       	       fprintf(matC,"Number of Threads: Main Thread +  %d\n",rowA);
	       	  	   fprintf(matC,"\n");
	    	       fprintf(matC,"******************************************************\n");


	    	             gettimeofday(&start,NULL);
	    	      	       method3();
	    	      	       gettimeofday(&stop,NULL);
	    	      	        time1inMicro = stop.tv_usec - start.tv_usec;

	    	      	       	       matC = fopen(fc.file_name,"a+");
	    	      	       	       fprintf(matC,"Method 3 output:\n");
	    	      	       	       fprintf(matC,"\n");
	    	      	       	       for(int p=0;p<rowA;p++){
	    	      	       	           for(int q=0;q<colB;q++){
	    	      	       	               fprintf(matC,"%d ",matrixC[p][q]);
	    	      	       	           }
	    	      	       	           fprintf(matC,"\n");
	    	      	       	       }
	    	      	       	       fprintf(matC,"\n");
	    	      	       	       fprintf(matC,"Time token in micro-seconds: %lu\n",time1inMicro);
	    	      	       	       fprintf(matC,"\n");
	    	      	       	       fprintf(matC,"Number of Threads: Main Thread +  %d\n",rowA*colB);
	    	      	       	 	   fprintf(matC,"\n");
	    	      	    	       fprintf(matC,"******************************************************\n");
	    	      	    	     for (int i = 0; i < rowA; i++)
	    	      	    	     {
	    	      	    	    	free(matrixA[i]);
	    	      	    	    	free(matrixC[i]);
	    	      	    	     }
	    	      	    	   for (int i = 0; i < rowB; i++)
	    	      	    	   {
	    	      	    		   free(matrixB[i]);
	    	      	    	   }


   return 0;
}
