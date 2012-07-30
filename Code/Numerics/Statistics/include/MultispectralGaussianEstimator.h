// --------------------------------------------------------------------------------------
// File:          MultispectralGaussianEstimator.h
// Date:          Nov 14, 2011
// Author:        code@oscaresteban.es (Oscar Esteban)
// Version:       1.0 beta
// License:       GPLv3 - 29 June 2007
// Short Summary:
// --------------------------------------------------------------------------------------
//
// Copyright (c) 2012, code@oscaresteban.es (Oscar Esteban)
// with Signal Processing Lab 5, EPFL (LTS5-EPFL)
// and Biomedical Image Technology, UPM (BIT-UPM)
// All rights reserved.
//
// This file is part of MBIS
//
// MBIS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MBIS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MBIS.  If not, see <http://www.gnu.org/licenses/>.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef MULTISPECTRALGAUSSIANESTIMATOR_H_
#define MULTISPECTRALGAUSSIANESTIMATOR_H_

#include <vector>


#include <itkImageToListSampleFilter.h>
#include <itkImageToListSampleAdaptor.h>
#include <itkJointDomainImageToListSampleAdaptor.h>
#include "Numerics/Statistics/include/GaussianMixtureModelComponent.h"
#include "Numerics/Statistics/include/BiasedMixtureModelEMEstimator.h"
#include <itkWeightedMeanSampleFilter.h>
#include <itkWeightedCovarianceSampleFilter.h>

#include <itkMaximumDecisionRule.h>
#include <itkSampleClassifierFilter.h>
#include <itkImageClassifierFilter.h>

#include <itkMaskImageFilter.h>
#include <itkInvertIntensityImageFilter.h>

#include "MultispectralFilter.h"
//#include "itkGaussianLogMembershipFunction.h"

namespace itk
{

template <class TInputComponent, class TProbabilityPixelType = float >
class ITK_EXPORT MultispectralGaussianEstimator:
	public MultispectralFilter< TInputComponent, itk::Image< unsigned char, itk::GetImageDimension<TInputComponent>::ImageDimension> >
{
public:
	/** Standard class typedefs */
	typedef MultispectralGaussianEstimator                                                  Self;
	typedef SmartPointer<Self>                                                     Pointer;
	typedef SmartPointer<const Self>                                               ConstPointer;

	typedef Image< unsigned char,
			itk::GetImageDimension<TInputComponent>::ImageDimension>               OutputImageType;
	typedef typename OutputImageType::Pointer                                      OutputImagePointer;
	typedef typename itk::ImageRegionIterator< OutputImageType >                   OutputImageIterator;

	typedef MultispectralFilter< TInputComponent, OutputImageType >                Superclass;

	typedef typename Superclass::InputVectorImageType                              InputVectorImageType;
	typedef typename itk::ImageRegionConstIteratorWithIndex
			< InputVectorImageType > 			   InputImageIterator;

    typedef itk::Array< double >                                                   ParametersType;
    typedef std::vector< ParametersType >                                          ParametersVectorType;

    /** Priors probability images typedefs */
    typedef TProbabilityPixelType                                                  ProbabilityPixelType;
    typedef itk::Image< ProbabilityPixelType,
    		itk::GetImageDimension<TInputComponent>::ImageDimension >              ProbabilityImageType;
    typedef typename ProbabilityImageType::Pointer                                 ProbabilityImagePointer;
    typedef typename ProbabilityImageType::ConstPointer                            ProbabilityImageConstPointer;
    typedef typename std::vector< ProbabilityImageConstPointer >                   ProbabilityImagesVector;

    typedef itk::Statistics::ImageToListSampleAdaptor
    		                                  < ProbabilityImageType >             ProbabilitySampleType;
    typedef typename ProbabilitySampleType::Pointer                                ProbabilitySamplePointer;
    typedef typename ProbabilitySampleType::ConstPointer                           ProbabilitySampleConstPointer;


	typedef itk::Statistics::ImageToListSampleAdaptor
	                                          < InputVectorImageType >             InputSampleType;
	typedef typename InputSampleType::MeasurementVectorType                        MeasurementVectorType;

	typedef mfbs::Statistics::BiasedMixtureModelEMEstimator< InputSampleType >     EstimatorType;

    typedef typename mfbs::Statistics::GaussianMixtureModelComponent
    		                                     < InputSampleType >               ComponentType;
    typedef std::vector< typename ComponentType::Pointer >                         ComponentVectorType;

    /** Weighted statistical parameters estimation */
    typedef typename  itk::Statistics::WeightedCovarianceSampleFilter
    		                                    < InputSampleType >                CovarianceEstimatorType;
    typedef typename CovarianceEstimatorType::WeightArrayType                      WeightArrayType;

    typedef typename EstimatorType::MembershipFunctionType                         MembershipFunctionType;
    typedef typename MembershipFunctionType::Pointer                               MembershipFunctionPointer;
    typedef itk::Statistics::MaximumDecisionRule                                   DecisionRuleType;
    typedef typename itk::Statistics::ImageClassifierFilter
    		< InputSampleType, InputVectorImageType, OutputImageType >             ImageClassifierType;
    typedef typename ImageClassifierType::ClassLabelVectorObjectType               ImageClassLabelVectorObjectType;
    typedef typename ImageClassifierType::ClassLabelVectorType                     ImageClassLabelVectorType;
    typedef typename ImageClassifierType::MembershipFunctionVectorObjectType       ImageMembershipFunctionVectorObjectType;
    typedef typename ImageClassifierType::MembershipFunctionVectorType             ImageMembershipFunctionVectorType;
    typedef typename ImageClassifierType::MembershipFunctionsWeightsArrayType      ImageMembershipFunctionsWeightsArrayType;

    typedef typename itk::MaskImageFilter
    		< InputVectorImageType, ProbabilityImageType, InputVectorImageType>    InputMaskFilter;
    typedef typename itk::InvertIntensityImageFilter
    		< ProbabilityImageType, ProbabilityImageType >						   InputMaskInverter;


    typedef itk::Array< float >                                                    DataCostArrayType;
//    typedef itk::Statistics::GaussianLogMembershipFunction
//    		                                  < MeasurementVectorType >            LogEnergyFunction;


    /** Method to get the list sample, the generated output. Note that this does
     * not invoke Update(). You should have called update on this class to get
     * any meaningful output. */
    const InputSampleType * GetSample() const  { return m_Sample; }

	/** Method for creation through the object factory. */
	itkNewMacro(Self);
	/** Run-time type information (and related methods). */
	itkTypeMacro(MultispectralGaussianEstimator, MultispectralFilter);

    itkSetMacro(MaximumIteration, unsigned int);
    itkGetConstMacro(MaximumIteration, unsigned int);

    itkSetMacro(UsePriorProbabilityImages, bool);
    itkGetConstMacro(UsePriorProbabilityImages, bool);

    void SetPriorProbabilityImage( unsigned int classId, const ProbabilityImageType * image );

    void SetPriors( const ProbabilityImagesVector& priors );

    itkGetConstMacro(DataCostArray, DataCostArrayType);

    void SetMeans( ParametersVectorType& means );


    void SetInitParamClass( ParametersVectorType& params) {
    	m_StatParameters = params;
    }
    itkGetConstMacro(StatParameters, ParametersVectorType);

    itkSetObjectMacro( MaskImage, ProbabilityImageType);
    itkGetObjectMacro( MaskImage, ProbabilityImageType);

    itkSetMacro(UseMaskedMode, bool);
    itkGetConstMacro(UseMaskedMode, bool);

    itkSetClampMacro( NumberOfClasses, unsigned char, 0, 50 );
    itkGetConstMacro( NumberOfClasses, unsigned char);

protected:
	MultispectralGaussianEstimator();
	virtual ~MultispectralGaussianEstimator();

	virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

	  /** Standard pipeline method. While this class does not implement a
	   * ThreadedGenerateData(), its GenerateData() delegates all
	   * calculations to an InputSampleFilter. Multithreading depends on
	   * that one  */
	  void GenerateData();


	  void ComputeInitialParameters();
	  void ComputeInitialParametersMaskedMode();

private:
	MultispectralGaussianEstimator( const Self& ); //purposely not implemented
	void operator=(const Self& );       //purposely not implemented


	ProbabilityImagePointer                   m_MaskImage;
	typename InputMaskFilter::Pointer         m_Masker;
	ProbabilityImagesVector                   m_Priors;

	typename InputVectorImageType::ConstPointer  m_Input;

	typename InputSampleType::Pointer         m_Sample;

	typename EstimatorType::Pointer           m_Estimator;
	unsigned int                              m_MaximumIteration;
	bool                                      m_UsePriorProbabilityImages;

	ParametersVectorType                      m_StatParameters;
	std::vector< WeightArrayType >            m_WeightArrayVector;
	ParametersType                            m_InitialProportions;

	DataCostArrayType                         m_DataCostArray;

	ComponentVectorType                       m_CVector;

	unsigned char                             m_NumberOfClasses;
	unsigned char                             m_NumberOfComponents;
	unsigned int                              m_PriorIndexOffset;

	bool									  m_UseMaskedMode;
};

}

#include "MultispectralGaussianEstimator.txx"



#endif /* MULTISPECTRALGAUSSIANESTIMATOR_H_ */
