#pragma once
#define CL_TARGET_OPENCL_VERSION 200
#include <CL/cl.h>
#include <iostream>
#include <vector>

inline int countGPUThreads() {
    cl_uint platformCount;
    clGetPlatformIDs(0, nullptr, &platformCount);
    std::vector<cl_platform_id> platforms(platformCount);
    clGetPlatformIDs(platformCount, platforms.data(), nullptr);
    size_t output = 0;

    for (const auto platform : platforms) {
        cl_uint deviceCount;
        clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &deviceCount);
        std::vector<cl_device_id> devices(deviceCount);
        clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, deviceCount, devices.data(), nullptr);

        for (const auto device : devices) {
            char name[128];
            size_t computeUnits;
            size_t maxWorkGroupSize;
            clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, nullptr);
            clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, nullptr);
            clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, nullptr);

            output = computeUnits * maxWorkGroupSize;

            std::cout << "GPU: " << name
                      << " | Wątki: " << computeUnits * maxWorkGroupSize
                      << std::endl;
        }
    }
    return static_cast<int>(output);
}
