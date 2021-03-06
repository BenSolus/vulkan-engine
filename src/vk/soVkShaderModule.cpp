/*
 * Copyright (C) 2017-2018 by Bennet Carstensen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "soVkShaderModule.hpp"

#include "soDebugCallback.hpp"
#include "soFileSystem.hpp"

so::vk::ShaderModule::ShaderModule()
  : mShaderModule(VK_NULL_HANDLE),
    mDevice(LogicalDevice::getSharedPtrNullDevice())
{}

so::vk::ShaderModule::ShaderModule(SharedPtrLogicalDevice const& device,
                                   std::string            const& file)
  : mShaderModule(VK_NULL_HANDLE), mDevice(device)
{
  std::vector<char> shaderCode;

  if(readBinaryFile(file, shaderCode) is_eq failure)
  {
    std::string message{ "Failed to load shader code from binary file'" };
    
    message += file;
    message += "'";

    DEBUG_CALLBACK(error, message, readBinaryFile);

    return;
  }

  VkShaderModuleCreateInfo createInfo{};

  createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = shaderCode.size();
  createInfo.pCode    = reinterpret_cast<uint32_t const*>(shaderCode.data());

  VkResult const result(vkCreateShaderModule(device->getVkDevice(),
                                             &createInfo,
                                             nullptr,
                                             &mShaderModule));

  if(result not_eq VK_SUCCESS)
  {
    DEBUG_CALLBACK(error,
                   "Failed to create shader module.",
                   vkCreateShaderModule);

    mShaderModule = VK_NULL_HANDLE;
  }
}

so::vk::ShaderModule::~ShaderModule() noexcept
{
  destroy_members();
}
   
so::vk::ShaderModule&
so::vk::ShaderModule::operator=(ShaderModule&& other) noexcept
{
	if(this is_eq &other)
  {
    return *this;
  }

  destroy_members();

  mShaderModule = other.mShaderModule;
  mDevice       = other.mDevice;

  other.mShaderModule = VK_NULL_HANDLE;
  other.mDevice       = LogicalDevice::getSharedPtrNullDevice();

  return *this;
}


void
so::vk::ShaderModule::destroy_members()
{
  VkDevice device{ mDevice->getVkDevice() };

  if((mShaderModule not_eq VK_NULL_HANDLE) and (device not_eq VK_NULL_HANDLE))
  {
    vkDestroyShaderModule(device, mShaderModule, nullptr);
  }
}

