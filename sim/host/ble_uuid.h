/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_BLE_UUID_
#define H_BLE_UUID_

/**
 * @brief Bluetooth UUID
 * @defgroup bt_uuid Bluetooth UUID
 * @ingroup bt_host
 * @{
 */

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct os_mbuf;

/** Type of UUID */
enum {
    /** 16-bit UUID (BT SIG assigned) */
    BLE_UUID_TYPE_16 = 16,

    /** 32-bit UUID (BT SIG assigned) */
    BLE_UUID_TYPE_32 = 32,

    /** 128-bit UUID */
    BLE_UUID_TYPE_128 = 128,
};

/** Generic UUID type, to be used only as a pointer */
typedef struct {
    /** Type of the UUID */
    uint8_t type;
} ble_uuid_t;

/** 16-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint16_t value;
} ble_uuid16_t;

/** 32-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint32_t value;
} ble_uuid32_t;

/** 128-bit UUID */
typedef struct {
    ble_uuid_t u;
    uint8_t value[16];
} ble_uuid128_t;

/** Universal UUID type, to be used for any-UUID static allocation */
typedef union {
    ble_uuid_t u;
    ble_uuid16_t u16;
    ble_uuid32_t u32;
    ble_uuid128_t u128;
} ble_uuid_any_t;

/** @brief Compares two Bluetooth UUIDs.
 *
 * @param uuid1  The first UUID to compare.
 * @param uuid2  The second UUID to compare.
 *
 * @return       0 if the two UUIDs are equal, nonzero if the UUIDs differ.
 */
int ble_uuid_cmp(const ble_uuid_t *uuid1, const ble_uuid_t *uuid2);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _BLE_HOST_UUID_H */
