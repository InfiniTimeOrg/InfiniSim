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


/**
 * @addtogroup OSKernel
 * @{
 *   @defgroup OSMbuf Chained Memory Buffers
 *   @{
 */


#ifndef _OS_MBUF_H
#define _OS_MBUF_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * A packet header structure that preceeds the mbuf packet headers.
 */
struct os_mbuf_pkthdr {
    /**
     * Overall length of the packet.
     */
    uint16_t omp_len;
    /**
     * Flags
     */
    uint16_t omp_flags;

    // sim: disabled
    // STAILQ_ENTRY(os_mbuf_pkthdr) omp_next;
};

/**
 * Chained memory buffer.
 */
struct os_mbuf {
    /**
     * Current pointer to data in the structure
     */
    uint8_t *om_data;
    /**
     * Flags associated with this buffer, see OS_MBUF_F_* defintions
     */
    uint8_t om_flags;
    /**
     * Length of packet header
     */
    uint8_t om_pkthdr_len;
    /**
     * Length of data in this buffer
     */
    uint16_t om_len;

    // sim: disabled, not used by InfiniSim
    ///**
    // * The mbuf pool this mbuf was allocated out of
    // */
    //struct os_mbuf_pool *om_omp;

    //SLIST_ENTRY(os_mbuf) om_next;

    /**
     * Pointer to the beginning of the data, after this buffer
     */
    uint8_t om_databuf[0];
};


/** Get a packet header pointer given an mbuf pointer */
#define OS_MBUF_PKTHDR(__om) ((struct os_mbuf_pkthdr *)     \
    ((uint8_t *)&(__om)->om_data + sizeof(struct os_mbuf)))

/**
 * Gets the length of an entire mbuf chain.  The specified mbuf must have a
 * packet header.
 */
#define OS_MBUF_PKTLEN(__om) (OS_MBUF_PKTHDR(__om)->omp_len)

// sim: function from os_mbuf.h
/*
 * Copy data from an mbuf chain starting "off" bytes from the beginning,
 * continuing for "len" bytes, into the indicated buffer.
 *
 * @param m The mbuf chain to copy from
 * @param off The offset into the mbuf chain to begin copying from
 * @param len The length of the data to copy
 * @param dst The destination buffer to copy into
 *
 * @return                      0 on success;
 *                              -1 if the mbuf does not contain enough data.
 */
int os_mbuf_copydata(const struct os_mbuf *m, int off, int len, void *dst);


/**
 * Append data onto a mbuf
 *
 * @param om   The mbuf to append the data onto
 * @param data The data to append onto the mbuf
 * @param len  The length of the data to append
 *
 * @return 0 on success, and an error code on failure
 */
int os_mbuf_append(struct os_mbuf *m, const void *, uint16_t);


#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
