 /*
  * Copyright (C) 2015-2018 NXP Semiconductors
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *      http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <data_types.h>
#include "JcDnld.h"
#include "JcopOsDownload.h"

using android::base::StringPrintf;

JcopOsDwnld *jd;
IChannel_t *channel;
static bool inUse = false;
static int16_t jcHandle;
extern pJcopOs_Dwnld_Context_t gpJcopOs_Dwnld_Context;
/*******************************************************************************
**
** Function:        JCDNLD_Init
**
** Description:     Initializes the JCOP library and opens the DWP communication channel
**
** Returns:         true if ok.
**
*******************************************************************************/
tJBL_STATUS JCDNLD_Init(IChannel_t *channel)
{
    static const char fn[] = "JCDNLD_Init";
    bool    stat = false;
    jcHandle = EE_ERROR_OPEN_FAIL;
    DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter", fn);

    if (inUse == true)
    {
        return STATUS_INUSE;
    }
    else if(channel == NULL)
    {
        return STATUS_FAILED;
    }
    /*TODO: inUse assignment should be with protection like using semaphore*/
    inUse = true;
    jd = JcopOsDwnld::getInstance();
    stat = jd->initialize (channel);
    if(stat != true)
    {
        LOG(ERROR) << StringPrintf("%s: failed", fn);
    }
    else
    {
        if((channel != NULL) &&
           (channel->open) != NULL)
        {
            jcHandle = channel->open();
            if(jcHandle == EE_ERROR_OPEN_FAIL)
            {
                LOG(ERROR) << StringPrintf("%s:Open DWP communication is failed", fn);
                stat = false;
            }
            else
            {
                LOG(ERROR) << StringPrintf("%s:Open DWP communication is success", fn);
                stat = true;
            }
        }
        else
        {
            LOG(ERROR) << StringPrintf("%s: NULL DWP channel", fn);
            stat = false;
        }
    }
    return (stat == true)?STATUS_OK:STATUS_FAILED;
}

/*******************************************************************************
**
** Function:        JCDNLD_StartDownload
**
** Description:     Starts the JCOP update
**
** Returns:         SUCCESS if ok.
**
*******************************************************************************/
tJBL_STATUS JCDNLD_StartDownload()
{
    static const char fn[] = "JCDNLD_StartDownload";
    DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Enter", fn);
    tJBL_STATUS status = STATUS_FAILED;
    status = jd->JcopOs_Download();
    DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Exit; status=0x0%X", fn, status);
    return status;
}

/*******************************************************************************
**
** Function:        JCDNLD_DeInit
**
** Description:     Deinitializes the JCOP Lib
**
** Returns:         true if ok.
**
*******************************************************************************/
bool JCDNLD_DeInit()
{
    static const char fn[] = "JCDNLD_DeInit";
    bool    stat = false;
    DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter", fn);

    if(gpJcopOs_Dwnld_Context != NULL)
    {
        channel = gpJcopOs_Dwnld_Context->channel;
        if((channel != NULL) && (channel->doeSE_JcopDownLoadReset != NULL))
        {
            channel->doeSE_JcopDownLoadReset();
            if(channel->close != NULL)
            {
                stat = channel->close(jcHandle);
                if(stat != true)
                {
                    LOG(ERROR) << StringPrintf("%s:closing DWP channel is failed", fn);
                }
            }
            else
            {
                LOG(ERROR) << StringPrintf("%s: NULL fp DWP_close", fn);
                stat = false;
            }
        }
    }
    else
    {
        LOG(ERROR) << StringPrintf("%s: NULL dwnld context", fn);
    }
    jd->finalize();
    /*TODO: inUse assignment should be with protection like using semaphore*/
    inUse = false;
    return stat;
}

/*******************************************************************************
**
** Function:        JCDNLD_CheckVersion
**
** Description:     Check the existing JCOP OS version
**
** Returns:         true if ok.
**
*******************************************************************************/
bool JCDNLD_CheckVersion()
{

    /**
     * Need to implement in case required
     * */
    return true;
}
