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

#include "host/ble_uuid.h"

#include <string.h>

// sim: real comparison (the original stub returned 0 for every pair, which
// breaks services that dispatch on the characteristic UUID).
int
ble_uuid_cmp(const ble_uuid_t *uuid1, const ble_uuid_t *uuid2)
{
    if (uuid1->type != uuid2->type) {
        return (int) uuid1->type - (int) uuid2->type;
    }
    switch (uuid1->type) {
    case BLE_UUID_TYPE_16: {
        const ble_uuid16_t *a = (const ble_uuid16_t *) uuid1;
        const ble_uuid16_t *b = (const ble_uuid16_t *) uuid2;
        return (int) a->value - (int) b->value;
    }
    case BLE_UUID_TYPE_32: {
        const ble_uuid32_t *a = (const ble_uuid32_t *) uuid1;
        const ble_uuid32_t *b = (const ble_uuid32_t *) uuid2;
        return a->value == b->value ? 0 : (a->value < b->value ? -1 : 1);
    }
    case BLE_UUID_TYPE_128: {
        const ble_uuid128_t *a = (const ble_uuid128_t *) uuid1;
        const ble_uuid128_t *b = (const ble_uuid128_t *) uuid2;
        return memcmp(a->value, b->value, sizeof(a->value));
    }
    default:
        return -1;
    }
}
