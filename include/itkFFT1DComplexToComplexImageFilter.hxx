/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkFFT1DComplexToComplexImageFilter_hxx
#define itkFFT1DComplexToComplexImageFilter_hxx

#include "itkFFT1DComplexToComplexImageFilter.h"

#include "itkVnlFFT1DComplexToComplexImageFilter.h"

#if defined(ITK_USE_FFTWD) || defined(ITK_USE_FFTWF)
#include "itkFFTW1DComplexToComplexImageFilter.h"
#endif

#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"

namespace itk
{

template < typename TInputImage, typename TOutputImage >
typename FFT1DComplexToComplexImageFilter< TInputImage, TOutputImage >::Pointer
FFT1DComplexToComplexImageFilter< TInputImage, TOutputImage >
::New()
{
  Pointer smartPtr = ::itk::ObjectFactory< Self >::Create();

#ifdef ITK_USE_FFTWD
  if( smartPtr.IsNull() )
    {
    if( typeid( TPixel ) == typeid( double ) )
      {
      smartPtr = dynamic_cast< Self* >(
	FFTW1DComplexToComplexImageFilter< double, VDimension >
	::New().GetPointer() );
      }
    }
#endif
#ifdef ITK_USE_FFTWF
  if( smartPtr.IsNull() )
    {
    if( typeid( TPixel ) == typeid( float ) )
      {
      smartPtr = dynamic_cast<Self *>(
	FFTW1DComplexToComplexImageFilter< float, VDimension >
	::New().GetPointer() );
      }
    }
#endif

  if( smartPtr.IsNull() )
    {
    smartPtr = VnlFFT1DComplexToComplexImageFilter< TInputImage, TOutputImage >
      ::New().GetPointer();
    }

  return smartPtr;
}


template< typename TInputImage, typename TOutputImage >
FFT1DComplexToComplexImageFilter< TInputImage, TOutputImage >
::FFT1DComplexToComplexImageFilter():
 m_Direction(0), m_TransformDirection( DIRECT )
{
  this->m_ImageRegionSplitter = ImageRegionSplitterDirection::New();
}


template <typename TInputImage, typename TOutputImage>
const ImageRegionSplitterBase*
FFT1DComplexToComplexImageFilter < TInputImage, TOutputImage >
::GetImageRegionSplitter(void) const
{
  return this->m_ImageRegionSplitter.GetPointer();
}


template<typename TInputImage, typename TOutputImage >
void
FFT1DComplexToComplexImageFilter < TInputImage, TOutputImage >
::BeforeThreadedGenerateData()
{
  this->m_ImageRegionSplitter->SetDirection( this->GetDirection() );
}


template< typename TInputImage, typename TOutputImage >
void
FFT1DComplexToComplexImageFilter< TInputImage, TOutputImage >
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the inputs
  typename InputImageType::Pointer inputPtr  =
    const_cast<InputImageType *> (this->GetInput());
  typename OutputImageType::Pointer outputPtr = this->GetOutput();

  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // we need to compute the input requested region (size and start index)
  typedef const typename OutputImageType::SizeType& OutputSizeType;
  OutputSizeType outputRequestedRegionSize =
    outputPtr->GetRequestedRegion().GetSize();
  typedef const typename OutputImageType::IndexType& OutputIndexType;
  OutputIndexType outputRequestedRegionStartIndex =
    outputPtr->GetRequestedRegion().GetIndex();

  //// the regions other than the fft direction are fine
  typename InputImageType::SizeType  inputRequestedRegionSize = outputRequestedRegionSize;
  typename InputImageType::IndexType inputRequestedRegionStartIndex = outputRequestedRegionStartIndex;

  // we but need all of the input in the fft direction
  const unsigned int direction = this->m_Direction;
  const typename InputImageType::SizeType& inputLargeSize =
    inputPtr->GetLargestPossibleRegion().GetSize();
  inputRequestedRegionSize[direction] = inputLargeSize[direction];
  const typename InputImageType::IndexType& inputLargeIndex =
    inputPtr->GetLargestPossibleRegion().GetIndex();
  inputRequestedRegionStartIndex[direction] = inputLargeIndex[direction];

  typename InputImageType::RegionType inputRequestedRegion;
  inputRequestedRegion.SetSize( inputRequestedRegionSize );
  inputRequestedRegion.SetIndex( inputRequestedRegionStartIndex );

  inputPtr->SetRequestedRegion( inputRequestedRegion );
}


template< typename TInputImage, typename TOutputImage >
void
FFT1DComplexToComplexImageFilter< TInputImage, TOutputImage >
::EnlargeOutputRequestedRegion(DataObject *output)
{
  OutputImageType* outputPtr = dynamic_cast<OutputImageType*>( output );

  // we need to enlarge the region in the fft direction to the
  // largest possible in that direction
  typedef const typename OutputImageType::SizeType& ConstOutputSizeType;
  ConstOutputSizeType requestedSize =
    outputPtr->GetRequestedRegion().GetSize();
  ConstOutputSizeType outputLargeSize =
    outputPtr->GetLargestPossibleRegion().GetSize();
  typedef const typename OutputImageType::IndexType& ConstOutputIndexType;
  ConstOutputIndexType requestedIndex =
    outputPtr->GetRequestedRegion().GetIndex();
  ConstOutputIndexType outputLargeIndex =
    outputPtr->GetLargestPossibleRegion().GetIndex();

  typename OutputImageType::SizeType enlargedSize   = requestedSize;
  typename OutputImageType::IndexType enlargedIndex = requestedIndex;
  enlargedSize[this->m_Direction]  = outputLargeSize[this->m_Direction];
  enlargedIndex[this->m_Direction] = outputLargeIndex[this->m_Direction];

  typename OutputImageType::RegionType enlargedRegion;
  enlargedRegion.SetSize( enlargedSize );
  enlargedRegion.SetIndex( enlargedIndex );
  outputPtr->SetRequestedRegion( enlargedRegion );
}


template< typename TInputImage, typename TOutputImage >
void
FFT1DComplexToComplexImageFilter< TInputImage, TOutputImage >
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "Direction: " << m_Direction << std::endl;
  os << indent << "TransformDirection: " << m_TransformDirection << std::endl;
}


} // end namespace itk

#endif // itkFFT1DComplexToComplexImageFilter_hxx
