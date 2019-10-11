#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<complex.h>
#include<string.h>

#define UPPER_BOUND 10e20
#define LOWER_BOUND 0.000001
#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds
#define MAX_ITERATION 52
#define NORM_BOUND 0.001

pthread_mutex_t item_done_mutex;
int n_lines, n_threads, deg_func;
char * item_done;
int ** attractors;
int ** convergences;

void * compute_main(void * args);
void * write_main(void * args);

int main(int argc, char *argv[])
{
  if (argc < 4)
    {
      printf("Provide 3 arguments\n");
      exit(-1);
    }

  printf("Assignment 2\n\n");
  char t = 't';
  int base = 10;
  char alph1 = *(argv[1]+1); 
  if(alph1 == t)
    {
      n_threads = strtol(argv[1]+2,NULL,base);
      n_lines = strtol(argv[2]+2, NULL, base);
      deg_func = atoi(argv[3]);
    }

  else
    {
      n_threads = strtol(argv[2]+2, NULL, base);
      n_lines = strtol(argv[1]+2, NULL, base);
      deg_func = atoi(argv[3]);
    }

  // Allocation of global variables
  item_done = (char *) malloc(sizeof(char) * n_lines);
  attractors = (int **) malloc(sizeof(int *) * n_lines);
  convergences = (int **) malloc(sizeof(int *) * n_lines);

  // Initialisation of mutex
  pthread_mutex_init(&item_done_mutex, NULL);

  // creation of compute threads and write thread
  // dynamic allocation of number of threads
  pthread_t * compute_threads = (pthread_t * ) malloc(sizeof(pthread_t) * n_threads); 
  pthread_t write_thread;

  int ret;
  size_t tx; 

  for ( tx = 0; tx < n_threads; ++tx )
    {
      size_t * args = malloc(sizeof(size_t));
      *args = tx;

      if (ret = pthread_create(&compute_threads[tx], NULL, compute_main, (void*)args)) 
  {
    printf("Error creating compute thread: %d\n", ret);
    exit(1);
  }
    }
  if (ret = pthread_create(&write_thread, NULL, write_main, NULL))
    {
      printf("Error creating write thread: %d\n", ret);
      exit(1);
    }

  // Joining of compute threads
  for ( tx=0; tx < n_threads; ++tx )
    {
      if (ret = pthread_join(compute_threads[tx], NULL))
  {
    printf("Error joining thread: %d\n", ret);
    exit(1);
  }
    }

  // Joining of write thread
  if (ret = pthread_join(write_thread, NULL))
    {
      printf("Error joining thread: %d\n", ret);
      exit(1);
    }

  // Destruction of the mutex
  pthread_mutex_destroy(&item_done_mutex);

  free(compute_threads);
  free(item_done);
  free(attractors);
  free(convergences);
  printf("Execution done!\n");
}


// Computation of Newton Iteration - compute_main()
void * compute_main(void * args)
{
  size_t offset = *((size_t*)args);
  free(args);

  // Creating a matrix that would hold the complex initialisations
  double h = 4/(double)n_lines;
  double * x = (double *) malloc(sizeof(double) * n_lines);
  double * y = (double *) malloc(sizeof(double) * n_lines);

  x[0] = -2; y[0] = 2;

  for(int i = 1; i<n_lines; ++i){
    x[i] = x[i-1] + h;
    y[i] = y[i-1] - h;
  }

  // Switch Cases with Newtons Iterations and Checking
  double complex roots[deg_func-1];

  switch(deg_func)
    {

    case 1:
      //STATEMENTS FOR DEGREE 1
      roots[0] =  1;
      break;

    case 2:
      //STATEMENTS FOR DEGREE 2
      roots[0] =  1;
      roots[1] =  -1;
      break;


    case 3:
      //STATEMENTS FOR DEGREE 3
      roots[0] =  1;
      roots[1] =  -0.500000000000000 - 0.866025403784439 *I;
      roots[2] =  -0.500000000000000 + 0.866025403784439 *I;
      break;

    case 4:
      //STATEMENTS FOR DEGREE 4
      roots[0] =  1;
      roots[1] =  -1;
      roots[2] =  0+1*I;
      roots[3] =  0-1*I;
      break;

    case 5:
      //STATEMENTS FOR DEGREE 5
      roots[0] =  1;
      roots[1] =  -0.809016994374947 - 0.587785252292473 *I;
      roots[2] =  0.309016994374947 + 0.951056516295154 *I;
      roots[3] =  0.309016994374947 - 0.951056516295154 *I;
      roots[4] =  -0.809016994374947 + 0.587785252292473 *I;
      break;

    case 6:
      //STATEMENTS FOR DEGREE 6
      roots[0] =  1;
      roots[1] =  -1;
      roots[2] =  -0.500000000000000 - 0.866025403784439 *I;
      roots[3] =  0.500000000000000 + 0.866025403784439 *I;
      roots[4] =  0.500000000000000 - 0.866025403784439 *I;
      roots[5] =  -0.500000000000000 + 0.866025403784439 *I;
      break;

    case 7:
      //STATEMENTS FOR DEGREE 7
      roots[0] =  1;
      roots[1] =  -0.900968867902419 - 0.433883739117558 *I;
      roots[2] =  0.623489801858734 + 0.781831482468030 *I;
      roots[3] =  -0.222520933956314 - 0.974927912181824 *I;
      roots[4] =  -0.222520933956314 + 0.974927912181824 *I;
      roots[5] =  0.623489801858734 - 0.781831482468030 *I;
      roots[6] =  -0.900968867902419 + 0.433883739117558 *I;
      break;

    case 8:
      //STATEMENTS FOR DEGREE 8
      roots[0] =  1;
      roots[1] =  -1;
      roots[2] =  0+1*I;
      roots[3] =  0-1*I;
      roots[4] =  -0.707106781186548 - 0.707106781186548 *I;
      roots[5] =  0.707106781186548 + 0.707106781186548 *I;
      roots[6] =  0.707106781186548 - 0.707106781186548 *I;
      roots[7] =  -0.707106781186548 + 0.707106781186548 *I;
      break;

    case 9:
      //STATEMENTS FOR DEGREE 9
      roots[0] =  1;
      roots[1] =  -0.939692620785908 - 0.342020143325669 *I;
      roots[2] =  0.766044443118978 + 0.642787609686539 *I;
      roots[3] =  -0.500000000000000 - 0.866025403784439 *I;
      roots[4] =  0.173648177666930 + 0.984807753012208 *I;
      roots[5] =  0.173648177666930 - 0.984807753012208 *I;
      roots[6] =  -0.500000000000000 + 0.866025403784439 *I;
      roots[7] =  0.766044443118978 - 0.642787609686539 *I;
      roots[8] =  -0.939692620785908 + 0.342020143325669 *I;
      break;

    default:
      fprintf(stderr, "unexpected degree\n");
      exit(1);    

    }

  //BEGINNING ITERATION FOR NEWTONS COMPUTATION

  double complex xk;   
  double complex x_k1;    
  double complex xdr;   

  int conv;   //no. of iterations for convergence
  int attr;   //root the iteration converges to

  size_t conv_x;
  size_t attr_x;

  for ( size_t ix = offset; ix < n_lines; ix += n_threads )
    {

      // Attractor and convergence
      int * attractor = (int *) malloc(sizeof(int) * n_lines);
      int * convergence = (int *) malloc(sizeof(int) * n_lines);

      for ( size_t cx = 0; cx < n_lines; ++cx ) {
        attractor[cx] = 0;
        convergence[cx] = 0;
      }

      conv_x = 0;
      attr_x = 0;

      // compute work item
      for ( size_t jx = 0; jx < n_lines; ++jx )
	{
	  xk = x[jx] + I * y[ix];
    
	  double abs_real_xk = creal(xk)*creal(xk);
	  double abs_imag_xk = cimag(xk)*cimag(xk);

        //PERFORMING VALIDATION CHECKS FOR ITERATIONS AND FOR ROOTS

        //Default value of attr set to 99 as that doesnt correspond to any root/color
	  for ( conv = 0, attr = 99; /*conv<MAX_ITERATION*/; ++conv ) {

        if ((abs_real_xk > UPPER_BOUND) && (abs_imag_xk > UPPER_BOUND)) { 
          attr = 11;    //A Default color (maroon) which is not associated to any actual root is assigned
          //conv = 52;
          break;
        }

        if ((abs_real_xk + abs_imag_xk) < LOWER_BOUND) {
          attr = 10;    //A Default color (maroon) which is not associated to any actual root is assigned
          break;
        }

        for (int i = 0; i < deg_func; i++) {
              double real_xdr = creal(xk) - creal(roots[i]);
              double imag_xdr = cimag(xk) - cimag(roots[i]);
            
                if ( (real_xdr * real_xdr) + (imag_xdr * imag_xdr) < LOWER_BOUND) {
                  attr = i;   //A color associated to the actual root is assigned
                  break;
                }

        /*if ((abs_real_xk + abs_imag_xk) >= ((1 - NORM_BOUND)) && ((abs_real_xk + abs_imag_xk) <= ((1 + NORM_BOUND)))) {
          
          }*/
        }
    

        if ( attr != 99 )
    {
      break;
    }

        // COMPUTATION OF NEWTONS ITERATION
        switch (deg_func) {
          case 1:
              //STATEMENTS FOR DEGREE 1
              x_k1 = 1;
              break;

            case 2:
            //STATEMENTS FOR DEGREE 2
              x_k1 = (0.5)*(xk) + (0.5)*(1/xk);
              break;

            case 3:
            //STATEMENTS FOR DEGREE 3
              x_k1 = (0.666666666666667)*(xk) + (0.333333333333333)*(1/(xk*xk));
              break;

            case 4:
              //STATEMENTS FOR DEGREE 4
          x_k1 = (0.750000000000000)*(xk) + (0.250000000000000)*(1/(xk*xk*xk));
              break;

            case 5:
              //STATEMENTS FOR DEGREE 5
              x_k1 = (0.800000000000000)*(xk)+ (0.200000000000000)*(1/(xk*xk*xk*xk));
              break;

            case 6:
              //STATEMENTS FOR DEGREE 6
              x_k1 = (0.833333333333333)*(xk) + (0.166666666666667)*(1/(xk*xk*xk*xk*xk));
              break;

            case 7:
              //STATEMENTS FOR DEGREE 7
              x_k1 = (0.857142857142857)*(xk) + (0.142857142857142)*(1/(xk*xk*xk*xk*xk*xk));
              break;

            case 8:
              //STATEMENTS FOR DEGREE 8
              x_k1 = (0.875000000000000)*(xk) + (0.125000000000000)*(1/(xk*xk*xk*xk*xk*xk*xk));
              break;

            case 9:
              //STATEMENTS FOR DEGREE 9
              x_k1 = (0.888888888888889)*(xk) + (0.111111111111111)*(1/(xk*xk*xk*xk*xk*xk*xk*xk));
              break;

            default:
              fprintf(stderr, "unexpected degree\n");
              exit(1);     
            }
        
        xk = x_k1;    //Updating the value of xk with the newly computed value for the next iteraion

      }

    if(attr == 99)
      {
        attr = 11;
      }

    convergence[conv_x] = conv;
    attractor[attr_x] = attr;
    conv_x++;
    attr_x++;

  }
      convergences[ix] = convergence;
      attractors[ix] = attractor;

      pthread_mutex_lock(&item_done_mutex);
      item_done[ix] = 1;
      pthread_mutex_unlock(&item_done_mutex);

      //free(convergence);
      //free(attractor);
    }
  
  free(x);
  free(y);

  return NULL;
}

// Writing to file - write_main()
void * write_main(void * args)
{
  char * item_done_loc = (char*)calloc(n_lines, sizeof(char));
  
  // Open the files for attractor and convergence for writing
  char filename1[30];
  char filename2[30];
  sprintf(filename1, "newton_attractors_x%d.ppm", deg_func);
  sprintf(filename2, "newton_convergence_x%d.ppm", deg_func);

  FILE * attr_file, * conv_file;
  attr_file = fopen(filename1, "wb"); 
  conv_file = fopen(filename2, "wb"); 
  
  // Writing the File Header
  int max_color_val = 255;
  fprintf(attr_file, "P3\n%d %d\n%d\n", n_lines, n_lines, max_color_val);
  fprintf(conv_file, "P3\n%d %d\n%d\n", n_lines, n_lines, max_color_val);

  // Creating array of colors
  char colors [12][13] = {
       "255 000 000 ", //Red
       "000 255 000 ", //Green
       "000 000 255 ", //Blue
       "255 255 000 ", //Yellow
       "255 000 255 ", //Magenta
       "000 255 255 ", //Cyan
       "255 069 000 ", //Orange
       "255 020 147 ", //Pink
       "128 000 128 ", //Purple
       "000 128 128 ", //Teal
       "139 069 019 ", //Brown
       "128 000 000 ", //Maroon
  };

  // Creating array of grayscale
  char gray [52][13] = {
      "000 000 000 ",
      "005 005 005 ",
      "010 010 010 ",
      "015 015 015 ",
      "020 020 020 ",
      "025 025 025 ",
      "030 030 030 ",
      "035 035 035 ",
      "040 040 040 ",
      "045 045 045 ",
      "050 050 050 ",
      "055 055 055 ",
      "060 060 060 ",
      "065 065 065 ",
      "070 070 070 ",
      "075 075 075 ",
      "080 080 080 ",
      "085 085 085 ",
      "090 090 090 ",
      "095 095 095 ",
      "100 100 100 ",
      "105 105 105 ",
      "110 110 110 ",
      "115 115 115 ",
      "120 120 120 ",
      "125 125 125 ",
      "130 130 130 ",
      "135 135 135 ",
      "140 140 140 ",
      "145 145 145 ",
      "150 150 150 ",
      "155 155 155 ",
      "160 160 160 ",
      "165 165 165 ",
      "170 170 170 ",
      "175 175 175 ",
      "180 180 180 ",
      "185 185 185 ",
      "190 190 190 ",
      "195 195 195 ",
      "200 200 200 ",
      "205 205 205 ",
      "210 210 210 ",
      "215 215 215 ",
      "220 220 220 ",
      "225 225 225 ",
      "230 230 230 ",
      "235 235 235 ",
      "240 240 240 ",
      "245 245 245 ",
      "250 250 250 ",
      "255 255 255 ",
  };

  for ( size_t ix = 0; ix < n_lines; ) {
    pthread_mutex_lock(&item_done_mutex);
    if ( item_done[ix] != 0 )
      memcpy(item_done_loc, item_done, n_lines * sizeof(char));
    pthread_mutex_unlock(&item_done_mutex);

    // Sleep for 100 microseconds
    const long INTERVAL_MS = 100 * NANO_SECOND_MULTIPLIER;
    struct timespec sleep_timespec;
    sleep_timespec.tv_sec = 0;
    sleep_timespec.tv_nsec = INTERVAL_MS;
    if ( item_done_loc[ix] == 0 ) {
      nanosleep(&sleep_timespec, NULL);
      continue;
    }

    for(; ix < n_lines && item_done_loc[ix] != 0; ++ix ) {
  
    // Write result
    // Creating ROW BUFFER  
    char row_attr[(n_lines*13)];
    char row_conv[(n_lines*13)];
          
    for ( size_t cx = 0; cx < n_lines; ++cx ) {
      int attractor_pixel_cx = attractors[ix][cx];
      memcpy(row_attr+(cx*12), colors[attractor_pixel_cx], 12);
        int convergence_pixel_cx = convergences[ix][cx];
        memcpy(row_conv+(cx*12), gray[convergence_pixel_cx], 12);
    }

    // Adding NEW LINE to end of row
    row_attr[n_lines*13] = '\n';
    row_conv[n_lines*13] = '\n';
    fwrite(row_attr, (n_lines*12)+1, 1, attr_file);
    fwrite(row_conv, (n_lines*12)+1, 1, conv_file);

    } // FOR LOOP ends here


} // ix ends here

  free(item_done_loc);
  fclose(attr_file);
  fclose(conv_file);
  return NULL;
} // write main ends here
