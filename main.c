#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<time.h>
#include<complex.h>
#include<string.h>

#define UPPER_BOUND 100000000000000000000
#define LOWER_BOUND 0.000006
#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds


pthread_mutex_t item_done_mutex;
int n_rows_col, n_threads, deg_func;
int ** results;
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

  printf("Assignment 2\n");
  char t = 't';
  int base = 10;
  char alph1 = *(argv[1]+1); // alph2 = *(argv[2]+1), alph3 = *(argv[3]);
  if(alph1 == t)
    {
      n_threads = strtol(argv[1]+2,NULL,base);
      //printf("No of threads are %d\n",n_threads);
      n_rows_col = strtol(argv[2]+2, NULL, base);
      //printf("No of rows and columns is %d\n", n_rows_col);
      deg_func = atoi(argv[3]);
      //printf("Dimension is %d\n", deg_func);
    }

  else
    {
      n_threads = strtol(argv[2]+2, NULL, base);
      //printf("No of threads are %d\n", n_threads);
      n_rows_col = strtol(argv[1]+2, NULL, base);
      //printf("No of rows and columns is %d\n", n_rows_col);
      deg_func = atoi(argv[3]);
      //printf("Dimension is %d\n", deg_func);
    }
  //double complex z = 3 + I * 8;
  //printf("Real part of z is %lf and imaginary part is %lf\n", creal(z), cimag(z));

  // Allocation of global variables
  results = (int **) malloc(sizeof(int*) * n_rows_col);
  item_done = (char *) malloc(sizeof(char) * n_rows_col);
  attractors = (int **) malloc(sizeof(int *) * n_rows_col);
  convergences = (int **) malloc(sizeof(int *) * n_rows_col);

  // Initialisation of mutex
  pthread_mutex_init(&item_done_mutex, NULL);

  // creation of compute threads and write thread
  // dynamic allocation of number of threads
  pthread_t * compute_threads = (pthread_t * ) malloc(sizeof(pthread_t) * n_threads); // sizeof(pthread_t)
  pthread_t write_thread;

  int ret;
  size_t tx; //ix,

  for ( tx = 0; tx < n_threads; ++tx )
    {
      size_t * args = malloc(sizeof(size_t));
      *args = tx;

      if (ret = pthread_create(&compute_threads[tx], NULL, compute_main, (void*)args)) // (pthread_t * restrict)
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
  free(results);
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
  // printf("I'm in compute thread %lu\n", offset);

  // Attractor and convergence
  int * attractor = (int *) malloc(sizeof(int) * n_rows_col);
  int * convergence = (int *) malloc(sizeof(int) * n_rows_col);

  for ( size_t cx = 0; cx < n_rows_col; ++cx ) {
    attractor[cx] = 0;
    convergence[cx] = 0;
  }


  // Creating a matrix that would hold the complex initialisations
  double h = 4/(double)n_rows_col;
  //printf("%0.19lf\n", h);
  double * x = (double *) malloc(sizeof(double) * n_rows_col);
  double * y = (double *) malloc(sizeof(double) * n_rows_col);

  // When n_rows_col is even, then we get x,y = 0 at which f' is undefined, so we need to avoid it
  if((n_rows_col%2)==0) {
    x[0] = -2 + h;
    y[0] = 2 - h;
  }
  else
    x[0] = -2; y[0] = 2;

  //printf("%lf %lf\n", x[0], y[0]);
  for(int i = 1; i<n_rows_col; ++i){
    x[i] = x[i-1] + h;
    y[i] = y[i-1] - h;
  }
  /*for(int i = 0; i<n_rows_col; i++)
    printf("x and y: %lf  %lf\n", x[i], y[i]);*/

  // Creating matrix of complex nos.
  double complex * zentries = (double complex *) malloc(sizeof(double complex) * n_rows_col*n_rows_col);
  double complex ** z = (double complex **) malloc(sizeof(double complex*) * n_rows_col);
  for ( size_t ix = 0, jx = 0; ix < n_rows_col; ++ix, jx+=n_rows_col )
    z[ix] = zentries + jx;

  for ( size_t ix = 0; ix < n_rows_col; ++ix )
    for ( size_t jx = 0; jx < n_rows_col; ++jx )
      z[ix][jx] = x[jx] + I * y[ix];

  //printf("Real: %lf and Complex: %lf\n", creal(z[1][1]), cimag(z[1][1]));

  // Switch Cases with Newtons Iterations and Checking


  for ( size_t ix = offset; ix < n_rows_col; ix += n_threads )
    {
      int * result = (int*)malloc(sizeof(int) * n_rows_col); // I don't free this malloc and still don't get any error
      // compute work item
      results[ix] = result;

      pthread_mutex_lock(&item_done_mutex);
      item_done[ix] = 1;
      pthread_mutex_unlock(&item_done_mutex);
      // compute with work item ix
      //printf("im in row %d\n", ix);

    }
  free(attractor);
  free(convergence);
  free(zentries);
  free(z);
  free(x);
  free(y);

  return NULL;
}

// Writing to file - write_main()
void * write_main(void * args)
{
  char * item_done_loc = (char*)calloc(n_rows_col, sizeof(char));
  //printf("I'm in write thread\n");

  // Open the files for attractor and convergence for writing
  char filename1[30];
  char filename2[30];
  sprintf(filename1, "newton_attractors_x%d.ppm", deg_func);
  sprintf(filename2, "newton_convergence_x%d.ppm", deg_func);
  FILE * attr_file, * conv_file;
  //attr_file = fopen("newton_attractors_x3.ppm", "wb"); /* b - binary mode */
  //conv_file = fopen("newton_convergence_x3.ppm.ppm", "wb"); /* b - binary mode */
  attr_file = fopen(filename1, "wb"); /* b - binary mode */
  conv_file = fopen(filename2, "wb"); /* b - binary mode */

  // Writing the File Header
  int max_color_val = 255;
  fprintf(attr_file, "P3\n%d %d\n%d\n", n_rows_col, n_rows_col, max_color_val);
  fprintf(conv_file, "P3\n%d %d\n%d\n", n_rows_col, n_rows_col, max_color_val);

  // Closing the files
  fclose(attr_file);
  fclose(conv_file);

  // Creating array of colors
  char colors [12][12] ={
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
  char gray [52][12] = {
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

  for ( size_t ix = 0; ix < n_rows_col; ) {
    pthread_mutex_lock(&item_done_mutex);
    if ( item_done[ix] != 0 )
      memcpy(item_done_loc, item_done, n_rows_col * sizeof(char));
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

    int * result;
    for(; ix < n_rows_col && item_done_loc[ix] != 0; ++ix ) {
      result = results[ix];

      // Write result
      attr_file = fopen(filename1, "ab"); /* b - binary mode */
      conv_file = fopen(filename2, "ab"); /* b - binary mode */

      char row_attr[25];
      char temp[1] = {'\n'};
      memcpy(row_attr,*(colors+1), 12);
      memcpy(row_attr+12, *(colors+2), 12);
      memcpy(row_attr+2*12, temp, 1);
      fwrite(row_attr, 2*12+1, 1, attr_file);
      char row_conv[25];
      memcpy(row_conv,*(gray+1), 12);
      memcpy(row_conv+12, *(gray+20), 12);
      memcpy(row_conv+2*12, temp, 1);
      fwrite(row_conv, 2*12+1, 1, conv_file);

      //char row_attr[sizeof(colors[0]) * n_rows_col + 1)];



      // Closing the files
      fflush(attr_file);
      fflush(conv_file);
      fclose(attr_file);
      fclose(conv_file);
      free(result);
    }

  }

  free(item_done_loc);
  return NULL;
}
