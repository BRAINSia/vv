#ifndef clitkMedianImageGenericFilter_txx
#define clitkMedianImageGenericFilter_txx

/* =================================================
 * @file   clitkMedianImageGenericFilter.txx
 * @author Bharath Navalapakkam <Bharath.Navalpakkam@creatis.insa-lyon.fr>
 * @date   20 March 2010
 * 
 * @brief 
 * 
 ===================================================*/

// itk include
#include "itkMedianImageFilter.h"
#include "itkImage.h"

namespace clitk
{
  
  //--------------------------------------------------------------------
  template<class args_info_type>
  MedianImageGenericFilter<args_info_type>::MedianImageGenericFilter():
    ImageToImageGenericFilter<Self>("MedianImage") {
    InitializeImageType<2>();
    InitializeImageType<3>();
    InitializeImageType<4>();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dim>
  void MedianImageGenericFilter<args_info_type>::InitializeImageType() {      
    ADD_IMAGE_TYPE(Dim, char);
    ADD_IMAGE_TYPE(Dim, uchar);
    ADD_IMAGE_TYPE(Dim, short);
    ADD_IMAGE_TYPE(Dim, int);
    ADD_IMAGE_TYPE(Dim, float);
    ADD_IMAGE_TYPE(Dim, double);
    ADD_IMAGE_TYPE(Dim, unsigned short); 
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template<class args_info_type>
  void MedianImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a) {
    mArgsInfo=a;
    SetIOVerbose(mArgsInfo.verbose_flag);  

    if (mArgsInfo.input_given) {
      SetInputFilename(mArgsInfo.input_arg);
    }
    if (mArgsInfo.output_given) {
      SetOutputFilename(mArgsInfo.output_arg);
    }

  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //--------------------------------------------------------------------
  template<class args_info_type>
  template<class InputImageType>
  void 
  MedianImageGenericFilter<args_info_type>::UpdateWithInputImageType()
  {
    // Reading input
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
    // Typedef
    typedef typename InputImageType::PixelType PixelType;

    // typedef itk::Image<PixelType,InputImageType::ImageDimension> OutputImageType;

    // Main filter   
    typedef itk::Image<PixelType, InputImageType::ImageDimension> OutputImageType;
    class  InputImageType::SizeType indexRadius;

    // Filter
    typedef itk::MedianImageFilter<InputImageType, OutputImageType> MedianImageFilterType; 
    typename MedianImageFilterType::Pointer thresholdFilter=MedianImageFilterType::New();
    thresholdFilter->SetInput(input);

    indexRadius[0]=mArgsInfo.radius_arg[0];
    indexRadius[1]=mArgsInfo.radius_arg[1];
    indexRadius[2]=mArgsInfo.radius_arg[2];
  
    // indexRadius[0] = 1; 
    // indexRadius[1] = 1;

    thresholdFilter->SetRadius( indexRadius );  

    typename OutputImageType::Pointer outputImage = thresholdFilter->GetOutput();
    thresholdFilter->Update();
     
    // Write/Save results
    this->template SetNextOutput<OutputImageType>(outputImage);   
  }
  
  //--------------------------------------------------------------------

}//end clitk

 
#endif //#define clitkMedianImageGenericFilter_txx

