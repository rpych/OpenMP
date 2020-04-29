#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "list_utils.h"

#define SIZE 1000

// typedef struct Node
// {
//     int data;
//     struct Node *next;
// } Node;


void generateData(int* buffer)
{
    int i;
    #pragma omp parallel 
    {
    unsigned int seed = ((unsigned int) omp_get_wtime()) + omp_get_thread_num();
    #pragma omp for private(i)
    for(i=0;i<SIZE;i++)
    {
        
        buffer[i] = rand_r(&seed) % 1000;
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


int main(int argc, char** argv)
{
    int K = 10;//atoi(argv[1]);
    int threads_num = K;
    //printf("K= %d\n", K);
    int* buffer = (int*)calloc(SIZE, sizeof(int));
    generateData(buffer);
    int max = findMax(buffer);
    int min = findMin(buffer);
    int range = max - min;
    int BUCKET_SIZE = range / K;
    printf("MAX = %d, MIN = %d, BUCKET_SIZE = %d\n", max, min, BUCKET_SIZE);
    printf("%d|%d|%d|%d|%d\n", buffer[SIZE-1], buffer[SIZE-2], buffer[SIZE-3],buffer[SIZE-4],buffer[SIZE-5]);
    //main part

    Node** buckets = (Node**)calloc(K, sizeof(Node*));
    long* buckets_elems_counter = (long*)calloc(K, sizeof(long));

    int thread_num;
    int i; 
    int buck_counter;
    long less_bucks_counter;

    #pragma omp parallel shared(buffer,buckets,buckets_elems_counter,BUCKET_SIZE,K) \ 
            private(thread_num,i,buck_counter,less_bucks_counter) num_threads(K)
    {
        buck_counter = 0;
        less_bucks_counter = 0;
        thread_num = omp_get_thread_num();
        for(i=(K-thread_num);i<SIZE;++i)
        {
            if(isElemInNthBucket(buffer[i],thread_num,BUCKET_SIZE,min,K))
            {
                push(&buckets[thread_num], buffer[i]);
                buck_counter++;
            }
        }
        
        printf("Byłem tu %d\n", thread_num);
        #pragma omp critical
        {
            thread_num = omp_get_thread_num();
            for(i=(K-thread_num-1);i>=0;i--)
            {
                if(isElemInNthBucket(buffer[i],thread_num,BUCKET_SIZE,min,K))
                {
                    push(&buckets[thread_num], buffer[i]);
                    buck_counter++;
                }
            }
        }
        buckets_elems_counter[thread_num] = buck_counter;
        #pragma omp barrier
        quickSort(&buckets[thread_num]); 
        if(thread_num == 0)
        {
            less_bucks_counter = 0;
        }
        else{
            //#pragma omp barrier
            for(i=0;i<thread_num;++i)
            {
                less_bucks_counter += buckets_elems_counter[i];
                if(i==thread_num-1)
                    printf("Bucket = %d, less_elems =  %ul\n", thread_num, less_bucks_counter);
            }
        }

        #pragma omp barrier
    
        if(buckets[thread_num]!=NULL)
        {
            Node* it = buckets[thread_num];
            while(it!=NULL)
            {
                buffer[less_bucks_counter] = it->data;
                less_bucks_counter += 1;
                it = it->next;
            }
            printf("Thread = %d last index = %d\n", thread_num, less_bucks_counter-1);
        }

    }

    
    
    // for(i=0;i<K;i++){
    //     if(buckets[i]!=NULL){
    //         Node* it = buckets[i];
    //         while(it!=NULL){
    //             printf("Bucket nr %d and elem = %d\n", i, (it->data));
    //             it = it->next;
    //         }
    //     }
            
    // }

    for(i=0;i<SIZE;++i)
    {
      printf("Elem = %d\n", buffer[i]);  
    }


    //printf("10==10 | %d\n", (10==10));

    free(buffer);
    free(buckets);
    return 0;
}