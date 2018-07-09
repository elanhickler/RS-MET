/** We create simplified typenames for some explicit instantiations of the RAPT template classes. 
We use the following convention: we append one or more letters to template class name that 
indicate the template parameters. For example, rsPhaseScopeBuffer<float, float, double> becomes: 
PhaseScopeBufferFFD. For each typedef here, we also need the corresponding explicit 
instantiation in RaptInstantiations.cpp, otherwise linker errors will occur when the defined types 
are actually used somewhere. On the other hand, we don't necessarily need a typedef for every 
explicit instantiation in RaptInstatiations.cpp, but then we have to always write out the full 
template: we would have to write rsPhaseScopeBuffer<float, float, double> everywhere, if the 
PhaseScopeBufferFFD typedef wouldn't exist. */

#ifndef RAPT_INSTANTIATIONS_H
#define RAPT_INSTANTIATIONS_H

#include "../../../../../Libraries/JUCE/modules/rapt/rapt.h"
//#include "../../../../../Libraries/JUCE/modules/rosic/rosic.h"
//using namespace RAPT;

//#include "../../../../../Libraries/JUCE/modules/rapt/Data/Simd/Float64x2.h"
// needed when it's commented out in rapt -> reduce build time during tweaking the class


// Data:
//typedef RAPT::rsArray<float> rsArrayF;
//typedef RAPT::rsArray<double> rsArrayD;


// Math:

typedef RAPT::rsPolynomial<float> rsPolynomialF;
typedef RAPT::rsPolynomial<double> rsPolynomialD;
//typedef RAPT::rsPolynomial<int> rsPolynomialI;

typedef RAPT::rsConicSection<float> rsConicSectionF;
typedef RAPT::rsEllipse<float> rsEllipseF;
typedef RAPT::rsLine2D<float> rsLine2DF;

typedef RAPT::rsCoordinateMapper<float> rsCoordinateMapperF;
typedef RAPT::rsCoordinateMapper<double> rsCoordinateMapperD;
typedef RAPT::rsCoordinateMapper2D<float> rsCoordinateMapper2DF;
typedef RAPT::rsCoordinateMapper2D<double> rsCoordinateMapper2DD;

typedef RAPT::rsRootFinder<float> rsRootFinderF;
typedef RAPT::rsNodeBasedFunction<float> rsNodeBasedFunctionF;
typedef RAPT::rsSinCosTable<float> rsSinCosTableF;
typedef RAPT::rsSinCosTable<double> rsSinCosTableD;

typedef RAPT::rsVector2D<float> rsVector2DF;
typedef RAPT::rsVector3D<float> rsVector3DF;



// Filters-Musical:
typedef RAPT::rsSmoothingFilter<float, float> rsSmoothingFilterFF;
typedef RAPT::rsLadderFilter<float, float> rsLadderFilterFF;
typedef RAPT::rsLadderFilter<double, double> rsLadderFilterDD;
//typedef RAPT::rsLadderFilter<rsFloat64x2, double> rsLadderFilterD2D;
//typedef RAPT::rsLadderFilter<rsFloat64x2, rsFloat64x2> rsLadderFilterD2D2;
typedef RAPT::rsPhasorFilter<float, float> rsPhasorFilterFF;
typedef RAPT::rsPhasorStateMapper<float> rsPhasorStateMapperF;
typedef RAPT::rsStateVariableFilter<float, float> rsStateVariableFilterFF; 

// Filters-Scientific:
typedef RAPT::rsPrototypeDesigner<float> rsPrototypeDesignerF;
//typedef RAPT::rsPrototypeDesigner<double> rsPrototypeDesignerD;
typedef RAPT::rsPoleZeroMapper<float> rsPoleZeroMapperF;
typedef RAPT::rsFilterCoefficientConverter<float> rsFilterCoefficientConverterF;
typedef RAPT::rsInfiniteImpulseResponseDesigner<float> rsInfiniteImpulseResponseDesignerF;
typedef RAPT::rsFilterAnalyzer<float> rsFilterAnalyzerF;
typedef RAPT::rsBiquadCascade<float, float> rsBiquadCascadeFF;
typedef RAPT::rsEngineersFilter<float, float> rsEngineersFilterFF;
typedef RAPT::rsLinkwitzRileyCrossOver<float, float> rsLinkwitzRileyCrossOverFF;
typedef RAPT::rsCrossOver4Way<float, float> rsCrossOver4WayFF;
typedef RAPT::rsDirectFormFilter<float, float> rsDirectFormFilterFF;
typedef RAPT::rsEllipticSubBandFilter<float, float> rsEllipticSubBandFilterFF;
typedef RAPT::rsEllipticSubBandFilterDirectForm<float, float> rsEllipticSubBandFilterDirectFormFF;
typedef RAPT::rsQuadratureNetwork<float, float> QuadratureNetworkFF;




// Physics:
typedef RAPT::rsParticleSystem<float> rsParticleSystemF;

// Generators:
typedef RAPT::rsBouncillator<float> rsBouncillatorF;
typedef RAPT::rsRayBouncer<float> rsRayBouncerF;
typedef RAPT::rsNoiseGenerator<float> rsNoiseGeneratorF;

// Modulation:
typedef RAPT::rsBreakpointModulator<float> rsBreakpointModulatorF;

// Visualization:
typedef RAPT::rsImage<float> rsImageF;
typedef RAPT::rsAlphaMask<float> rsAlphaMaskF;
typedef RAPT::rsImagePainter<float, float, float> rsImagePainterFFF;
typedef RAPT::rsImageDrawer<float, float, float> rsImageDrawerFFF;
typedef RAPT::rsLineDrawer<float, float, float> rsLineDrawerFFF;
typedef RAPT::rsPhaseScopeBuffer<float, float, double> rsPhaseScopeBufferFFD;

// Unfinished:
typedef RAPT::rsTwoBandSplitter<float, float> rsTwoBandSplitterFF;
typedef RAPT::rsMultiBandSplitter<float, float> rsMultiBandSplitterFF;

#endif