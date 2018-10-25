#pragma once
#ifndef HEPHAESTUS_ENGINE_VOXEL_SAMPLER_HPP
#define HEPHAESTUS_ENGINE_VOXEL_SAMPLER_HPP
#include "glm/vec3.hpp"
#include "VoxelVolume.hpp"

template<typename VoxelType>
struct VoxelSampler {

    constexpr VoxelSampler() noexcept = default;
    constexpr ~VoxelSampler() noexcept = default;
    constexpr VoxelSampler(glm::ivec3 p, VoxelVolume* volume) noexcept : Position{ std::move(p) }, Volume{ volume } {}

    glm::ivec3 Position{ 0, 0, 0 };
    VoxelVolume* Volume{ nullptr };

    VoxelType Get() const;    
    void Set(VoxelType v) noexcept;
    template<int PeekX, int PeekY, int PeekZ>
    VoxelType Peek() const;

};

#endif //!HEPHAESTUS_ENGINE_VOXEL_SAMPLER_HPP
