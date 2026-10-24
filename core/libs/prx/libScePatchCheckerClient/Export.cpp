#include <cstdint>
#include <cstddef>
#include <atomic>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {

constexpr int32_t kOk = 0;
constexpr int32_t kErrorInvalidArgument = static_cast<int32_t>(0x80020016);
constexpr int32_t kErrorInvalidHandle = static_cast<int32_t>(0x80020009);
constexpr int32_t kMaxHandles = 64;

std::atomic<bool> g_initialized{false};
std::atomic<bool> g_autoDownloadEnabled{true};
std::atomic<bool> g_handleUsed[kMaxHandles];

bool IsValidHandle(int32_t handle) {
    if (handle < 1 || handle > kMaxHandles) {
        return false;
    }
    return g_handleUsed[handle - 1].load();
}

}

extern "C" {

int APS5_VABI scePatchCheckerInitialize() {
    g_initialized.store(true);
    g_autoDownloadEnabled.store(true);
    return kOk;
}

int APS5_VABI scePatchCheckerTerminate() {
    for (int32_t i = 0; i < kMaxHandles; ++i) {
        g_handleUsed[i].store(false);
    }
    g_initialized.store(false);
    return kOk;
}

int APS5_VABI scePatchCheckerCreateHandler(int32_t* outHandle) {
    if (outHandle == nullptr) {
        return kErrorInvalidArgument;
    }
    for (int32_t i = 0; i < kMaxHandles; ++i) {
        bool expected = false;
        if (g_handleUsed[i].compare_exchange_strong(expected, true)) {
            *outHandle = i + 1;
            return kOk;
        }
    }
    return kErrorInvalidArgument;
}

int APS5_VABI scePatchCheckerDestroyHandler(int32_t handle) {
    if (handle < 1 || handle > kMaxHandles) {
        return kErrorInvalidHandle;
    }
    bool expected = true;
    if (!g_handleUsed[handle - 1].compare_exchange_strong(expected, false)) {
        return kErrorInvalidHandle;
    }
    return kOk;
}

int APS5_VABI scePatchCheckerCancel(int32_t handle) {
    if (!IsValidHandle(handle)) {
        return kErrorInvalidHandle;
    }
    return kOk;
}

int APS5_VABI scePatchCheckerCheckPatch(int32_t handle) {
    if (!IsValidHandle(handle)) {
        return kErrorInvalidHandle;
    }
    return kOk;
}

int APS5_VABI scePatchCheckerRequestCheckPatch(int32_t handle) {
    if (!IsValidHandle(handle)) {
        return kErrorInvalidHandle;
    }
    return kOk;
}

int APS5_VABI scePatchCheckerRequestCheckPatchByType(int32_t handle, int32_t type) {
    (void)type;
    if (!IsValidHandle(handle)) {
        return kErrorInvalidHandle;
    }
    return kOk;
}

int APS5_VABI scePatchCheckerEnableAutoDownload() {
    g_autoDownloadEnabled.store(true);
    return kOk;
}

int APS5_VABI scePatchCheckerDisableAutoDownload() {
    g_autoDownloadEnabled.store(false);
    return kOk;
}

int APS5_VABI scePatchCheckerGetApplicableTick(int32_t handle, uint64_t* outTick) {
    if (!IsValidHandle(handle)) {
        return kErrorInvalidHandle;
    }
    if (outTick != nullptr) {
        *outTick = 0;
    }
    return kOk;
}

int APS5_VABI scePatchCheckerGetPackageInfo(int32_t handle, void* outInfo) {
    (void)outInfo;
    if (!IsValidHandle(handle)) {
        return kErrorInvalidHandle;
    }
    return kOk;
}

}