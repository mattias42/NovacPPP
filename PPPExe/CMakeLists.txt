# This is the main project of Novac Post Processing Program (NovacPPP)

cmake_minimum_required (VERSION 3.6)

# Define the different groups of files
set(NPP_COMMON_SPECTRA     
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/ScanFileHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/ScanFileHandler.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/Spectrum.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/Spectrum.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/SpectrumInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/SpectrumInfo.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/SpectrumIO.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/Spectra/SpectrumIO.h
)

set(NPP_COMMON_SPECTRUM_FORMAT 
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrumFormat/MKPack.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrumFormat/MKPack.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrumFormat/STDFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrumFormat/STDFile.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrumFormat/TXTFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrumFormat/TXTFile.h
)

set(NPP_COMMON
    ${CMAKE_CURRENT_LIST_DIR}/Common/ASCII.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/Common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/Common.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/DateTime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/DateTime.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/EvaluationConfigurationParser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/EvaluationConfigurationParser.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/EvaluationLogFileHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/EvaluationLogFileHandler.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/GPSData.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/GPSData.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/ProcessingFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/ProcessingFileReader.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrometerModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/SpectrometerModel.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/Version.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/Version.h
    ${CMAKE_CURRENT_LIST_DIR}/Common/XMLFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Common/XMLFileReader.h
)
set(NPP_COMMUNICATION
    ${CMAKE_CURRENT_LIST_DIR}/communication/FTPCom.cpp
    ${CMAKE_CURRENT_LIST_DIR}/communication/FTPCom.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/FTPServerConnection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/communication/FTPServerConnection.h
    ${CMAKE_CURRENT_LIST_DIR}/communication/FTPSocket.cpp
    ${CMAKE_CURRENT_LIST_DIR}/communication/FTPSocket.h
)

set(NPP_CONFIGURATION
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/DarkCorrectionConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/DarkCorrectionConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/DarkSettings.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/DarkSettings.h
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/EvaluationConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/EvaluationConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/InstrumentConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/InstrumentConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/InstrumentLocation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/InstrumentLocation.h
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/LocationConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/LocationConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/NovacPPPConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/NovacPPPConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/UserConfiguration.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Configuration/UserConfiguration.h
)

set(NPP_EVALUATION
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/BasicMath.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/BasicMath.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ColumnCorrection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ColumnCorrection.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/CrossSectionData.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/CrossSectionData.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/Evaluation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/Evaluation.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/EvaluationResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/EvaluationResult.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ExtendedScanResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ExtendedScanResult.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/FitWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/FitWindow.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/MessageLog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/MessageLog.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/PostEvaluationController.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/PostEvaluationController.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ReferenceFile.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ReferenceFile.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ReferenceFitResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ReferenceFitResult.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ScanEvaluation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ScanEvaluation.h
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ScanResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Evaluation/ScanResult.h
)

set(NPP_FIT
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ApertureFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BandedMatrix.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BSpline.cxx
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BSpline.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BSplineD.cxx
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BSplineD.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BSplineF.cxx
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BSplineF.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/BSplineImpl.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ConstFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ConvoluteFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ConvolutionCoreFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/CubicBSplineFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/CubicSplineFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/DataSet.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/DiscreteFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/DivFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/DOASVector.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ExpFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/Fit.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/FitBasic.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/FitException.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/Function.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/GaussFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/LeastSquareFit.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/LevenbergMarquardtFit.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/LinearInterpolateFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/LnFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/Matrix.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/Minimizer.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/MulFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/NegateFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/NonlinearParameterFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ParameterLinkItem.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ParameterVector.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ParamFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/PolynomialFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/ReferenceSpectrumFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/SimpleDOASFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/StandardFit.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/StandardMetricFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/StatisticVector.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/SumFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/Fit/Vector.h
)

set(NPP_FLUX
    ${CMAKE_CURRENT_LIST_DIR}/Flux/FluxCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Flux/FluxCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/Flux/FluxResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Flux/FluxResult.h
    ${CMAKE_CURRENT_LIST_DIR}/Flux/FluxStatistics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Flux/FluxStatistics.h
)

set(NPP_GEOMETRY
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/GeometryCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/GeometryCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/GeometryResult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/GeometryResult.h
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/PlumeDataBase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/PlumeDataBase.h
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/PlumeHeight.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Geometry/PlumeHeight.h
)

set(NPP_METEOROLOGY
    ${CMAKE_CURRENT_LIST_DIR}/Meteorology/WindDataBase.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Meteorology/WindDataBase.h
    ${CMAKE_CURRENT_LIST_DIR}/Meteorology/WindField.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Meteorology/WindField.h
    ${CMAKE_CURRENT_LIST_DIR}/Meteorology/XMLWindFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Meteorology/XMLWindFileReader.h
)

set(NPP_STRATOSPHERE
    ${CMAKE_CURRENT_LIST_DIR}/Stratosphere/StratosphereCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Stratosphere/StratosphereCalculator.h

)

set(NPP_WINDMEASUREMENT
    ${CMAKE_CURRENT_LIST_DIR}/WindMeasurement/WindSpeedCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WindMeasurement/WindSpeedCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/WindMeasurement/WindSpeedMeasSettings.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WindMeasurement/WindSpeedMeasSettings.h
)


# Add the different components
add_executable(NovacPPP
    ${CMAKE_CURRENT_LIST_DIR}/ContinuationOfProcessing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ContinuationOfProcessing.h
    ${CMAKE_CURRENT_LIST_DIR}/FileInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FileInfo.h
    ${CMAKE_CURRENT_LIST_DIR}/MeteorologySource.h
    ${CMAKE_CURRENT_LIST_DIR}/Molecule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Molecule.h
    ${CMAKE_CURRENT_LIST_DIR}/ObservatoryInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ObservatoryInfo.h
    ${CMAKE_CURRENT_LIST_DIR}/PlumeInScanProperty.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PlumeInScanProperty.h
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessing.h
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessingStatistics.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PostProcessingStatistics.h
    ${CMAKE_CURRENT_LIST_DIR}/SetupFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SetupFileReader.h
    ${CMAKE_CURRENT_LIST_DIR}/StatusLogFileWriter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/StatusLogFileWriter.h
    ${CMAKE_CURRENT_LIST_DIR}/StringTokenizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/StringTokenizer.h
    ${CMAKE_CURRENT_LIST_DIR}/VolcanoInfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/VolcanoInfo.h

    ${NPP_COMMON_SPECTRA}
    ${NPP_COMMON_SPECTRUM_FORMAT}
    ${NPP_COMMON}
    ${NPP_COMMUNICATION}
    ${NPP_CONFIGURATION}
    ${NPP_EVALUATION}
    ${NPP_FIT}
    ${NPP_FLUX}
    ${NPP_GEOMETRY}
    ${NPP_METEOROLOGY}
    ${NPP_STRATOSPHERE}
    ${NPP_WINDMEASUREMENT}
)

target_include_directories(NovacPPP PRIVATE 
    ${PppLib_INCLUDE_DIRS}
    ${PocoNet_DIR}
    ${PocoFoundation_DIR})

target_link_libraries(NovacPPP PRIVATE PPPLib ${Poco_LIBRARIES})
    
# TODO: Also include /sdl and /WX
target_compile_options(NovacPPP PUBLIC /W3)

#Organize the files into folders, for ease of use
source_group("Common/Spectra"               FILES ${NPP_COMMON_SPECTRA})
source_group("Common/SpectrumFormat"        FILES ${NPP_COMMON_SPECTRUM_FORMAT})
source_group("Common"                       FILES ${NPP_COMMON})
source_group("Communication"                FILES ${NPP_COMMUNICATION})
source_group("Configuration"                FILES ${NPP_CONFIGURATION})
source_group("Evaluation"                   FILES ${NPP_EVALUATION})
source_group("Fit"                          FILES ${NPP_FIT})
source_group("Flux"                         FILES ${NPP_FLUX})
source_group("Geometry"                     FILES ${NPP_GEOMETRY})
source_group("Meteorology"                  FILES ${NPP_METEOROLOGY})
source_group("Stratosphere"                 FILES ${NPP_STRATOSPHERE})
source_group("WindMeasurement"              FILES ${NPP_WINDMEASUREMENT})

