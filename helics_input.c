/*
 * Copyright (c) 2024-2024, Lawrence Livermore National Security
 * See the top-level NOTICE for additional details. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define S_FUNCTION_NAME helics_input /* Defines and Includes */
#define S_FUNCTION_LEVEL 2


#include "simstruc.h"
#include "helics/helics.h"
#include "math.h"
#include <string.h>

static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 2);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch reported by the Simulink engine*/
    }

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S,0,3);

    ssSetInputPortDataType(S, 0, SS_DOUBLE );

    if (!ssSetNumOutputPorts(S,1)) return;
    const mxArray *params=ssGetSFcnParam(S, 1);
    char *param_string=mxArrayToString(params);
    int output_width=1;
    int dataType=HELICS_DATA_TYPE_DOUBLE;

    char *token=NULL;
    token=strtok(param_string," ,");
    while (token){
        ssPrintf("got token %s\n",token);
        if (strncmp(token,"output_size=",12)==0)
        {
            output_width=atoi(token+12);
            ssPrintf("got token output_size read as %s converted to %d\n",token+12,output_width);
        }
        else if (strncmp(token,"output_type=",12)==0)
        {
            dataType=helicsGetDataType(token+12);
            ssPrintf("got token output_type read as %s converted to %d\n",token+12,dataType);

        }
        token=strtok(NULL," ,");
    }
    ssSetOutputPortWidth(S, 0, output_width);
    switch (dataType)
    {
        case HELICS_DATA_TYPE_DOUBLE: case HELICS_DATA_TYPE_VECTOR:
            ssSetOutputPortDataType(S, 0, SS_DOUBLE);
            break;
        case HELICS_DATA_TYPE_INT:
            ssSetOutputPortDataType(S, 0, SS_INT32);
            break;
        case HELICS_DATA_TYPE_BOOLEAN:
            ssSetOutputPortDataType(S, 0, SS_BOOLEAN);
            break;
        case HELICS_DATA_TYPE_COMPLEX: case HELICS_DATA_TYPE_COMPLEX_VECTOR:
            ssSetOutputPortDataType(S, 0, SS_DOUBLE);
            //something with complex indications
            break;
    }

    ssSetInputPortRequiredContiguous(S,0,1);
    /* sample times */
    ssSetNumSampleTimes(   S, 1 );
    /* Set this S-function as runtime thread-safe for multicore execution */
    ssSetRuntimeThreadSafetyCompliance(S, RUNTIME_THREAD_SAFETY_COMPLIANCE_TRUE);
    ssSetOptions(S,SS_OPTION_CALL_TERMINATE_ON_EXIT);
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime( S, 0, INHERITED_SAMPLE_TIME );
    ssSetOffsetTime( S, 0, FIXED_IN_MINOR_STEP_OFFSET );
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
}

#define MDL_START
void mdlStart(SimStruct *S)
{


}


#define MDL_INITIALIZE_CONDITIONS
void mdlInitializeConditions(SimStruct *S)
{
    SimStruct *root_ss=ssGetRootSS(S);
     HelicsFederate fed=ssGetUserData(root_ss);
     if (!helicsFederateIsValid(fed)){
        ssPrintf("HELICS federate is not valid\n");
        ssSetErrorStatus(S,"HELICS federate is not valid");
        return;
     }


     const mxArray *index_param=ssGetSFcnParam(S, 0);
     mxClassID paramType=mxGetClassID(index_param);
     HelicsInput inp;
    switch (paramType)
    {
        case mxCHAR_CLASS:
        {
            char *input_name=mxArrayToString(index_param);
            HelicsError err=helicsErrorInitialize();
            inp=helicsFederateGetInput(fed,input_name,&err);
            if (err.error_code!=HELICS_OK)
            {
                ssSetErrorStatus(S,err.message);
                return;
            }}
            break;
        case mxDOUBLE_CLASS:
        {
            double input_index=mxGetScalar(index_param);
            HelicsError err=helicsErrorInitialize();
            inp=helicsFederateGetInputByIndex(fed,(int32_t)(input_index),&err);
            if (err.error_code!=HELICS_OK)
            {
                ssSetErrorStatus(S,err.message);
                return;
            }
        }
            break;
    }
    ssPrintf("Got input name %s\n",helicsInputGetName(inp));
    ssSetUserData(S,inp);
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
    HelicsInput inp=ssGetUserData(S);

    DTypeId  outputDataType = ssGetOutputPortDataType(S, 0);
    int width=ssGetOutputPortWidth(S,0);

    switch(outputDataType){
        case SS_UINT8:
        case SS_INT8:
            break;
        case SS_UINT16:
        case SS_UINT32:
        case SS_INT16:
        case SS_INT32:
            break;
        case SS_DOUBLE:
        case SS_SINGLE:
            {
                if (helicsGetDataType(helicsInputGetType(inp))==HELICS_DATA_TYPE_COMPLEX)
                {

                }
                else if (width>1)
                {
                    ssPrintf("input input width =%d\n",width);
                    real_T *y = ssGetOutputPortRealSignal(S,0);
                    int sz;
                    HelicsError err=helicsErrorInitialize();
                    helicsInputGetVector(inp,y,width,&sz,&err);
                    ssPrintf("got input size %d input[0]=%f",sz,y[0]);
                    if (err.error_code!=HELICS_OK)
                    {

                    }
                    else
                    {
                        if (sz<width)
                        {
                        int ii=0;
                        for (ii=sz;ii<width;++ii)
                        {
                            y[ii]=HELICS_INVALID_PROPERTY_VALUE;
                        }
                        }
                    }
                }
                else
                {
                    ssPrintf("input input width =%d\n",width);
                    double val=helicsInputGetDouble(inp,NULL);
                    real_T *y = ssGetOutputPortRealSignal(S,0);
                    y[0]=val;
                }

            }
            break;
        case SS_BOOLEAN:
            break;
    }
}

#define MDL_UPDATE
void mdlUpdate(SimStruct *S, int_T tid)
{

}

static void mdlTerminate(SimStruct *S)
{


}


#include "simulink.c" /* MEX-file interface mechanism */
