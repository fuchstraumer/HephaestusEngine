#ifndef ARRAY3_H
#define ARRAY3_H
#include <stdint.h>
#include <assert.h>
template<uint32_t noOfDims, typename ElementType>
class Array {
public:
	Array(uint32_t width) : Elements(0) {
		static_assert(noOfDims == 1, "This constructor is only for 1D arrays");
		Dimensions[0] = width;
		init();
	}
	Array(uint32_t width, uint32_t length) : Elements(0) {
		static_assert(noOfDims == 2, "This constructor is only for 2D arrays");
		Dimensions[0] = width;
		Dimensions[1] = length;
		init();
	}
	Array(uint32_t width, uint32_t length, uint32_t height) : Elements(0) {
		static_assert(noOfDims == 3, "This constructor is only for 3D arrays");
		Dimensions[0] = width;
		Dimensions[1] = length;
		Dimensions[2] = height;
		init();
	}

	// Ensure accidental copying can't occur
	Array<noOfDims, ElementType>(const Array<noOfDims, ElementType>&) = delete;
	Array<noOfDims, ElementType>& operator=(const Array<noOfDims, ElementType>&) = delete;

	ElementType& operator()(uint32_t x) const {
		static_assert(noOfDims == 1, "This accessor is only for 1D arrays");
		//assert(x < Dimensions[0], "This index is out of range");
		return Elements[x];
	}
	ElementType& operator()(uint32_t x, uint32_t y) const {
		static_assert(noOfDims == 2, "This accessor is only for 2D arrays");
		//assert(x < Dimensions[0] && y < Dimensions[1], "This index is out of range");
		return Elements[y*Dimensions[0 + x]];
	}
	ElementType& operator()(uint32_t x, uint32_t y, uint32_t z) const {
		static_assert(noOfDims == 3, "This accessor is only for 3D arrays");
		//assert(x < Dimensions[0] && y < Dimensions[1] && z < Dimensions[2]);
		return Elements[z*Dimensions[0] * Dimensions[1] + y*Dimensions[0] + x];
	}
	uint32_t getDimensions(uint32_t dimension) const {
		return Dimensions[dimension];
	}
	ElementType* getRawData() {
		return Elements;
	}
	uint32_t getNumElements() {
		return NumElements;
	}
	void swap(Array& other) {
		ElementType* temp = other.Elements;
		other.Elements = Elements;
		Elements = temp;
	}
private:
	void init(void) {
		// find number of elements in array
		NumElements = 1;
		for (uint32_t i = 0; i < noOfDims; ++i) {
			NumElements *= Dimensions[i];
		}
		Elements = new ElementType[NumElements];
	}
	uint32_t Dimensions[noOfDims];
	uint32_t NumElements;
	ElementType * Elements;
};

///A 1D Array of floats.
typedef Array<1, float> Array1DF;
///A 1D Array of doubles.
typedef Array<1, double> Array1DD;
///A 1D Array of signed 8-bit values.
typedef Array<1, int8_t> Array1D8;
///A 1D Array of unsigned 8-bit values.
typedef Array<1, uint8_t> Array1DUI8;
///A 1D Array of signed 16-bit values.
typedef Array<1, int16_t> Array1DI16;
///A 1D Array of unsigned 16-bit values.
typedef Array<1, uint16_t> Array1DUI16;
///A 1D Array of signed 32-bit values.
typedef Array<1, int32_t> Array1DI32;
///A 1D Array of unsigned 32-bit values.
typedef Array<1, uint32_t> Array1DUI32;
/// A 1D array of booleans
typedef Array<1, bool> Array1DB;


///A 2D Array of floats.
typedef Array<2, float> Array2DF;
///A 2D Array of doubles.
typedef Array<2, double> Array2DD;
///A 2D Array of signed 8-bit values.
typedef Array<2, int8_t> Array2DI8;
///A 2D Array of unsigned 8-bit values.
typedef Array<2, uint8_t> Array2DUI8;
///A 2D Array of signed 16-bit values.
typedef Array<2, int16_t> Array2DI16;
///A 2D Array of unsigned 16-bit values.
typedef Array<2, uint16_t> Array2DUI16;
///A 2D Array of signed 32-bit values.
typedef Array<2, int32_t> Array2DI32;
///A 2D Array of unsigned 32-bit values.
typedef Array<2, uint32_t> Array2DUI32;
///A 2D array of booleans
typedef Array<2, bool> Array2DB;

///A 3D Array of floats.
typedef Array<3, float> Array3DF;
///A 3D Array of doubles.
typedef Array<3, double> Array3DD;
///A 3D Array of signed 8-bit values.
typedef Array<3, int8_t> Array3DI8;
///A 3D Array of unsigned 8-bit values.
typedef Array<3, uint8_t> Array3DUI8;
///A 3D Array of signed 16-bit values.
typedef Array<3, int16_t> Array3DI16;
///A 3D Array of unsigned 16-bit values.
typedef Array<3, uint16_t> Array3DUI16;
///A 3D Array of signed 32-bit values.
typedef Array<3, int32_t> Array3DI32;
///A 3D Array of unsigned 32-bit values.
typedef Array<3, uint32_t> Array3DUI32;
///A 3D Array of booleans
typedef Array<3, bool> Array3DB;

#endif // !ARRAY3_H
