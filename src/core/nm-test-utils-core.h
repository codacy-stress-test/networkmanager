/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2014 - 2016 Red Hat, Inc.
 */

#ifndef __NM_TEST_UTILS_CORE_H__
#define __NM_TEST_UTILS_CORE_H__

#include "NetworkManagerUtils.h"
#include "libnm-core-intern/nm-keyfile-internal.h"

#define _NMTST_INSIDE_CORE 1

#include "libnm-glib-aux/nm-test-utils.h"

/*****************************************************************************/

#ifdef __NETWORKMANAGER_PLATFORM_H__

static inline NMPlatformIP4Address *
nmtst_platform_ip4_address(const char *address, const char *peer_address, guint plen)
{
    static NMPlatformIP4Address addr;

    g_assert(plen <= 32);

    memset(&addr, 0, sizeof(addr));
    addr.address = nmtst_inet4_from_string(address);
    if (peer_address)
        addr.peer_address = nmtst_inet4_from_string(peer_address);
    else
        addr.peer_address = addr.address;
    addr.plen = plen;

    return &addr;
}

static inline NMPlatformIP4Address *
nmtst_platform_ip4_address_full(const char      *address,
                                const char      *peer_address,
                                guint            plen,
                                int              ifindex,
                                NMIPConfigSource source,
                                guint32          timestamp,
                                guint32          lifetime,
                                guint32          preferred,
                                guint32          flags,
                                const char      *label)
{
    NMPlatformIP4Address *addr = nmtst_platform_ip4_address(address, peer_address, plen);

    G_STATIC_ASSERT(NM_IFNAMSIZ == sizeof(addr->label));
    g_assert(!label || strlen(label) < NM_IFNAMSIZ);

    addr->ifindex     = ifindex;
    addr->addr_source = source;
    addr->timestamp   = timestamp;
    addr->lifetime    = lifetime;
    addr->preferred   = preferred;
    addr->n_ifa_flags = flags;
    if (label)
        g_strlcpy(addr->label, label, sizeof(addr->label));

    return addr;
}

static inline NMPlatformIP6Address *
nmtst_platform_ip6_address(const char *address, const char *peer_address, guint plen)
{
    static NMPlatformIP6Address addr;

    g_assert(plen <= 128);

    memset(&addr, 0, sizeof(addr));
    addr.address      = nmtst_inet6_from_string(address);
    addr.peer_address = nmtst_inet6_from_string(peer_address);
    addr.plen         = plen;

    return &addr;
}

static inline NMPlatformIP6Address *
nmtst_platform_ip6_address_full(const char      *address,
                                const char      *peer_address,
                                guint            plen,
                                int              ifindex,
                                NMIPConfigSource source,
                                guint32          timestamp,
                                guint32          lifetime,
                                guint32          preferred,
                                guint32          flags)
{
    NMPlatformIP6Address *addr = nmtst_platform_ip6_address(address, peer_address, plen);

    addr->ifindex     = ifindex;
    addr->addr_source = source;
    addr->timestamp   = timestamp;
    addr->lifetime    = lifetime;
    addr->preferred   = preferred;
    addr->n_ifa_flags = flags;

    return addr;
}

static inline NMPlatformIP4Route *
nmtst_platform_ip4_route(const char *network, guint plen, const char *gateway)
{
    static NMPlatformIP4Route route;

    g_assert(plen <= 32);

    memset(&route, 0, sizeof(route));
    route.network = nmtst_inet4_from_string(network);
    route.plen    = plen;
    route.gateway = nmtst_inet4_from_string(gateway);

    return &route;
}

static inline NMPlatformIP4Route *
nmtst_platform_ip4_route_full(const char      *network,
                              guint            plen,
                              const char      *gateway,
                              int              ifindex,
                              NMIPConfigSource source,
                              guint            metric,
                              guint            mss,
                              guint8           scope,
                              const char      *pref_src)
{
    NMPlatformIP4Route *route = nmtst_platform_ip4_route(network, plen, gateway);

    route->ifindex   = ifindex;
    route->rt_source = source;
    route->metric    = metric;
    route->mss       = mss;
    route->scope_inv = nm_platform_route_scope_inv(scope);
    route->pref_src  = nmtst_inet4_from_string(pref_src);

    return route;
}

static inline NMPlatformIP6Route *
nmtst_platform_ip6_route(const char *network, guint plen, const char *gateway, const char *pref_src)
{
    static NMPlatformIP6Route route;

    nm_assert(plen <= 128);

    memset(&route, 0, sizeof(route));
    route.network  = nmtst_inet6_from_string(network);
    route.plen     = plen;
    route.gateway  = nmtst_inet6_from_string(gateway);
    route.pref_src = nmtst_inet6_from_string(pref_src);

    return &route;
}

static inline NMPlatformIP6Route *
nmtst_platform_ip6_route_full(const char      *network,
                              guint            plen,
                              const char      *gateway,
                              int              ifindex,
                              NMIPConfigSource source,
                              guint            metric,
                              guint            mss)
{
    NMPlatformIP6Route *route = nmtst_platform_ip6_route(network, plen, gateway, NULL);

    route->ifindex   = ifindex;
    route->rt_source = source;
    route->metric    = metric;
    route->mss       = mss;

    return route;
}

static inline int
_nmtst_platform_ip4_routes_equal_sort(gconstpointer a, gconstpointer b, gpointer user_data)
{
    return nm_platform_ip4_route_cmp((const NMPlatformIP4Route *) a,
                                     (const NMPlatformIP4Route *) b,
                                     NM_PLATFORM_IP_ROUTE_CMP_TYPE_FULL);
}

static inline void
nmtst_platform_ip4_routes_equal(const NMPlatformIP4Route *a,
                                const NMPlatformIP4Route *b,
                                gsize                     len,
                                gboolean                  ignore_order)
{
    gsize                             i;
    gs_free const NMPlatformIP4Route *c_a = NULL, *c_b = NULL;

    g_assert(a);
    g_assert(b);

    if (ignore_order) {
        a = c_a = nm_memdup(a, sizeof(NMPlatformIP4Route) * len);
        b = c_b = nm_memdup(b, sizeof(NMPlatformIP4Route) * len);
        g_qsort_with_data(c_a,
                          len,
                          sizeof(NMPlatformIP4Route),
                          _nmtst_platform_ip4_routes_equal_sort,
                          NULL);
        g_qsort_with_data(c_b,
                          len,
                          sizeof(NMPlatformIP4Route),
                          _nmtst_platform_ip4_routes_equal_sort,
                          NULL);
    }

    for (i = 0; i < len; i++) {
        if (nm_platform_ip4_route_cmp(&a[i], &b[i], NM_PLATFORM_IP_ROUTE_CMP_TYPE_FULL) != 0) {
            char buf1[NM_UTILS_TO_STRING_BUFFER_SIZE];
            char buf2[NM_UTILS_TO_STRING_BUFFER_SIZE];

            g_error("Error comparing IPv4 route[%lu]: %s vs %s",
                    (unsigned long) i,
                    nm_platform_ip4_route_to_string(&a[i], buf1, sizeof(buf1)),
                    nm_platform_ip4_route_to_string(&b[i], buf2, sizeof(buf2)));
            g_assert_not_reached();
        }
    }
}

#ifdef __NMP_OBJECT_H__

static inline void
nmtst_platform_ip4_routes_equal_aptr(const NMPObject *const   *a,
                                     const NMPlatformIP4Route *b,
                                     gsize                     len,
                                     gboolean                  ignore_order)
{
    gsize                       i;
    gs_free NMPlatformIP4Route *c_a = NULL;

    g_assert(len > 0);
    g_assert(a);

    c_a = g_new(NMPlatformIP4Route, len);
    for (i = 0; i < len; i++)
        c_a[i] = *NMP_OBJECT_CAST_IP4_ROUTE(a[i]);
    nmtst_platform_ip4_routes_equal(c_a, b, len, ignore_order);
}

#endif

static inline int
_nmtst_platform_ip6_routes_equal_sort(gconstpointer a, gconstpointer b, gpointer user_data)
{
    return nm_platform_ip6_route_cmp((const NMPlatformIP6Route *) a,
                                     (const NMPlatformIP6Route *) b,
                                     NM_PLATFORM_IP_ROUTE_CMP_TYPE_FULL);
}

static inline void
nmtst_platform_ip6_routes_equal(const NMPlatformIP6Route *a,
                                const NMPlatformIP6Route *b,
                                gsize                     len,
                                gboolean                  ignore_order)
{
    gsize                             i;
    gs_free const NMPlatformIP6Route *c_a = NULL, *c_b = NULL;

    g_assert(a);
    g_assert(b);

    if (ignore_order) {
        a = c_a = nm_memdup(a, sizeof(NMPlatformIP6Route) * len);
        b = c_b = nm_memdup(b, sizeof(NMPlatformIP6Route) * len);
        g_qsort_with_data(c_a,
                          len,
                          sizeof(NMPlatformIP6Route),
                          _nmtst_platform_ip6_routes_equal_sort,
                          NULL);
        g_qsort_with_data(c_b,
                          len,
                          sizeof(NMPlatformIP6Route),
                          _nmtst_platform_ip6_routes_equal_sort,
                          NULL);
    }

    for (i = 0; i < len; i++) {
        if (nm_platform_ip6_route_cmp(&a[i], &b[i], NM_PLATFORM_IP_ROUTE_CMP_TYPE_FULL) != 0) {
            char buf1[NM_UTILS_TO_STRING_BUFFER_SIZE];
            char buf2[NM_UTILS_TO_STRING_BUFFER_SIZE];

            g_error("Error comparing IPv6 route[%lu]: %s vs %s",
                    (unsigned long) i,
                    nm_platform_ip6_route_to_string(&a[i], buf1, sizeof(buf1)),
                    nm_platform_ip6_route_to_string(&b[i], buf2, sizeof(buf2)));
            g_assert_not_reached();
        }
    }
}

#ifdef __NMP_OBJECT_H__

static inline void
nmtst_platform_ip6_routes_equal_aptr(const NMPObject *const   *a,
                                     const NMPlatformIP6Route *b,
                                     gsize                     len,
                                     gboolean                  ignore_order)
{
    gsize                       i;
    gs_free NMPlatformIP6Route *c_a = NULL;

    g_assert(len > 0);
    g_assert(a);

    c_a = g_new(NMPlatformIP6Route, len);
    for (i = 0; i < len; i++)
        c_a[i] = *NMP_OBJECT_CAST_IP6_ROUTE(a[i]);
    nmtst_platform_ip6_routes_equal(c_a, b, len, ignore_order);
}

#endif

#endif

#ifdef __NETWORKMANAGER_IP4_CONFIG_H__

#include "libnm-glib-aux/nm-dedup-multi.h"

static inline NMIP4Config *
nmtst_ip4_config_new(int ifindex)
{
    nm_auto_unref_dedup_multi_index NMDedupMultiIndex *multi_idx = nm_dedup_multi_index_new();

    return nm_ip4_config_new(multi_idx, ifindex);
}

#endif

#ifdef __NETWORKMANAGER_IP6_CONFIG_H__

#include "libnm-glib-aux/nm-dedup-multi.h"

static inline NMIP6Config *
nmtst_ip6_config_new(int ifindex)
{
    nm_auto_unref_dedup_multi_index NMDedupMultiIndex *multi_idx = nm_dedup_multi_index_new();

    return nm_ip6_config_new(multi_idx, ifindex);
}

#endif

#endif /* __NM_TEST_UTILS_CORE_H__ */
