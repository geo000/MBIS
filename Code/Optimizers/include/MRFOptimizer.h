// --------------------------------------------------------------------------------------
// File:    MRFOptimizer.h
// Date:    Mar 15, 2012
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

#ifndef MRFOPTIMIZER_H_
#define MRFOPTIMIZER_H_

#include <vector>

#include <itkObject.h>
#include <itkImage.h>

template < class TEnergy >
class MRFOptimizer : public itk::Object {

public:
	typedef MRFOptimizer<TEnergy>                 Self;
	typedef itk::SmartPointer<Self>               Pointer;
	typedef itk::SmartPointer<const Self>         ConstPointer;
	typedef TEnergy                               EnergyType;

    // Start Optimization
    virtual void StartOptimization() = 0;

    itkSetClampMacro( MaxNumberOfIterations, size_t, 1, 100 );
    itkGetConstMacro( MaxNumberOfIterations, size_t );

    itkSetObjectMacro( MRFEnergy, EnergyType );
    itkGetObjectMacro( MRFEnergy, EnergyType );

protected:

    typename EnergyType::Pointer                 m_MRFEnergy;
private:
	size_t                                       m_MaxNumberOfIterations;

};

#endif /* MRFOPTIMIZER_H_ */
