/*
 * Copyright (c) 2024-2024, Lawrence Livermore National Security
 * See the top-level NOTICE for additional details. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define S_FUNCTION_NAME helics_publication /* Defines and Includes */
#define S_FUNCTION_LEVEL 2



#include "simstruc.h"
#include "helics/helics.h"
#include "math.h"

static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 2);
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return; /* Parameter mismatch reported by the Simulink engine*/
    }

    if (!ssSetNumInputPorts(S, 2)) return;
    ssSetInputPortWidth(S,0,3);

    ssSetInputPortDataType(S, 0, SS_DOUBLE );

    const mxArray *params=ssGetSFcnParam(S, 1);
    char *param_string=mxArrayToString(params);
    ssPrintf("pub parameter string=%s\n",param_string);

    int input_width=1;
    int dataType=HELICS_DATA_TYPE_DOUBLE;

    char *token=NULL;
    token=strtok(param_string," ,");
    while (token){
        ssPrintf("got token %s\n",token);
        if (strncmp(token,"input_size=",11)==0)
        {
            input_width=atoi(token+11);
            ssPrintf("got token input_size read as %s converted to %d\n",token+11,input_width);
        }
        else if (strncmp(token,"input_type=",11)==0)
        {
            dataType=helicsGetDataType(token+11);
            ssPrintf("got token input_type read as %s converted to %d\n",token+11,dataType);

        }
        token=strtok(NULL," ,");
    }
    ssSetInputPortWidth(S, 1, input_width);
    switch (dataType)
    {
        case HELICS_DATA_TYPE_DOUBLE: case HELICS_DATA_TYPE_VECTOR:
            ssSetInputPortDataType(S, 1, SS_DOUBLE);
            break;
        case HELICS_DATA_TYPE_INT:
            ssSetInputPortDataType(S, 1, SS_INT32);
            break;
        case HELICS_DATA_TYPE_BOOLEAN:
            ssSetInputPortDataType(S, 1, SS_BOOLEAN);
            break;
        case HELICS_DATA_TYPE_COMPLEX: case HELICS_DATA_TYPE_COMPLEX_VECTOR:
            ssSetInputPortDataType(S, 1, SS_DOUBLE);
            //something with complex indications
            break;
        case HELICS_DATA_TYPE_STRING:
           ssSetInputPortDataType(S,1,SS_UINT8);
           break;

    }
    ssSetInputPortRequiredContiguous(S,0,1);
    ssSetInputPortRequiredContiguous(S,1,1);
    if (!ssSetNumOutputPorts(S,0)) return;

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
        ssPrintf("Helics federate for publication is not valid\n");
        ssSetErrorStatus(S,"HELICS federate is not valid");
        return;
     }


     const mxArray *index_param=ssGetSFcnParam(S, 0);
     mxClassID paramType=mxGetClassID(index_param);
     HelicsPublication pub;
    switch (paramType)
    {
        case mxCHAR_CLASS:
        {
            char *pub_name=mxArrayToString(index_param);
            HelicsError err=helicsErrorInitialize();
            pub=helicsFederateGetPublication(fed,pub_name,&err);
            if (err.error_code!=HELICS_OK)
            {
                ssSetErrorStatus(S,err.message);
                return;
            }}
            break;
        case mxDOUBLE_CLASS:
        {
            double pub_index=mxGetScalar(index_param);
            HelicsError err=helicsErrorInitialize();
            pub=helicsFederateGetPublicationByIndex(fed,(int32_t)(pub_index),&err);
            if (err.error_code!=HELICS_OK)
            {
                ssSetErrorStatus(S,err.message);
                return;
            }
        }
        break;
    }
    ssPrintf("Got publication name %s\n",helicsPublicationGetName(pub));
    ssSetUserData(S,pub);

}

static void mdlOutputs(SimStruct *S, int_T tid)
{

}

#define MDL_UPDATE
void mdlUpdate(SimStruct *S, int_T tid)
{
    HelicsPublication pub=ssGetUserData(S);

    const double *tsig=ssGetInputPortRealSignal(S,0);

    DTypeId  inputDataType = ssGetInputPortDataType(S, 1);
    switch(inputDataType){
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
                const double *sig = ssGetInputPortRealSignal(S,1);
                ssPrintf("publishing double %f\n",sig[0]-1.0);

                helicsPublicationPublishDouble(pub,sig[0]-1.0,NULL);
                ssPrintf("published double\n");
            }
            break;
        case SS_BOOLEAN:
            break;
    }
}

static void mdlTerminate(SimStruct *S)
{


}


#include "simulink.c" /* MEX-file interface mechanism */
