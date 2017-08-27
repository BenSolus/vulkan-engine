/*
 * Copyright (c) 2017 Bennet Carstensen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vk/soVkUniformBuffer.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <random>

static glm::vec3 rotations[125];
static glm::vec3 rotationSpeeds[125];

so::vk::UniformBuffer::UniformBuffer()
  : mViewProjectionUBO({}),
    mViewProjectionUBOBuffer(),
    mDynamicUBOs(),
    mDynamicBuffer(),
    mDevice(SHARED_PTR_NULL_LOGICAL_DEVICE)
{}

so::vk::UniformBuffer::UniformBuffer
  (SharedPtrLogicalDevice const& device,
   const std::size_t             dynamicAlignment,
   const std::size_t             numDynamicUBOs)
  : mViewProjectionUBO({}),
    mViewProjectionUBOBuffer(device,
                             sizeof(ViewProjectionUBO),
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    mDynamicUBOs(dynamicAlignment, numDynamicUBOs),
    mDynamicBuffer(device,
                   numDynamicUBOs * dynamicAlignment,
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
    mDevice(device)
{
  if(numDynamicUBOs not_eq 0)
  {
    static auto startTime(std::chrono::high_resolution_clock::now());

    auto currentTime(std::chrono::high_resolution_clock::now());

    float time =
      static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>
        (currentTime - startTime).count()) / 1000.0f;

    std::mt19937 rndGen(static_cast<uint32_t>(time));

    std::normal_distribution<float> rndDist(-1.0f, 1.0f);

    for (uint32_t i = 0; i < 125; i++)
    {
      rotations[i] = glm::vec3(rndDist(rndGen), rndDist(rndGen), rndDist(rndGen)) * 2.0f * (float)M_PI;
      rotationSpeeds[i] = glm::vec3(rndDist(rndGen), rndDist(rndGen), rndDist(rndGen));
    }
  }
}

void
so::vk::UniformBuffer::updateViewProjectionUBO(VkExtent2D swapChainExtent)
{
  mViewProjectionUBO.view  = glm::lookAt(glm::vec3(0.0f, -30.0f, -30.0f),
                                         glm::vec3(0.0f, 0.0f, 0.0f),
                                         glm::vec3(0.0f, -1.0f, 0.0f));

  mViewProjectionUBO.proj  = glm::perspective
    (glm::radians(45.0f),
     static_cast<float>(swapChainExtent.width) /
     static_cast<float>(swapChainExtent.height),
     0.1f,
     50.0f);

  mViewProjectionUBO.proj[1][1] *= -1;

  VkDevice device(mDevice->getVkDevice());

  void* data;

  vkMapMemory(device,
              mViewProjectionUBOBuffer.getVkDeviceMemory(),
              0,
              sizeof(ViewProjectionUBO),
              0,
              &data);

  memcpy(data, &mViewProjectionUBO, sizeof(ViewProjectionUBO));

  vkUnmapMemory(device, mViewProjectionUBOBuffer.getVkDeviceMemory());
}

void
so::vk::UniformBuffer::update()
{
  static auto startTime(std::chrono::high_resolution_clock::now());

  auto currentTime(std::chrono::high_resolution_clock::now());

  float time =
    static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>
                         (currentTime - startTime).count()) / 100000000.0f;

  uint32_t dim = static_cast<uint32_t>(pow(125, (1.0f / 3.0f)));
  glm::vec3 offset(5.0f);

  for (uint32_t x = 0; x < dim; x++)
  {
    for (uint32_t y = 0; y < dim; y++)
    {
      for (uint32_t z = 0; z < dim; z++)
      {
        uint32_t index = x * dim * dim + y * dim + z;

        DynamicUBO dynamicUBO({});

        // Update rotations
        rotations[index] += time * rotationSpeeds[index];

        // Update matrices
        glm::vec3 pos = glm::vec3(-((dim * offset.x) / 2.0f) + offset.x / 2.0f + x * offset.x, -((dim * offset.y) / 2.0f) + offset.y / 2.0f + y * offset.y, -((dim * offset.z) / 2.0f) + offset.z / 2.0f + z * offset.z);
        dynamicUBO.model = glm::translate(glm::mat4(), pos);
        dynamicUBO.model = glm::rotate(dynamicUBO.model, rotations[index].x, glm::vec3(1.0f, 1.0f, 0.0f));
        dynamicUBO.model = glm::rotate(dynamicUBO.model, rotations[index].y, glm::vec3(0.0f, 1.0f, 0.0f));
        dynamicUBO.model = glm::rotate(dynamicUBO.model, rotations[index].z, glm::vec3(0.0f, 0.0f, 1.0f));

        mDynamicUBOs[index] = dynamicUBO;
      }
    }
  }

  VkDevice device(mDevice->getVkDevice());

  void* data;

  vkMapMemory(device,
              mDynamicBuffer.getVkDeviceMemory(),
              0,
              mDynamicUBOs.alignment() * mDynamicUBOs.size(),
              0,
              &data);

  memcpy(data,
         mDynamicUBOs.data(),
         mDynamicUBOs.alignment() * mDynamicUBOs.size());

  VkMappedMemoryRange memoryRange({ });

  memoryRange.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  memoryRange.pNext  = nullptr;
  memoryRange.memory = mDynamicBuffer.getVkDeviceMemory();
  memoryRange.offset = 0;
  memoryRange.size   = mDynamicUBOs.alignment() * mDynamicUBOs.size();

  vkFlushMappedMemoryRanges(device, 1, &memoryRange);

  vkUnmapMemory(device, mDynamicBuffer.getVkDeviceMemory());
}