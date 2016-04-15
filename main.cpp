#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <pthread.h>

#include "image_matrix.hpp"


// function that performs the median filtering on pixel p(r_,c_) of input_image_,
// using a window of size window_size_
// the function returns the new filtered value p'(r_,c_)
float median_filter_pixel( const image_matrix& input_image_,int r_,int c_,int window_size_ ){
  	int half_window = (window_size_)/2;
  	int window_area=0;
  	std::vector<float> arr;
  	
  	//r_ is same as the 'y' position
  	//c_ is same as the 'x' position
	for(int y = r_ - half_window; y <= r_ + half_window; y++){
		for(int x = c_ - half_window; x <= c_ + half_window; x++){
			if(y<0 || y>=input_image_.get_n_rows() || x<0 || x>=input_image_.get_n_cols())
				continue;
			arr.push_back(input_image_.get_pixel(y,x));
			window_area++;
		}
	}
	std::sort(arr.begin(), arr.end());
	if(window_area%2==0){
		int temp=(window_area-1)/2;
		float tempp = arr[temp] +arr[temp+1];
		tempp/=2;
		
		return (tempp);
		}
  	return (arr[window_area/2]);
}


// struct passed to each thread, containing the information necessary
// to process its assigned range
struct task{
	int start_row, stop_row, window_size;
	image_matrix input_image;
	image_matrix * output_image;
};


// function run by each thread
void* func( void* arg ){
  	task* t_arg = ( task* )arg;
  
	int start = t_arg->start_row;
	int stop = t_arg->stop_row;
	int window_size = t_arg->window_size;
	
	image_matrix& input = t_arg->input_image;

	image_matrix * output_address= t_arg->output_image;
	
	//'y' is the same as row
	//'x' is the same as col
	for(int y = start; y<=stop; y++){
		for(int x=0;x<input.get_n_cols();x++){
			float filtered_value = median_filter_pixel(input, y, x, window_size);
			output_address->set_pixel(y,x,filtered_value);
		}
	}
  	pthread_exit( NULL );
}

// read input image from file filename_ into image_in_
bool read_input_image( const std::string& filename_, image_matrix& image_in_ )
{
  bool ret = false;
  std::ifstream is( filename_.c_str() );
  if( is.is_open() )
  {
    int n_rows;
    int n_cols;
	
    is >> n_rows;
    is >> n_cols;
	
    image_in_.resize( n_rows, n_cols );

    for( int r = 0; r < n_rows; r++ )
    {
      for( int c = 0; c < n_cols; c++ )
      {
        float value;
        is >> value;
        image_in_.set_pixel( r, c, value );
      }
    }
    is.close();
    ret = true;
  }
  return ret;
}


// write filtered image_out_ to file filename_
bool write_filtered_image( const image_matrix& image_out_ )
{
  bool ret = false;
  std::ofstream os( "filtered.txt" );
  if( os.is_open() )
  {
    int n_rows = image_out_.get_n_rows();
    int n_cols = image_out_.get_n_cols();

    os << n_rows << std::endl;
    os << n_cols << std::endl;

    for( int r = 0; r < n_rows; r++ )
    {
      for( int c = 0; c < n_cols; c++ )
      {
        os << image_out_.get_pixel( r, c ) << " ";
      }
      os << std::endl;
    }
    os.close();
    ret = true;
  }
  return ret;
}

int main( int argc, char* argv[] )
{
  if( argc < 5 )
  {
    std::cerr << "Not enough arguments provided to " << argv[ 0 ] << ". Terminating." << std::endl;
    return 1;
  }

  // get input arguments
  std::string input_filename( argv[ 1 ] );
  int window_size = std::stoi( argv[ 2 ] );
  int n_threads = std::stoi( argv[ 3 ] );
  int mode = std::stoi( argv[ 4 ] );

  std::cout << argv[ 0 ] << " called with parameters " << input_filename << " " << window_size << " " << n_threads << " " << mode << std::endl;

  // input and the filtered image matrices
  image_matrix input_image;
  image_matrix filtered_image;
   
  // read input matrix
  read_input_image( input_filename, input_image );
  // get dimensions of the image matrix
  int n_rows = input_image.get_n_rows();
  int n_cols = input_image.get_n_cols();

  filtered_image.resize( n_rows, n_cols );

  // start with the actual processing
  if( mode == 0 )
  {
    // ******    SERIAL VERSION     ******

    for( int r = 0; r < n_rows; r++ )
    {
      for( int c = 0; c < n_cols; c++ )
      {
        float p_rc_filt = median_filter_pixel( input_image, r, c, window_size);
        filtered_image.set_pixel( r, c, p_rc_filt );
      }
    }

    // ***********************************
  }
  else if( mode == 1 )
  {
    // ******   PARALLEL VERSION    ******
	int section_size = input_image.get_n_rows()/n_threads;
	int j=0;
	int rc;
    // declaration of pthread_t variables for the threads
    pthread_t threads[n_threads];    
    
    // declaration of the struct variables to be passed to the threads
    struct task td[n_threads];
	
    // create threads
	for(int i = 0; i < input_image.get_n_rows(); i += section_size, j++ ){
		td[j].start_row = i;
		//If the last portion of the image is too small to fit into a regular section
		if(i>input_image.get_n_rows())
			td[j].stop_row = input_image.get_n_rows();
		else
			td[j].stop_row = i + section_size;
			
		td[j].window_size = window_size;
		td[j].input_image = input_image;
		td[j].output_image = &filtered_image;
		
		rc = pthread_create(&threads[j], NULL, func, (void *)&td[j]);
		
		if(rc){
			std::cout << "Could not create thread" << rc << std::endl;
			exit(-1);
			}
	}

    // wait for termination of threads
    void *status;
	for(int n = 0; n < n_threads; n++){
		rc = pthread_join(threads[n], &status);
		if(rc){
			std::cout <<"Error waiting for thread" << rc <<std::endl;
			exit(-1);
		}
	}
  }
  else
  {
    std::cerr << "Invalid mode. Terminating" << std::endl;
    return 1;
  }

  // write filtered matrix to file
  write_filtered_image( filtered_image );

  return 0;
}
