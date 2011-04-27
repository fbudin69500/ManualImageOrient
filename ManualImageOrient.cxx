#include "ManualImageOrientCLP.h"
#include <itkAffineTransform.h>
#include <itkMatrix.h>
#include <itkTransformFileWriter.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

bool CheckString( std::string orientationString )
{
  //check size (string should be 3 letters long)
  if( orientationString.size() != 3 )
  {
    return 0 ;
  }
  std::vector< char > usedChar ;
  std::string listChar = "LRPASI" ;
  for( unsigned int i = 0 ; i < orientationString.size() ; i++ )
  {
    size_t pos = listChar.find( orientationString[ i ] ) ;
    if( pos == std::string::npos )
    {
      return 0 ;
    }
    for( unsigned int j = 0 ; j < usedChar.size() ; j++ )
    {
      if( orientationString[ i ] == usedChar[ j ] )
      {
        return 0 ;
      }
    }
    usedChar.push_back( orientationString[ i ] ) ;
    //find complementary character: PA,LR,IS
    usedChar.push_back( listChar[ (pos/2)*2+!(pos%2) ] ) ;
  }
  return 1 ;
}

int main( int argc, char * argv[] )
{
  PARSE_ARGS;
  bool correctInputString = CheckString( inputOrientation ) ;
  bool correctOutputString = CheckString( outputOrientation ) ;
  
  if( !correctOutputString || !correctInputString )
  {
    std::cerr << "Both the input and output orientation strings should be composed of 3 different letters picked in L,P,S,R,A,I" << std::endl ;
    std::cerr << "ie: LPS RAS or PRS LPS" << std::endl ;
    return EXIT_FAILURE ;
  }
  //Compute conjugate characters of output orientation string
  std::string conjugate = "LPS" ;
  std::string listChar = "LRPASI" ;
  for( int i = 0 ; i < 3 ; i++ )
  {
    size_t pos = listChar.find( inputOrientation[ i ] ) ;
    conjugate[ i ] = listChar[ (pos/2)*2+!(pos%2) ] ;
  }
  itk::Matrix< double , 3 , 3 > matrix ;
  matrix.Fill( 0.0 ) ;
  for( int i = 0 ; i < 3 ; i++ )
  {
    for( int j = 0 ; j < 3 ; j++ )
    {
      if( outputOrientation[ i ] == inputOrientation[ j ] )
      {
        matrix[ j ][ i ] = 1.0 ;
      }
      if( outputOrientation[ i ] == conjugate[ j ] )
      {
        matrix[ j ][ i ] = -1.0 ;
      }
    }
  }
  typedef itk::AffineTransform< double , 3 > AffineType ;
  AffineType::Pointer affine = AffineType::New() ;
  affine->SetMatrix( matrix ) ;
  typedef itk::TransformFileWriter WriterType ;
  WriterType::Pointer writer = WriterType::New() ;
  writer->SetFileName( transform ) ;
  writer->AddTransform( affine ) ;
  writer->Update() ;
  return EXIT_SUCCESS ;
}
