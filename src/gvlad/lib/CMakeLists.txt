SET(gvlad_lib_sources
	Angle.h Angle.cpp
	Classifier.h Classifier.cpp
	Compressor.h Compressor.cpp
	DetectorDescriptor.h DetectorDescriptor.cpp
	Descriptor.h Descriptor.cpp
	File.h	File.cpp
	GeoDescriptor.h
	GeoDescriptorVector.h
	GeometricVerifier.h GeometricVerifier.cpp
	GlobalConfig.h GlobalConfig.cpp
	KnnClassifier.h KnnClassifier.cpp
	Math.h Math.cpp
	RankItem.h
	Serializer.h Serializer.cpp
	SVMClassifier.h SVMClassifier.cpp
	Timer.h Timer.cpp
	Vocabulary.h Vocabulary.cpp
)
ADD_LIBRARY(gvlad_lib ${gvlad_lib_sources})

TARGET_LINK_LIBRARIES(gvlad_lib ${Boost_LIBRARIES} ${OpenCV_LIBS})
