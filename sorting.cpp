#include<iostream>
#include<omp.h>
#include<ctime>
#include<cstdlib>
#include<chrono>

using namespace std;
using namespace std::chrono;

void bubble_sort(int arr[],int n)
{
  for(int i=0;i<n-1;++i)
  {
    for(int j=0;j<n-i-1;++j)
    {
      if(arr[j]>arr[j+1])
      {
        swap(arr[j],arr[j+1]);
      }
    }
  }
}

void merge(int arr[],int l,int m,int r)
{
    int i,j,k;
    int n1=m-l+1;
    int n2=r-m;
    
    int* L= new int[n1];
    int* R=new int[n2];
    
    for(int i=0;i<n1;++i)
    {
        L[i]=arr[l+i];
    }
    for(int j=0;j<n2;++j)
    {
       R[j]=arr[m+1+j];
    }  
    
    i=0;
    j=0;
    k=l;
    
    while(i<n1 && j<n2)
    {
      if(L[i]<=R[j])
      {
        arr[k]=L[i];
        ++i;
      }
      else
      {
        arr[k]=R[j];
        ++j;
      }
      ++k;
    }
    
    while(i<n1)
    {
      arr[k]=L[i];
      ++i;
      ++k;
    }
    
    while(i<n2)
    {
      arr[k]=R[j];
      ++j;
      ++k;
    }
  delete[] L;
  delete[] R;
}

void merge_sort(int arr[],int l,int r)
{
  
    if(l<r)
    {
      int m=l+(r-l)/2;
      #pragma omp parallel sections
      {
        #pragma omp section
        {
          merge_sort(arr,l,m);
        }
        #pragma omp section
        {
          merge_sort(arr,m+1,r);
        }
      }
     merge(arr,l,m,r);
    }
}

void print(int arr[],int n)
{
  for(int i=0;i<n;++i)
  {
    cout<<arr[i]<<" ";
  }
}

int main()
{
  int n;
  cout<<"enter the number o array";
  cin>>n;
  
  int* arr=new int[n];
  
  for(int i=0;i<n;++i)
  {
    cin>>arr[i];
  }
  
  cout<<"original array";
  print(arr,n);
  
  auto start=high_resolution_clock::now();
  auto end=high_resolution_clock::now();
  auto duration=duration_cast<microseconds>(end-start);
  
  start=high_resolution_clock::now();
  bubble_sort(arr,n);
  end=high_resolution_clock::now();
  duration=duration_cast<microseconds>(end-start);
  cout<<"\nSequential Bubble sort:"<< " "<<endl;
  print(arr,n);
  cout<<"\nDuration: "<<duration.count()<<"ms";
  
  start=high_resolution_clock::now();
    #pragma omp parallel
  {
    #pragma omp single
    {
      bubble_sort(arr,n);
    }
  }
  end=high_resolution_clock::now();
  duration=duration_cast<microseconds>(end-start);
  cout<<"\nParallel Bubble sort:"<< " "<<endl;
  print(arr,n);
  cout<<"\nDuration: "<<duration.count()<<"ms"<<endl;
  
  
  
  start=high_resolution_clock::now();
  merge_sort(arr,0,n-1);
  end=high_resolution_clock::now();
  duration=duration_cast<microseconds>(end-start);
  cout<<"\nSequential merge sort"<<" "<<endl;
  print(arr,n);
  cout<<"\nTime Taken:"<<duration.count()<<"ms\n"<<endl;
  
  
  start=high_resolution_clock::now();
  #pragma omp parallel
  {
    #pragma omp single
    {
      merge_sort(arr,0,n-1);
    }
  }
  end=high_resolution_clock::now();
  duration=duration_cast<microseconds>(end-start);
  cout<<"\nParallel merge sort"<<" "<<endl;
  print(arr,n);
  cout<<"\nTime Taken:"<<duration.count()<<endl;
  
  
  delete[]arr;
  
}
