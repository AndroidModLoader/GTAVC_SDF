#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>

MYMODCFG(net.rusjj.gtavc.streamdistfix, GTA:VC Streaming Distance Fix, 1.0.1, RusJJ)
NEEDGAME(com.rockstargames.gtavc)

/* Saves */
void* pGTAVC = 0;
float fAspectRatioMultiplier = 1.0f;

ConfigEntry* pCfgFixStreamingForWideScreen;
ConfigEntry* pCfgSDScaleMin, *pCfgSDScaleMax;

DECL_HOOK(void*, CameraProcess, int self)
{
    void* ret = CameraProcess(self);
    if(pCfgFixStreamingForWideScreen->GetBool())
    {
        *(float*)(self + 240) *= fAspectRatioMultiplier * pCfgSDScaleMax->GetFloat(); // MAX
        *(float*)(self + 244) *= fAspectRatioMultiplier * pCfgSDScaleMin->GetFloat(); // MIN
    }
    else
    {
        *(float*)(self + 240) *= pCfgSDScaleMax->GetFloat(); // MAX
        *(float*)(self + 244) *= pCfgSDScaleMin->GetFloat(); // MIN
    }
    return ret;
}

extern "C" void OnModLoad()
{
    logger->SetTag("SDFix");
    pCfgFixStreamingForWideScreen = cfg->Bind("FixStreamingForWideScreen", true);
    pCfgSDScaleMin = cfg->Bind("StreamingDistanceScaleMin", 1.0f);
    pCfgSDScaleMax = cfg->Bind("StreamingDistanceScaleMax", 1.0f);
    pGTAVC = dlopen("libGTAVC.so", RTLD_LAZY);
    fAspectRatioMultiplier = 0.75f * ((float(*)())dlsym(pGTAVC, "_ZN5CDraw20CalculateAspectRatioEv"))();
    HOOK(CameraProcess, dlsym(pGTAVC, "_ZN7CCamera7ProcessEv"));
    // _ZN9CRenderer15ms_lodDistScaleE
}