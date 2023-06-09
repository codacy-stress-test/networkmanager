/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2013 - 2015 Red Hat, Inc.
 */

#ifndef __NM_VPN_HELPERS_H__
#define __NM_VPN_HELPERS_H__

typedef struct {
    const char *name;
    const char *ui_name;
} NmcVpnPasswordName;

GSList *nm_vpn_get_plugin_infos(void);

NMVpnEditorPlugin *nm_vpn_get_editor_plugin(const char *service_type, GError **error);

gboolean nm_vpn_supports_ipv6(NMConnection *connection);

const NmcVpnPasswordName *nm_vpn_get_secret_names(const char *service_type);

gboolean
nm_vpn_openconnect_authenticate_helper(NMSettingVpn *s_vpn, GPtrArray *secrets, GError **error);

#endif /* __NM_VPN_HELPERS_H__ */
