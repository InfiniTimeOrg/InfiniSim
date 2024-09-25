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

#include "host/ble_gatt.h"

int
ble_gatts_count_cfg(const struct ble_gatt_svc_def *defs)
{
    //struct ble_gatt_resources res = { 0 };
    //int rc;

    //rc = ble_gatts_count_resources(defs, &res);
    //if (rc != 0) {
    //    return rc;
    //}

    //ble_hs_max_services += res.svcs;
    //ble_hs_max_attrs += res.attrs;

    ///* Reserve an extra CCCD for the cache. */
    //ble_hs_max_client_configs +=
    //    res.cccds * (MYNEWT_VAL(BLE_MAX_CONNECTIONS) + 1);

    return 0;
}

int
ble_gatts_add_svcs(const struct ble_gatt_svc_def *svcs)
{
//    void *p;
//    int rc;
//
//    ble_hs_lock();
//    if (!ble_gatts_mutable()) {
//        rc = BLE_HS_EBUSY;
//        goto done;
//    }
//
//    p = realloc(ble_gatts_svc_defs,
//                (ble_gatts_num_svc_defs + 1) * sizeof *ble_gatts_svc_defs);
//    if (p == NULL) {
//        rc = BLE_HS_ENOMEM;
//        goto done;
//    }
//
//    ble_gatts_svc_defs = p;
//    ble_gatts_svc_defs[ble_gatts_num_svc_defs] = svcs;
//    ble_gatts_num_svc_defs++;
//
//    rc = 0;
//
//done:
//    ble_hs_unlock();
//    return rc;
    return 0;
}
