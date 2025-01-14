#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <papi.h>
#include <omp.h>

using namespace std;

#define SYSTEMTIME clock_t

 
void OnMult(int m_ar, int m_br) 
{
    
    SYSTEMTIME Time1, Time2;
    
    char st[100];
    double temp;
    int i, j, k;

    double *pha, *phb, *phc;
    

        
    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

    for(i=0; i<m_ar; i++)
        for(j=0; j<m_ar; j++)
            pha[i*m_ar + j] = (double)1.0;



    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);



    Time1 = clock();

    // we add the intermediate values of the dot product 
    // to the 'temp' variable (which works as an accumulator)
    for(i=0; i<m_ar; i++)
    {   for( j=0; j<m_br; j++)
        {   temp = 0;
            for( k=0; k<m_ar; k++)
            {   
                temp += pha[i*m_ar+k] * phb[k*m_br+j];
            }
            phc[i*m_ar+j]=temp;
        }
    }


    Time2 = clock();
    sprintf(st, "Time: %3.3f seconds\n", (double)(Time2 - Time1) / CLOCKS_PER_SEC);
    cout << st;

    // display 10 elements of the result matrix tto verify correctness
    cout << "Result matrix: " << endl;
    for(i=0; i<1; i++)
    {   for(j=0; j<min(10,m_br); j++)
            cout << phc[j] << " ";
    }
    cout << endl;

    free(pha);
    free(phb);
    free(phc);
    
    
}

// add code here for line x line matriz multiplication
void OnMultLine(int m_ar, int m_br)
{
    SYSTEMTIME Time1, Time2;
    
    char st[100];
    //double temp;
    int i, j, k;

    double *pha, *phb, *phc;

    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

    for(i=0; i<m_ar; i++)
        for(j=0; j<m_ar; j++)
            pha[i*m_ar + j] = (double)1.0;

    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);
    
    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phc[i*m_br + j] = (double)0.0;

    Time1 = clock();

    // same as previous one, but with a slight difference
    // but here we store the dot produt value directly in the matrix
    // (instead of firstly keeping the intermediate values in an auxiliar variable)
    for (i = 0; i<m_ar;i++){
        for(k=0;k<m_ar;k++){
            for (j=0;j<m_br;j++){
                phc[i*m_ar+j] += pha[i*m_ar+k] * phb[k*m_br+j];
            }
        }
    }

     Time2 = clock();
    sprintf(st, "Time: %3.3f seconds\n", (double)(Time2 - Time1) / CLOCKS_PER_SEC);
    cout << st;

    // display 10 elements of the result matrix tto verify correctness
    /*cout << "Result matrix: " << endl;
    for(i=0; i<1; i++)
    {   for(j=0; j<min(10,m_br); j++)
            cout << phc[j] << " ";
    }*/
    cout << endl;

    free(pha);
    free(phb);
    free(phc);
 
}

// add code here for block x block matriz multiplication
void OnMultBlock(int m_ar, int m_br, int blockSize)
{
    SYSTEMTIME Time1, Time2;
    
    char st[100];
    //double temp;
    int i, j, k, l, m, n;

    double *pha, *phb, *phc;

    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

    for(i=0; i<m_ar; i++)
        for(j=0; j<m_ar; j++)
            pha[i*m_ar + j] = (double)1.0;

    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);
    
    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phc[i*m_br + j] = (double)0.0;

    Time1 = clock();

    /*
    for (int i0=0; i<m_ar;i+=bkSize){
        for(int j0=0;j<m_br;j+=bkSize){
            for(int k0=0; k<m_ar;k+=bkSize){
                for (int i = i0; i < std::min(i0 + bkSize, m_ar); ++i) {
                    for (int j = j0; j < std::min(j0 + bkSize, m_ar); ++j) {
                        for (int k = k0; k < std::min(k0 + bkSize, m_ar); ++k) {
                            phc[i*m_ar+j] += pha [i*m_ar+k] * phb [k*m_br+j];

                        }
                    }
                }
            }               
        }

    }
    */

    // these nested loops implement block-wise matrix multiplication, 
    // where each block of the resulting matrix ⁠ phc⁠ is computed by 
    // performing smaller block-wise multiplications of corresponding 
    // blocks of matrices⁠ pha⁠ and⁠ phb
    // ex: result matrix 9x9
    // first block (3x3) composed by rows 1 to 3 and cols 1 to 3 correspond to the 
    // 3x3 matrix multiplication of the same rows and cols of matrices pha and phb
    for(i=0; i<m_ar; i += blockSize)
    {
        for(j=0; j<m_br; j += blockSize)
        {
            for(k=0; k<m_ar; k += blockSize)
            {
                for(l = i; l<min(i + blockSize, m_ar); l++)
                {
                    for(m = k; m<min(k + blockSize, m_br); m++)
                    {
                        for (n = j; n<min(j + blockSize, m_ar); n++)
                        {
                            phc[m_ar*l + n] += pha[m_ar*l + m] * phb[m_ar*m + n];
                        }
                    }
                }
            }
        }
    }
    Time2 = clock();
    sprintf(st, "Time: %3.3f seconds\n", (double)(Time2 - Time1) / CLOCKS_PER_SEC);
    cout << st;

    // display 10 elements of the result matrix to verify correctness
    /*cout << "Result matrix: " << endl;
    for(i=0; i<1; i++)
    {    for(j=0; j<min(10,m_br); j++)
            cout << phc[j] << " ";
    }*/
    cout << endl;

    free(pha);
    free(phb);
    free(phc);
}

// add code here for line x parallel line matriz multiplication
void OnParallelMultLine1(int m_ar, int m_br)
{    
    char st[100];
    //double temp;
    int i, j, k;

    double *pha, *phb, *phc;

    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

    for(i=0; i<m_ar; i++)
        for(j=0; j<m_ar; j++)
            pha[i*m_ar + j] = (double)1.0;

    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);
    
    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phc[i*m_br + j] = (double)0.0;

    double start = omp_get_wtime();

    // This directive creates a set of threads, in which each thread is 
    // responsible for executing a block of code in parallel.
    // Thus, we can save some time when executing the program

    // In general, the approach used in OnParallelMultLine1 might be faster because it involves fewer overheads
    #pragma omp parallel for
    for (int i=0; i<m_ar; i++) {
        for (int k=0; k<m_ar; k++) {
            for (int j=0; j<m_ar; j++) {
                phc[i*m_ar+j] += pha[i*m_ar+k] * phb[k*m_br+j];
            }
        }
    }

    double end = omp_get_wtime();
    sprintf(st, "Time: %3.3f seconds\n", (double)(end - start));
    cout << st;

    // display 10 elements of the result matrix tto verify correctness
    cout << "Result matrix: " << endl;
    for(i=0; i<1; i++)
    {   for(j=0; j<min(10,m_br); j++)
            cout << phc[j] << " ";
    }
    cout << endl;

    free(pha);
    free(phb);
    free(phc);
 
}

// add code here for line x parallel line matriz multiplication
void OnParallelMultLine2(int m_ar, int m_br)
{
    SYSTEMTIME Time1, Time2;
    
    char st[100];
    //double temp;
    int i, j, k;

    double *pha, *phb, *phc;

    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

    for(i=0; i<m_ar; i++)
        for(j=0; j<m_ar; j++)
            pha[i*m_ar + j] = (double)1.0;

    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phb[i*m_br + j] = (double)(i+1);
    
    for(i=0; i<m_br; i++)
        for(j=0; j<m_br; j++)
            phc[i*m_br + j] = (double)0.0;

    double start = omp_get_wtime();

    #pragma omp parallel
    for (int i=0; i<m_ar; i++) {
        for (int k=0; k<m_ar; k++) {
            // This directive distributes the loop iterations of the inner loop  
            // across the threads created with the previous directive
            #pragma omp for
            for (int j=0; j<m_ar; j++) {
                phc[i*m_ar+j] += pha[i*m_ar+k] * phb[k*m_br+j];
            }
        }
    }

    double end = omp_get_wtime();
    sprintf(st, "Time: %3.3f seconds\n", (double)(end - start));
    cout << st;

    // display 10 elements of the result matrix tto verify correctness
    cout << "Result matrix: " << endl;
    for(i=0; i<1; i++)
    {   for(j=0; j<min(10,m_br); j++)
            cout << phc[j] << " ";
    }
    cout << endl;

    free(pha);
    free(phb);
    free(phc);
 
}

void handle_error (int retval)
{
  printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
  exit(1);
}

void init_papi() {
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT && retval < 0) {
    printf("PAPI library version mismatch!\n");
    exit(1);
  }
  if (retval < 0) handle_error(retval);

  std::cout << "PAPI Version Number: MAJOR: " << PAPI_VERSION_MAJOR(retval)
            << " MINOR: " << PAPI_VERSION_MINOR(retval)
            << " REVISION: " << PAPI_VERSION_REVISION(retval) << "\n";
}


int main (int argc, char *argv[])
{

    char c;
    int lin, col, blockSize;
    int op;
    
    int EventSet = PAPI_NULL;
    long long values[2];
    int ret;
    

    ret = PAPI_library_init( PAPI_VER_CURRENT );
    if ( ret != PAPI_VER_CURRENT )
        std::cout << "FAIL" << endl;


    ret = PAPI_create_eventset(&EventSet);
        if (ret != PAPI_OK) cout << "ERROR: create eventset" << endl;


    ret = PAPI_add_event(EventSet,PAPI_L1_DCM );
    if (ret != PAPI_OK) cout << "ERROR: PAPI_L1_DCM" << endl;


    ret = PAPI_add_event(EventSet,PAPI_L2_DCM);
    if (ret != PAPI_OK) cout << "ERROR: PAPI_L2_DCM" << endl;


    op=1;
    do {
        cout << endl << "1. Multiplication" << endl;
        cout << "2. Line Multiplication" << endl;
        cout << "3. Block Multiplication" << endl;
        cout << "4. OnParallel MultiLine1" << endl;
        cout << "5. OnParallel MultiLine2" << endl;
        cout << "Selection?: ";
        cin >>op;
        if (op == 0)
            break;

        switch (op){
            case 1: 
                for (int i = 600; i < 3400; i+=400) {

                    cout << "Matrix size: " << i << "x" << i << endl;

                    // Start counting
                    ret = PAPI_start(EventSet);
                    if (ret != PAPI_OK) cout << "ERROR: Start PAPI" << endl;

                    OnMult(i, i);

                    ret = PAPI_stop(EventSet, values);
                    if (ret != PAPI_OK) cout << "ERROR: Stop PAPI" << endl;
                    printf("L1 DCM: %lld \n",values[0]);
                    printf("L2 DCM: %lld \n",values[1]);

                    ret = PAPI_reset( EventSet );
                    if ( ret != PAPI_OK )
                        std::cout << "FAIL reset" << endl; 
                }
                break;
            case 2:
                /*
                for (int i = 600; i < 3400; i+=400) {
                    cout << "\nMatrix size: " << i << "x" << i << endl;

                    // Start counting
                    ret = PAPI_start(EventSet);
                    if (ret != PAPI_OK) cout << "ERROR: Start PAPI" << endl;

                    OnMultLine(i, i);

                    ret = PAPI_stop(EventSet, values);
                    if (ret != PAPI_OK) cout << "ERROR: Stop PAPI" << endl;
                    printf("L1 DCM: %lld \n",values[0]);
                    printf("L2 DCM: %lld \n",values[1]);

                    ret = PAPI_reset( EventSet );
                    if ( ret != PAPI_OK )
                        std::cout << "FAIL reset" << endl;
                }
                
                */
                for (int i = 4096; i < 10241; i+=2048) {

                    cout << "Matrix size: " << i << "x" << i << endl;

                    // Start counting
                    ret = PAPI_start(EventSet);
                    if (ret != PAPI_OK) cout << "ERROR: Start PAPI" << endl;

                    OnMultLine(i, i);

                    ret = PAPI_stop(EventSet, values);
                    if (ret != PAPI_OK) cout << "ERROR: Stop PAPI" << endl;
                    printf("L1 DCM: %lld \n",values[0]);
                    printf("L2 DCM: %lld \n",values[1]);

                    ret = PAPI_reset( EventSet );
                    if ( ret != PAPI_OK )
                        std::cout << "FAIL reset" << endl;
                }
                
                break;
            case 3:
                //for (int j = 0; j < 3; j++) {
                    for (int i = 4096; i < 10241; i+=2048) {
                        cout << "Matrix size: " << i << "x" << i << " Block size: " << 128 << endl;

                        // Start counting
                        ret = PAPI_start(EventSet);
                        if (ret != PAPI_OK) cout << "ERROR: Start PAPI" << endl;

                        OnMultBlock(i, i, 128);

                        ret = PAPI_stop(EventSet, values);
                        if (ret != PAPI_OK) cout << "ERROR: Stop PAPI" << endl;
                        printf("L1 DCM: %lld \n",values[0]);
                        printf("L2 DCM: %lld \n",values[1]);

                        ret = PAPI_reset( EventSet );
                        if ( ret != PAPI_OK )
                            std::cout << "FAIL reset" << endl;
                    }
                    //blockSize = blockSize * 2;
                //}
                break;
            case 4:
                for (int i = 600; i < 3400; i+=400) {

                    cout << "Matrix size: " << i << "x" << i << endl;

                    // Start counting
                    ret = PAPI_start(EventSet);
                    if (ret != PAPI_OK) cout << "ERROR: Start PAPI" << endl;

                    OnParallelMultLine1(i, i);

                    ret = PAPI_stop(EventSet, values);
                    if (ret != PAPI_OK) cout << "ERROR: Stop PAPI" << endl;
                    printf("L1 DCM: %lld \n",values[0]);
                    printf("L2 DCM: %lld \n",values[1]);

                    ret = PAPI_reset( EventSet );
                    if ( ret != PAPI_OK )
                        std::cout << "FAIL reset" << endl; 
                }
                break;
            case 5: 
                for (int i = 600; i < 3400; i+=400) {

                    cout << "Matrix size: " << i << "x" << i << endl;

                    // Start counting
                    ret = PAPI_start(EventSet);
                    if (ret != PAPI_OK) cout << "ERROR: Start PAPI" << endl;

                    OnParallelMultLine2(i, i);

                    ret = PAPI_stop(EventSet, values);
                    if (ret != PAPI_OK) cout << "ERROR: Stop PAPI" << endl;
                    printf("L1 DCM: %lld \n",values[0]);
                    printf("L2 DCM: %lld \n",values[1]);

                    ret = PAPI_reset( EventSet );
                    if ( ret != PAPI_OK )
                        std::cout << "FAIL reset" << endl; 
                } 
                break;
            default:
                cout << "Choose a valid option.\n";
                break;
        }



    }while (op != 0);

    ret = PAPI_remove_event( EventSet, PAPI_L1_DCM );
    if ( ret != PAPI_OK )
        std::cout << "FAIL remove event" << endl; 

    ret = PAPI_remove_event( EventSet, PAPI_L2_DCM );
    if ( ret != PAPI_OK )
        std::cout << "FAIL remove event" << endl; 

    ret = PAPI_destroy_eventset( &EventSet );
    if ( ret != PAPI_OK )
        std::cout << "FAIL destroy" << endl;

}
