#ifndef IMAGE_MATRIX_HPP_
#define IMAGE_MATRIX_HPP_

#include <vector>


class image_matrix
{
  public:
    image_matrix();
    image_matrix( int n_rows_, int n_cols_ );
    ~image_matrix();

  private:
    std::vector< float > _data;
    int _n_rows;
    int _n_cols;

  public:
    int get_n_rows() const;

    int get_n_cols() const;

    void resize( int n_rows_, int n_cols_ );

    float get_pixel( int r_, int c_ ) const;

    void set_pixel( int r_, int c_, float value_ );

};


#endif
