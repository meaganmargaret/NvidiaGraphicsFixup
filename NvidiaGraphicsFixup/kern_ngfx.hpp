//
//  kern_ngfx.hpp
//  NvidiaGraphicsFixup
//
//  Copyright © 2017 lvs1974. All rights reserved.
//

#ifndef kern_ngfx_hpp
#define kern_ngfx_hpp

#include <Headers/kern_patcher.hpp>
#include <Library/LegacyIOService.h>

struct KextPatch {
    KernelPatcher::LookupPatch patch;
    uint32_t minKernel;
    uint32_t maxKernel;
};

class NGFX {
public:
	bool init();
	void deinit();
	
private:
    /**
     *  Patch kernel
     *
     *  @param patcher KernelPatcher instance
     */
    void processKernel(KernelPatcher &patcher);
    
	/**
	 *  Patch kext if needed and prepare other patches
	 *
	 *  @param patcher KernelPatcher instance
	 *  @param index   kinfo handle
	 *  @param address kinfo load address
	 *  @param size    kinfo memory size
	 */
	void processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
    
    /**
     *  SetAccelProperties callback type
     */
    using t_set_accel_properties = void (*) (IOService * that);

    /**
     *  AppleGraphicsDevicePolicy::start callback type
     */
    using t_agdp_start = bool (*) (IOService *that, IOService *);
    
    /**
     *  csfg_get_platform_binary callback type
     */
    using t_csfg_get_platform_binary = int (*) (void * fg);
    
    /**
     *  csfg_get_teamid callback type
     */
    using t_csfg_get_teamid = const char* (*) (void *fg);
	
	/**
	 *  NVDAStartupWeb::probe callback type
	 */
	using t_nvdastartup_probe = IOService* (*) (IOService *that, IOService * provider, SInt32 *score);
    
    
    /**
     *  Hooked methods / callbacks
     */
    static void nvAccelerator_SetAccelProperties(IOService* that);

    static bool AppleGraphicsDevicePolicy_start(IOService *that, IOService *provider);
	
	static IOService* NVDAStartupWeb_probe(IOService *that, IOService * provider, SInt32 *score);
    
    static int csfg_get_platform_binary(void *fg);

    
    /**
     *  Trampolines for original method invocations
     */
    t_set_accel_properties      orgSetAccelProperties {nullptr};
    
    t_agdp_start                orgAgdpStart {nullptr};
    
    t_csfg_get_platform_binary  org_csfg_get_platform_binary {nullptr};
    
    t_csfg_get_teamid           csfg_get_teamid {nullptr};
	
	t_nvdastartup_probe			orgNvdastartupProbe {nullptr};
    
    
    /**
     *  Apply kext patches for loaded kext index
     *
     *  @param patcher    KernelPatcher instance
     *  @param index      kinfo index
     *  @param patches    patch list
     *  @param patchesNum patch number
     */
    void applyPatches(KernelPatcher &patcher, size_t index, const KextPatch *patches, size_t patchesNum, const char *name);
	
	/**
	 *  Current progress mask
	 */
	struct ProcessingState {
		enum {
			NothingReady = 0,
			GraphicsDevicePolicyPatched = 2,
            GeForceRouted = 4,
            GeForceWebRouted = 8,
			NVDAStartupWebRouted = 16,
            KernelRouted = 32,
			EverythingDone = GraphicsDevicePolicyPatched | GeForceRouted | GeForceWebRouted | NVDAStartupWebRouted | KernelRouted,
		};
	};
    int progressState {ProcessingState::NothingReady};
    
    static constexpr const char* kNvidiaTeamId { "6KR3T733EC" };
};

#endif /* kern_ngfx */
