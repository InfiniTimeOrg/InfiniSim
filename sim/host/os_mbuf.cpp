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

#include "host/os_mbuf.h"

#include <string.h>

// sim: real single-buffer implementations (the original stubs were no-ops,
// which silently broke any service using the proper NimBLE parsing idioms).
// A fake mbuf is one flat buffer: om_data points at the data, om_len is its
// length. There is no chaining.

int
os_mbuf_copydata(const struct os_mbuf *m, int off, int len, void *dst)
{
    if (m == NULL || m->om_data == NULL || off < 0 || len < 0 ||
        off + len > m->om_len) {
        return -1;
    }
    memcpy(dst, m->om_data + off, (size_t) len);
    return 0;
}

// Appends into the buffer om_data points at. For sim fake mbufs om_pkthdr_len
// carries the destination capacity (bytes) so an over-long append is rejected
// instead of overrunning the creator's stack buffer; a capacity of 0 means the
// creator vouches for the buffer (e.g. write buffers, which are never
// appended). Firmware code only checks the return value, exactly as it does
// against the real NimBLE mbuf-pool exhaustion.
int
os_mbuf_append(struct os_mbuf *om, const void *data,  uint16_t len)
{
    if (om == NULL || om->om_data == NULL) {
        return -1;
    }
    if (om->om_pkthdr_len != 0 && (uint32_t) om->om_len + len > om->om_pkthdr_len) {
        return -1; // would overrun the response buffer
    }
    memcpy(om->om_data + om->om_len, data, len);
    om->om_len += len;
    return 0;
}
