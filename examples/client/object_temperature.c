/*******************************************************************************
 * 
 * Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Scott Bertin, AMETEK, Inc. - Please refer to git log
 *    
 ******************************************************************************/
/*! \file
  LWM2M object "temperature" implementation

  \author Carlos Wei
*/

/*
 *  Object     |      | Multiple  |     | Description                   |
 *  Name       |  ID  | Instances |Mand.|                               |
 *-------------+------+-----------+-----+-------------------------------+
 *  temperature |  173 |    No     |  No |  temperature object            |
 *
 *  Resources:
 *  Name        | ID  | Oper.|Instances|Mand.|  Type   | Range | Units | Description                                                                      |
 * -------------+-----+------+---------+-----+---------+-------+-------+----------------------------------------------------------------------------------+
 *  Current     |  0  |  R   | Single  | Yes | Float   |       |  du   | The decimal notation of current  e.g. -  45.5723  [Worlds Geodetic System 1984]. |
 *  Max         |  1  |  R   | Single  | Yes | Float   |       |  du   | The decimal notation of Max e.g. - 153.21760 [Worlds Geodetic System 1984].      |
 *  Min         |  2  |  R   | Single  | Yes | Float   |       |  du   | The decimal notation of Min                                                      |
 *  Timestamp   |  5  |  R   | Single  | Yes | Time    |       |   s   | The timestamp when the location measurement was performed.                       |
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef LWM2M_CLIENT_MODE
#define LWM2M_CLIENT_MODE
#endif
#ifdef LWM2M_CLIENT_MODE

// ---- private "object location" specific defines ----
// Resource Id's:
#define RES_M_CURRENT      0
#define RES_M_MAX          1
#define RES_M_MIN          2
#define RES_M_TIMESTAMP    5

typedef struct
{
    double    current;
    double    max;
    double    min;
    unsigned long timestamp;

} temperature_data_t;

/**
implementation for all read-able resources
*/
static uint8_t prv_res2tlv(lwm2m_data_t* dataP,
                           temperature_data_t* tempDataP)
{
    //-------------------------------------------------------------------- JH --
    uint8_t ret = COAP_205_CONTENT;
    switch (dataP->id)     // temperature resourceId
    {
    case RES_M_CURRENT:
        lwm2m_data_encode_float(tempDataP->current, dataP);
        break;
    case RES_M_MAX:
        lwm2m_data_encode_float(tempDataP->max, dataP);
        printf("res2tlv max : %f\n", tempDataP->max);
        break;
    case RES_M_MIN:
        lwm2m_data_encode_float(tempDataP->min, dataP);
        break;
    case RES_M_TIMESTAMP:
        lwm2m_data_encode_int(tempDataP->timestamp, dataP);
        break;
    default:
        ret = COAP_404_NOT_FOUND;
        break;
    }

    return ret;
}

/**
  * Implementation (callback-) function of reading object resources. For whole
  * object, single resources or a sequence of resources
  * see 3GPP TS 23.032 V11.0.0(2012-09) page 23,24.
  * implemented for: HORIZONTAL_VELOCITY_WITH_UNCERTAINT
  * @param objInstId    in,     instances ID of the location object to read
  * @param numDataP     in/out, pointer to the number of resource to read. 0 is the
  *                             exception for all readable resource of object instance
  * @param tlvArrayP    in/out, TLV data sequence with initialized resource ID to read
  * @param objectP      in,     private temperature data structure
  */
static uint8_t prv_temperature_read(uint16_t objInstId,
                                 int*  numDataP,
                                 lwm2m_data_t** tlvArrayP,
                                 lwm2m_object_t*  objectP)
{
    //-------------------------------------------------------------------- JH --
    int     i;
    uint8_t result = COAP_500_INTERNAL_SERVER_ERROR;
    temperature_data_t* tempDataP = (temperature_data_t*)(objectP->userData);

    // defined as single instance object!
    if (objInstId != 0) return COAP_404_NOT_FOUND;

    if (*numDataP == 0)     // full object, readable resources!
    {
        uint16_t readResIds[] = {
                RES_M_CURRENT,
                RES_M_MAX,
                RES_M_MIN,
                RES_M_TIMESTAMP
        }; // readable resources!

        *numDataP  = sizeof(readResIds)/sizeof(uint16_t);
        *tlvArrayP = lwm2m_data_new(*numDataP);
        if (*tlvArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;

        // init readable resource id's
        for (i = 0 ; i < *numDataP ; i++)
        {
            (*tlvArrayP)[i].id = readResIds[i];
        }
    }

    for (i = 0 ; i < *numDataP ; i++)
    {
        if ((*tlvArrayP)[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
        }
        else
        {
            result = prv_res2tlv ((*tlvArrayP)+i, tempDataP);
        }
        if (result!=COAP_205_CONTENT) break;
    }

    return result;
}

static uint8_t prv_temperature_discover(uint16_t instanceId, 
                                       int * numDataP, 
                                       lwm2m_data_t ** dataArrayP, 
                                       lwm2m_object_t * objectP)
{
    uint8_t result;
    int i;
    printf("prv_temperature discover function\n");
    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            RES_M_CURRENT,
            RES_M_MAX,
            RES_M_MIN,
            RES_M_TIMESTAMP
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
                case RES_M_CURRENT:
                case RES_M_MAX:
                case RES_M_MIN:
                case RES_M_TIMESTAMP:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_device_execute(uint16_t instanceId,
                                  uint16_t resourceId,
                                  uint8_t * buffer,
                                  int length,
                                  lwm2m_object_t * objectP)
{
    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) return COAP_400_BAD_REQUEST;

    switch (resourceId)
    {
    case 0:
        fprintf(stdout, "\n\t RESET ERROR CODE\r\n\n");
        return COAP_204_CHANGED;
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}
#if 0
static void prv_temperature_change(char * buffer,
                                   void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
    int result;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_string_to_uri(buffer, end - buffer, &uri);
    if (result == 0) goto syntax_error;

    buffer = get_next_arg(end, &end);

    if (buffer[0] == 0)
    {
        printf("report change!\n");
        lwm2m_resource_value_changed(lwm2mH, &uri);
    }
    else
    {
        handle_value_changed(lwm2mH, &uri, buffer, end - buffer);
    }
    return;

syntax_error:
    printf("Syntax error !\n");
}
#endif
static uint8_t prv_temperature_write(uint16_t instanceId,
                                    int numData,
                                    lwm2m_data_t * dataArray,
                                    lwm2m_object_t * objectP,
                                    lwm2m_write_type_t writeType)
{
    int i;
    uint8_t result;
    temperature_data_t *targetP = NULL;
    lwm2m_context_t * lwm2mH = (lwm2m_context_t*)(objectP->userData);

    printf("prv_temperature_write function\n");
    // All write types are treated the same here
    (void)writeType;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }
    targetP = (temperature_data_t*)lwm2m_list_find(objectP->instanceList, instanceId);
    if (targetP == NULL) {
        return COAP_404_NOT_FOUND;
    }
    i = 0;

    do
    {
        /* No multiple instance resources */
        if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
            continue;
        }

        switch (dataArray[i].id)
        {
        case RES_M_CURRENT:
            printf("asBuffer %s\n", (dataArray + i)->value.asBuffer.buffer);
            if ( (dataArray + i)->value.asBuffer.length > 7) {
                result = COAP_400_BAD_REQUEST;
                return result;
            }

            if (1 == lwm2m_data_decode_float( dataArray + i, &(targetP->current)))
            {
                ((temperature_data_t*)(objectP->userData))->current = targetP->current;
                printf("client change the value : %f\n", targetP->current);
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        case RES_M_MAX:
            printf("asBuffer %s\n", (dataArray + i)->value.asBuffer.buffer);
            if (1 == lwm2m_data_decode_float(dataArray + i, &(targetP->max)))
            {
                ((temperature_data_t*)(objectP->userData))->max = targetP->max;
                printf("client change the max value : %f\n", targetP->max);
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        case RES_M_MIN:
            printf("asBuffer %s\n", (dataArray + i)->value.asBuffer.buffer);
            if (1 == lwm2m_data_decode_float(dataArray + i, &(targetP->min)))
            {
                ((temperature_data_t*)(objectP->userData))->min = targetP->min;
                printf("client change the min value : %f\n", targetP->min);
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;
        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

void display_temperature_object(lwm2m_object_t * object)
{
    temperature_data_t * data = (temperature_data_t *)object->userData;
    fprintf(stdout, "  /%u: temperature object:\r\n", object->objID);
    if (NULL != data)
    {
        fprintf(stdout, "    current: %.6f, max: %.6f, min: %.6f, timestamp: %lu\r\n",
                data->current, data->max, data->min, data->timestamp);
    }
}

/**
  * This function creates the LWM2M Location. 
  * @return gives back allocated LWM2M data object structure pointer. On error, 
  * NULL value is returned.
  */
lwm2m_object_t * get_object_temperature(void)
{
    //-------------------------------------------------------------------- JH --
    lwm2m_object_t * tempObj;

    tempObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    if (NULL != tempObj)
    {
        memset(tempObj, 0, sizeof(lwm2m_object_t));

        // It assigns its unique ID
        // The 25 is the standard ID for the optional object "temp".
        tempObj->objID = LWM2M_TEMPERATURE_OBJECT_ID;

        // and its unique instance
        tempObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        if (NULL != tempObj->instanceList)
        {
            memset(tempObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(tempObj);
            return NULL;
        }

        // And the private function that will access the object.
        // Those function will be called when a read query is made by the server.
        // In fact the library don't need to know the resources of the object, only the server does.
        //
        tempObj->readFunc    = prv_temperature_read;
        tempObj->writeFunc   = prv_temperature_write;
        tempObj->discoverFunc= prv_temperature_discover;
        tempObj->userData    = lwm2m_malloc(sizeof(temperature_data_t));

        // initialize private data structure containing the needed variables
        if (NULL != tempObj->userData)
        {
            temperature_data_t* data = (temperature_data_t*)tempObj->userData;
            data->current    = 27.986065;  // Mount Everest :)
            data->max        = 100.922623;
            data->min        = -20.855;
            data->timestamp   = time(NULL);
        }
        else
        {
            lwm2m_free(tempObj);
            tempObj = NULL;
        }
    }

    return tempObj;
}

void free_object_temperature(lwm2m_object_t * object)
{
    lwm2m_list_free(object->instanceList);
    lwm2m_free(object->userData);
    lwm2m_free(object);
}

#endif  //LWM2M_CLIENT_MODE
