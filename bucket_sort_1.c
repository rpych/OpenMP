#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "list_utils.h"

#define SIZE 1000
#define PRINT 1


void generateData(int* buffer)
{
    int i;
    #pragma omp parallel 
    {
    unsigned int seed = ((unsigned int) omp_get_wtime()) + omp_get_thread_num();
    #pragma omp for private(i)
    for(i=0;i<SIZE;i++)
    {
        
        buffer[i] = rand_r(&seed) % SIZE;
    }
            
    }
}


int findMax(int* buffer)
{
  int max_val = buffer[0];
  #pragma omp parallel shared(buffer,max_val)
  {
      int i;
      #pragma omp for private(i) reduction(max:max_val)
      for(i=0;i<SIZE;++i)
      {
         max_val = buffer[i] > max_val ? buffer[i]: max_val;
      }
         
  }

    return max_val;
}


int findMin(int* buffer)
{
  int min_val = buffer[0];
  #pragma omp parallel shared(buffer,min_val)
  {
      int i;
      #pragma omp for private(i) reduction(min:min_val)
      for(i=0;i<SIZE;++i)
      {
        min_val = buffer[i] < min_val ? buffer[i]: min_val;
      }
          
  }

    return min_val;
}

int isElemInNthBucket(int elem, int nth_bucket, int BUCKET_SIZE, int min_elem, int K)
{
    int offset_elem = elem - min_elem;
    int bucket = (offset_elem / BUCKET_SIZE);
    return bucket == nth_bucket || (bucket == K && nth_bucket == (K-1));
}


void splitToBuckets(int* buffer, Node** buckets, int thread_num, int priv_start_idx,
                    int BUCKET_SIZE, int min, int K, int* buck_counter)
{
    int i;
    for(i=priv_start_idx;i<SIZE;++i)
    {
        if(isElemInNthBucket(buffer[i],thread_num,BUCKET_SIZE,min,K))
        {
            push(&buckets[thread_num], buffer[i]);
            (*buck_counter)++;
        }
    }

    #pragma omp critical
    {
        for(i=priv_start_idx;i>=0;i--)
        {
            if(isElemInNthBucket(buffer[i],thread_num,BUCKET_SIZE,min,K))
            {
                push(&buckets[thread_num], buffer[i]);
                (*buck_counter)++;
            }
        }
    }
}

int countElemsInPrevBuckets(long* buckets_elems_counter, int thread_num)
{
    int less_bucks_counter = 0;
    if(thread_num == 0)
    {
        less_bucks_counter = 0;
    }
    else
    {
        int i;
        for(i=0;i<thread_num;++i)
        {
            less_bucks_counter += buckets_elems_counter[i];
            //if(i==thread_num-1)
                //printf("Bucket = %d, less_elems =  %ul\n", thread_num, less_bucks_counter);
        }
    }
    return less_bucks_counter;
}


void fillBufferWithSortedValues(int* buffer, Node** buckets, int thread_num, int less_bucks_counter)
{
    if(buckets[thread_num]!=NULL)
    {
        Node* it = buckets[thread_num];
        while(it!=NULL)
        {
            buffer[less_bucks_counter] = it->data;
            less_bucks_counter += 1;
            it = it->next;
        }
        //printf("Thread = %d last index = %d\n", thread_num, less_bucks_counter-1);
    }
}

int main(int argc, char** argv)
{
    int K = atoi(argv[1]);
    int threads_num = K;
    int* buffer = (int*)calloc(SIZE, sizeof(int));
    double times[6] = { 0 };
    times[0] = omp_get_wtime();
    generateData(buffer);
    times[1] = omp_get_wtime();
    int max = findMax(buffer);
    int min = findMin(buffer);
    int range = max - min;
    int BUCKET_SIZE = range / K;

    Node** buckets = (Node**)calloc(K, sizeof(Node*));
    long* buckets_elems_counter = (long*)calloc(K, sizeof(long));

    int thread_num;
    int start_idx = K;

    #pragma omp parallel shared(buffer,buckets,buckets_elems_counter,BUCKET_SIZE,K,start_idx) \  
            private(thread_num) num_threads(K) 
    {
        int buck_counter = 0;
        thread_num = omp_get_thread_num();
        int priv_start_idx;

        #pragma omp critical
        {
            priv_start_idx = start_idx;
            start_idx--;
        }

        #pragma omp barrier
        #pragma omp single
        {
            times[2] = omp_get_wtime();
        }
        
        splitToBuckets(buffer, buckets, thread_num, priv_start_idx, BUCKET_SIZE, min, K, &buck_counter);

        #pragma omp barrier
        #pragma omp single
        {
            times[3] = omp_get_wtime();
        }
        
        buckets_elems_counter[thread_num] = buck_counter;
        
        quickSort(&buckets[thread_num]); 

        #pragma omp barrier
        #pragma omp single
        {
            times[4] = omp_get_wtime();
        }

        int less_bucks_counter = countElemsInPrevBuckets(buckets_elems_counter, thread_num);
    
        fillBufferWithSortedValues(buffer, buckets, thread_num, less_bucks_counter);

        #pragma omp barrier
        #pragma omp single
        {
            times[5] = omp_get_wtime();
        }

    }

    
    int i;
    for(i=0;i<SIZE;++i)
    {
      printf("Elem = %d\n", buffer[i]);  
    }
    if(PRINT)
    {
        printf("Data generation time = %.7f s\n", times[1]-times[0]);
        printf("Buckets split time = %.7f s\n", times[3]-times[2]);
        printf("Buckets sorting time = %.7f s\n", times[4]-times[3]);
        printf("Sorted buffer filling time = %.7f s\n", times[5]-times[4]);
        printf("Overall execution time = %.7f s\n", times[5]-times[0]);
    }
    

    free(buffer);
    free(buckets);
    free(buckets_elems_counter);
    return 0;
}