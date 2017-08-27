/*
 * Copyright (c) 2017 Bennet Carstensen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**
 *  @file      VkCommandPool.hpp
 *  @author    Bennet Carstensen
 *  @date      2017
 *  @copyright Copyright (c) 2017 Bennet Carstensen
 *
 *             This Source Code Form is subject to the terms of the
 *             Mozilla Public License, v. 2.0. If a copy of the MPL was
 *             not distributed with this file, You can obtain one at
 *             http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <vk/soVkLogicalDevice.hpp>
#include <vk/soVkSurface.hpp>

namespace so
{
  namespace vk
  {
    class
    CommandPool : public std::enable_shared_from_this<CommandPool>
    {
      public:
        CommandPool();

        CommandPool(SharedPtrLogicalDevice const& device,
                    Surface&                      surface);

        CommandPool(SharedPtrLogicalDevice const& device,
                    VkCommandPool                 commandPool);

        CommandPool(CommandPool& other) = delete;

        CommandPool(CommandPool&& other) = delete;

        ~CommandPool() noexcept;

        CommandPool&
        operator=(CommandPool const& other) = delete;

        CommandPool&
        operator=(CommandPool&& other) noexcept
        {
          destroyMembers();

          mCommandPool = other.mCommandPool;
          mDevice      = other.mDevice;

          other.mCommandPool = VK_NULL_HANDLE;
          other.mDevice      = SHARED_PTR_NULL_LOGICAL_DEVICE;

          return *this;
        }

        inline auto getVkCommandPool() { return mCommandPool; }

      private:
        VkCommandPool          mCommandPool;

        SharedPtrLogicalDevice mDevice;

        void
        destroyMembers();
    };

    using SharedPtrCommandPool = std::shared_ptr<CommandPool>;
  } // namespace vk
} // namespace so

/**
 *
 * @todo When using default constructor, an additional std::shared_ptr is
 *       generated from @ref SHARED_PTR_NULL_LOGICAL_DEVICE as member @p
 *       mDevice. However, SHARED_PTR_NULL_LOGICAL_DEVICE seems to get destroyed
 *       before SHARED_PTR_NULL_COMMAND_POOL. Current workaround is defining
 *       an additional constructor which allows setting all member variables,
 *       defining another "invalid" instance of fcm::vk::SharedPtrLogicalDevice
 *       identical to SHARED_PTR_NULL_LOGICAL_DEVICE and setting the other
 *       member identical to the default constructor. Needless to say that
 *       that defining an additional constructor and creating an additional
 *       instance is kind of ugly...
 */
extern so::vk::SharedPtrCommandPool const SHARED_PTR_NULL_COMMAND_POOL;