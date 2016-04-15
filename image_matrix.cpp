#include "image_matrix.hpp"


image_matrix::image_matrix()
{}


image_matrix::image_matrix( int n_rows_, int n_cols_ )
  : _n_rows( n_rows_ ),
    _n_cols( n_cols_ )
{
  _data.resize( _n_rows * _n_cols, 0.0f );
}


image_matrix::~image_matrix()
{
  _data.clear();
}


int image_matrix::get_n_rows() const
{
  return _n_rows;
}


int image_matrix::get_n_cols() const
{
  return _n_cols;
}


void image_matrix::resize( int n_rows_, int n_cols_ )
{
  _n_rows = n_rows_;
  _n_cols = n_cols_;
  _data.resize( _n_rows * _n_cols, 0.0f );
}


float image_matrix::get_pixel( int r_, int c_ ) const
{
  return _data[ r_ * _n_cols + c_ ];
}


void image_matrix::set_pixel( int r_, int c_, float value_ )
{
  _data[ r_ * _n_cols + c_ ] = value_;
}
