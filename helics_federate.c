/*
 * Copyright (c) 2024-2024, Lawrence Livermore National Security
 * See the top-level NOTICE for additional details. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define S_FUNCTION_NAME helics_federate /* Defines and Includes */
#define S_FUNCTION_LEVEL 2


#include "simstruc.h"
#include "helics/helics.h"
#include "math.h"

static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 1);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch reported by the Simulink engine*/
    }

    if (!ssSetNumInputPorts(S, 0)) return;

    if (!ssSetNumOutputPorts(S,1)) return;
    ssSetOutputPortWidth(S, 0, 3);
    ssSetOutputPortDataType(S,0,SS_DOUBLE);

    ssSetNumSampleTimes(S, 1);

    
    /* Set this S-function as runtime thread-safe for multicore execution */
    ssSetRuntimeThreadSafetyCompliance(S, RUNTIME_THREAD_SAFETY_COMPLIANCE_TRUE);
    ssSetOptions(S,SS_OPTION_CALL_TERMINATE_ON_EXIT);
    
    ssSetNumDWork(S,2);
    ssSetDWorkDataType(S, 0, SS_DOUBLE);
    ssSetDWorkDataType(S, 1, SS_BOOLEAN);
    ssSetDWorkName(S,0,"timing");
    ssSetDWorkName(S,1,"control");
    ssSetDWorkWidth(S, 0, 3);
    ssSetDWorkWidth(S, 1, 2);
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
    
    ssSetSampleTime(S, 0, VARIABLE_SAMPLE_TIME);
}

#define MDL_GET_TIME_OF_NEXT_VAR_HIT
static void mdlGetTimeOfNextVarHit(SimStruct *S)
  {
    real_T *timing = (real_T*) ssGetDWork(S,0);
  	time_T timeOfNextHit = timing[0] + timing[2];
  	ssSetTNext(S, timeOfNextHit);
  }

#define MDL_START
void mdlStart(SimStruct *S)
{
    const mxArray *file_name_param=ssGetSFcnParam(S, 0);
    char *file_name=mxArrayToString(file_name_param);

    ssPrintf("trying to load file %s\n",file_name);
    HelicsError error= helicsErrorInitialize();
    HelicsFederate fed= helicsCreateCombinationFederateFromConfig(file_name,&error);

    if (helicsFederateIsValid(fed))
    {
        ssPrintf("loaded Federate %s\n",helicsFederateGetName(fed));
    }
    SimStruct *root_ss=ssGetRootSS(S);
    ssSetUserData(root_ss,fed);
    helicsFederateEnterInitializingMode(fed,NULL);
   real_T *timing = (real_T*) ssGetDWork(S,0);
   timing[0]=-1.0;
   timing[2]=helicsFederateGetTimeProperty(fed,HELICS_PROPERTY_TIME_PERIOD,NULL);
    ssPrintf("time period = %f\n",timing[2]);
}


#define MDL_INITIALIZE_CONDITIONS
void mdlInitializeConditions(SimStruct *S)
{
     SimStruct *root_ss=ssGetRootSS(S);
    HelicsFederate fed=ssGetUserData(root_ss);
     helicsFederateEnterExecutingMode(fed,NULL);
     real_T *timing = (real_T*) ssGetDWork(S,0);
    timing[0]=0.0;
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
    int_T i;
    time_T time=ssGetT(S);
    real_T *y = ssGetOutputPortRealSignal(S,0);
    int_T width = ssGetOutputPortWidth(S,0);

    real_T *timing = (real_T*) ssGetDWork(S,0);
    *y++ = timing[0];
    for (i=1; i<width; i++) {
        *y++ = sin(0.77552*2.0*3.14159265*time*(double)(i+1));
    }
}

#define MDL_UPDATE
void mdlUpdate(SimStruct *S, int_T tid)
{
     SimStruct *root_ss=ssGetRootSS(S);
     HelicsFederate fed=ssGetUserData(root_ss);
     double timeOut=helicsFederateRequestTime(fed,0,NULL);
     ssPrintf("federate at %f\n",timeOut);
     real_T *timing = (real_T*) ssGetDWork(S,0);
     timing[0]=timeOut;
}

static void mdlTerminate(SimStruct *S)
{
    SimStruct *root_ss=ssGetRootSS(S);
    HelicsFederate fed=ssGetUserData(root_ss);
    helicsFederateDestroy(fed);
     ssPrintf("federate terminated\n");

}

#define MDL_CLEANUP_RUNTIME_RESOURCES
static void mdlCleanupRuntimeResources(SimStruct *S)
{
  int i;
  for (i = 0; i<ssGetNumPWork(S); i++) {
    if (ssGetPWorkValue(S,i) != NULL) {
      free(ssGetPWorkValue(S,i));
    }
  }
}
#include "simulink.c" /* MEX-file interface mechanism */