#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "list_utils.h"

#define SIZE 10000000
#define PRINT 1


typedef struct thread_buck_info
{
  int  bucks_num;
  int* buck_inds;
  int* buck_el_count;
} thread_buck_info;


void generateData(int* buffer, int threads_num)
{
    int i;
    #pragma omp parallel num_threads(threads_num)
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

int isElemServedInNthThread(int bucket, int nth_thread, int all_threads)
{
    return (bucket % all_threads) == nth_thread;
}

int getBucketIndexFromThreadBuckInfo(thread_buck_info thread_info, int _bucket)
{
    int size = thread_info.bucks_num;
    int z;
    for(z=0;z<size;++z)
    {
        if(thread_info.buck_inds[z] == _bucket)
            return z;
    }

    return size;
}

void splitToBuckets(int* buffer, Node** buckets, int thread_num, int all_threads, int priv_start_idx,
                    int BUCKET_SIZE, int min_elem, int K, thread_buck_info* threads_buck_infos)
{
    int i;
    for(i=priv_start_idx;i<SIZE;++i)
    {
        int offset_elem = buffer[i] - min_elem;
        int bucket = (offset_elem / BUCKET_SIZE);
        if(bucket == K) bucket = K - 1;
        if(isElemServedInNthThread(bucket, thread_num, all_threads))
        {
            
            push(&buckets[bucket], buffer[i]);
            int curr_size = threads_buck_infos[thread_num].bucks_num;
            int ind = getBucketIndexFromThreadBuckInfo(threads_buck_infos[thread_num], bucket); 
            
            if(curr_size == ind)
            {
                threads_buck_infos[thread_num].buck_inds[ind] = bucket;
                threads_buck_infos[thread_num].bucks_num += 1;
            }
            threads_buck_infos[thread_num].buck_el_count[ind] += 1;
        }
    }

    #pragma omp critical
    {
        for(i=priv_start_idx;i>=0;i--)
        {
            int offset_elem = buffer[i] - min_elem;
            int bucket = (offset_elem / BUCKET_SIZE);
            if(isElemServedInNthThread(bucket, thread_num, all_threads))
            {
                if(bucket == K) bucket = K - 1;
                push(&buckets[bucket], buffer[i]);
                int curr_size = threads_buck_infos[thread_num].bucks_num;
                int ind = getBucketIndexFromThreadBuckInfo(threads_buck_infos[thread_num], bucket); 
                
                if(curr_size == ind)
                {
                    threads_buck_infos[thread_num].buck_inds[ind] = bucket;
                    threads_buck_infos[thread_num].bucks_num += 1;
                }
                threads_buck_infos[thread_num].buck_el_count[ind] += 1;
            }
        }
    }
}

void sortValuesInBuckets(Node** buckets, thread_buck_info threads_bucks_info)
{
    int size = threads_bucks_info.bucks_num;
    int f = 0;
    for(f=0;f<size;++f)
    {
        int ind = threads_bucks_info.buck_inds[f];
        quickSort(&buckets[ind]); 
    }
}

void writeNumOfElemsFromBucks(long* buckets_elems_counter, thread_buck_info thread_buck_info)
{
    int r;
    int size = thread_buck_info.bucks_num;
    
    for(r=0;r<size;++r)
    {
        int ind = thread_buck_info.buck_inds[r];
        (buckets_elems_counter[ind]) = thread_buck_info.buck_el_count[r];
    }
}


void countElemsInPrevBuckets(long* buckets_elems_counter, int* less_bucks_counters, thread_buck_info thread_buck_info)
{
    int n = 0;
    int size = thread_buck_info.bucks_num;
    for(n=0;n<size;++n)
    {
        int less_counter = 0;
        int x;
        int buck = thread_buck_info.buck_inds[n];
        for(x=0;x<buck;++x)
        {
           less_counter += buckets_elems_counter[x];  
        }
        less_bucks_counters[buck] = less_counter;
    }
}


void fillBufferWithSortedValues(int* buffer, Node** buckets, int* less_bucks_counters, thread_buck_info thread_buck_info)
{
    int n = 0;
    int size = thread_buck_info.bucks_num;
    for(n=0;n<size;++n)
    {
        int buck = thread_buck_info.buck_inds[n];
        if(buckets[buck]!=NULL)
        {
            Node* it = buckets[buck];
            int b_el_ind = less_bucks_counters[buck]; 
            while(it!=NULL)
            {   
                buffer[b_el_ind] = it->data;
                b_el_ind += 1;
                it = it->next;
            }
        }
    }
}

int main(int argc, char** argv)
{
    int K = atoi(argv[1]);
    int threads_num = atoi(argv[2]);

    int* buffer = (int*)calloc(SIZE, sizeof(int));
    const int MAX_BUCKS_PER_THREAD = (K>threads_num) ? K/threads_num + 1 : K;
    thread_buck_info* threads_bucks_infos = (thread_buck_info*)calloc(threads_num, sizeof(thread_buck_info)); 
    int j;
    for(j=0;j<threads_num;++j)
    {
        threads_bucks_infos[j].buck_inds = (int*)calloc(MAX_BUCKS_PER_THREAD, sizeof(int));
        threads_bucks_infos[j].buck_el_count = (int*)calloc(MAX_BUCKS_PER_THREAD, sizeof(int));
    }
    double times[6] = { 0 };
    times[0] = omp_get_wtime();
    generateData(buffer, threads_num);
    times[1] = omp_get_wtime();
    int max = findMax(buffer);
    int min = findMin(buffer);
    int range = max - min;
    int BUCKET_SIZE = range / K;

    Node** buckets = (Node**)calloc(K, sizeof(Node*));
    long* buckets_elems_counter = (long*)calloc(K, sizeof(long));
    int* less_bucks_counters = (int*)calloc(K, sizeof(int));

    int thread_num;
    int start_idx = K;

    #pragma omp parallel shared(buffer,buckets,buckets_elems_counter,less_bucks_counters,threads_bucks_infos,BUCKET_SIZE,K,start_idx) \  
            private(thread_num) num_threads(threads_num) 
    {
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
        
        splitToBuckets(buffer, buckets, thread_num, threads_num, priv_start_idx, BUCKET_SIZE, min, K, threads_bucks_infos);

        #pragma omp barrier
        #pragma omp single
        {
            times[3] = omp_get_wtime();
        }
        
        sortValuesInBuckets(buckets, threads_bucks_infos[thread_num]);

        #pragma omp barrier
        #pragma omp single
        {
            times[4] = omp_get_wtime();
        }

        writeNumOfElemsFromBucks(buckets_elems_counter, threads_bucks_infos[thread_num]);
        #pragma omp barrier
        countElemsInPrevBuckets(buckets_elems_counter, less_bucks_counters, threads_bucks_infos[thread_num]);
        #pragma omp barrier
        fillBufferWithSortedValues(buffer, buckets, less_bucks_counters, threads_bucks_infos[thread_num]);

        #pragma omp barrier
        #pragma omp single
        {
            times[5] = omp_get_wtime();
        }

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
    free(less_bucks_counters);
    free(threads_bucks_infos);
    return 0;
}