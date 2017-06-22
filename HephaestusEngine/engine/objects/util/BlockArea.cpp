#include "stdafx.h"
#include "BlockArea.h"
#include "common\CommonUtil.h"

namespace objects::util {

	// Inits to have zero area
	BlockArea::BlockArea() : Min(1), Max(0) {}

	BlockArea::BlockArea(const glm::ivec3 & min, const glm::ivec3 & max) : Min(min), Max(max) {}

	// Area of 1, just enclosing the given block
	BlockArea::BlockArea(const glm::ivec3 & p) : Min(p), Max(p) {}

	// Adds p to both min and max
	BlockArea & BlockArea::operator+=(const glm::ivec3 & p){
		Min += p;
		Max += p;
	}

	// Subtracts P from min and max
	BlockArea & BlockArea::operator-=(const glm::ivec3 & p){
		Min -= p;
		Max -= p;
	}

	// Multiplies min and max by p
	BlockArea & BlockArea::operator*=(const glm::ivec3 & p){
		Min *= p;
		Max *= p;
	}

	// Divides min and max by p
	BlockArea & BlockArea::operator/=(const glm::ivec3 & p){
		Min /= p;
		Max /= p;
	}

	// Adds scalar f to vectors min/max
	BlockArea & BlockArea::operator+=(const float & f){
		Min += f;
		Max += f;
	}

	// Subtracts scalar f from vectors min/max
	BlockArea & BlockArea::operator-=(const float & f){
		Min -= f;
		Max -= f;
	}

	// Multiplies min/max by scalar f
	BlockArea & BlockArea::operator*=(const float & f){
		Min *= f;
		Max *= f;
	}

	// Divides min/max by scalar f
	BlockArea & BlockArea::operator/=(const float & f){
		Min /= f;
		Max /= f;
	}

	bool BlockArea::operator==(const BlockArea & other) const{
		return (Max == other.Max && Min == other.Min);
	}

	// Expands area to include other area.
	void BlockArea::Merge(const BlockArea & other){
		if (Empty()) {
			*this = other;
			return;
		}
		else {
			if (other.Min.x < Min.x) {
				Min.x = other.Min.x;
			}
			if (other.Min.y < Min.y) {
				Min.y = other.Min.y;
			}
			if (other.Min.z < Min.z) {
				Min.z = other.Min.z;
			}
			if (other.Max.x > Max.x) {
				Max.x = other.Max.x;
			}
			if (other.Max.y > Max.y) {
				Max.y = other.Max.y;
			}
			if (other.Max.z > Max.z) {
				Max.z = other.Max.z;
			}
		}
	}

	// Adds point to this area, by expanding area ot include this point.
	void BlockArea::AddPoint(const glm::ivec3 & p){
		if (Empty()) {
			Min = p;
			Max = p;
			return;
		}
		if (p.x < Min.x) {
			Min.x = p.x;
		}
		if (p.y < Min.y) {
			Min.y = p.y;
		}
		if (p.z < Min.z) {
			Min.z = p.z;
		}
		if (p.x > Max.x) {
			Max.x = p.x;
		}
		if (p.y > Max.y) {
			Max.y = p.y;
		}
		if (p.z > Max.z) {
			Max.z = p.z;
		}
	}

	// Adds padding p around this area.
	void BlockArea::AddPadding(const size_t & p){
		Min -= p;
		Max -= p;
	}

	// Dimensions of this area.
	glm::ivec3 BlockArea::GetSize() const{
		return Max - Min + glm::ivec3(1, 1, 1);
	}

	// Whether or not this area is empty.
	bool BlockArea::Empty() const{
		return (Max - Min) == glm::ivec3(-1, -1, -1);
	}

	// Get volume of this area (in blocks)
	size_t BlockArea::GetVolume() const{
		glm::ivec3 s = GetSize();
		return size_t(s.x * s.y * s.z);
	}

	bool BlockArea::Contains(const BlockArea & other) const{
		return false;
	}

	bool BlockArea::Contains(const glm::ivec3 & p) const{
		return false;
	}

	void BlockArea::Diff(const BlockArea & area, std::vector<BlockArea>& result){
	}

	size_t BlockArea::GetIndex(const glm::ivec3 & p) const{
		return size_t();
	}

	size_t BlockArea::TranslateIndexX(const glm::ivec3 & extent, size_t idx, size_t amt) const{
		return size_t();
	}

	size_t BlockArea::TranslateIndexY(const glm::ivec3 & extent, size_t idx, size_t amt) const{
		return size_t();
	}

	size_t BlockArea::TranslateIndexZ(const glm::ivec3 & extent, size_t idx, size_t amt) const{
		return size_t();
	}

	size_t BlockArea::TranslateIndex(const glm::ivec3 & extent, size_t idx, const glm::ivec3 & amt){
		return size_t();
	}


	BlockManipulator::BlockManipulator(){
	}

	Block BlockManipulator::GetBlock(const glm::ivec3 & p) const{
		return Block();
	}

	bool BlockManipulator::Exists(const glm::ivec3 & p) const{
		return false;
	}

	Block & BlockManipulator::GetBlockRef(const glm::ivec3 & p){
		// TODO: insert return statement here
	}

	void BlockManipulator::SetBlock(const glm::ivec3 & p, const Block & block){
	}

	void BlockManipulator::ClearFlag(uint8_t flag){
	}

	void BlockManipulator::AddArea(const BlockArea & area){
	}

	void BlockManipulator::CopyFrom(const std::vector<Block>& src, const BlockArea & src_area, const glm::ivec3 & from, const glm::ivec3 & to, const glm::ivec3 & dims){
	}

	void BlockManipulator::CopyTo(std::vector<Block>& dest, const BlockArea & dest_area, const glm::ivec3 & dest_pos, const glm::ivec3 & from, const glm::ivec3 & dims){
	}

	void SpreadLight(bool day, glm::ivec3& p){
	}

	void SpreadLight(bool day, std::set<glm::ivec3>& nodes){
	}

	void UnspreadLight(bool day, glm::ivec3 & p, uint8_t old_light_level, std::set<glm::ivec3>& light_sources){
	}

	void UnspreadLight(bool day, std::map<glm::ivec3, uint8_t>& from_nodes){
	}

	void SetLight(BlockManipulator & manip, BlockArea & area, uint8_t light){
	}

	void ClearAndCollectLightSrcs(BlockManipulator & manip, BlockArea & area, bool day, std::set<glm::ivec3>& light_sources, std::map<glm::ivec3, uint8_t>& unlight_from){
	}

	bool PropagateSunlight(BlockManipulator & manip, BlockArea & area, bool active_sunlight, std::set<glm::ivec3>& light_sources){
		return false;
	}

	void UpdateLighting(Region & region, std::vector<std::pair<glm::ivec3, Block>>& old_blocks, std::map<glm::ivec3, Chunk>& modified_chunks){
	}

}