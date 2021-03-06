/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef __STA_INFO_H_
#define __STA_INFO_H_


#define IBSS_START_MAC_ID	2
#define NUM_STA 32
#define NUM_ACL 16


//if mode ==0, then the sta is allowed once the addr is hit.
//if mode ==1, then the sta is rejected once the addr is non-hit.
struct rtw_wlan_acl_node {
        struct list_head		        list;
        uint8_t       addr[ETH_ALEN];
        uint8_t       valid;
};

//mode=0, disable
//mode=1, accept unless in deny list
//mode=2, deny unless in accept list
struct wlan_acl_pool {
	int mode;
	int num;
	struct rtw_wlan_acl_node aclnode[NUM_ACL];
	struct __queue	acl_node_q;
};

typedef struct _RSSI_STA{
	int32_t	UndecoratedSmoothedPWDB;
	int32_t	UndecoratedSmoothedCCK;
	int32_t	UndecoratedSmoothedOFDM;
	u64	PacketMap;
	uint8_t	ValidBit;
}RSSI_STA, *PRSSI_STA;

struct	stainfo_stats	{

	u64 rx_mgnt_pkts;
		u64 rx_beacon_pkts;
		u64 rx_probereq_pkts;
		u64 rx_probersp_pkts;
		u64 rx_probersp_bm_pkts;
		u64 rx_probersp_uo_pkts;
	u64 rx_ctrl_pkts;
	u64 rx_data_pkts;

	u64	last_rx_mgnt_pkts;
		u64 last_rx_beacon_pkts;
		u64 last_rx_probereq_pkts;
		u64 last_rx_probersp_pkts;
		u64 last_rx_probersp_bm_pkts;
		u64 last_rx_probersp_uo_pkts;
	u64	last_rx_ctrl_pkts;
	u64	last_rx_data_pkts;

	u64	rx_bytes;
	u64	rx_drops;

	u64	tx_pkts;
	u64	tx_bytes;
	u64  tx_drops;

};


struct sta_info {

	spinlock_t	lock;
	struct list_head	list; //free_sta_queue
	struct list_head	hash_list; //sta_hash
	//struct list_head asoc_list; //20061114
	//struct list_head sleep_list;//sleep_q
	//struct list_head wakeup_list;//wakeup_q
	struct rtl_priv *rtlpriv;

	struct sta_xmit_priv sta_xmitpriv;
	struct sta_recv_priv sta_recvpriv;

	struct __queue sleep_q;
	unsigned int sleepq_len;

	uint state;
	uint aid;
	uint mac_id;
	uint qos_option;
	uint8_t	hwaddr[ETH_ALEN];

	uint	ieee8021x_blocked;	//0: allowed, 1:blocked
	uint	dot118021XPrivacy; //aes, tkip...
	union Keytype	dot11tkiptxmickey;
	union Keytype	dot11tkiprxmickey;
	union Keytype	dot118021x_UncstKey;
	union pn48		dot11txpn;			// PN48 used for Unicast xmit.
	union pn48		dot11rxpn;			// PN48 used for Unicast recv.


	uint8_t	bssrateset[16];
	u32	bssratelen;
	int32_t  rssi;
	int32_t	signal_quality;

	uint8_t	cts2self;
	uint8_t	rtsen;

	uint8_t	raid;
	uint8_t 	init_rate;
	u32	ra_mask;
	uint8_t	wireless_mode;	// NETWORK_TYPE

	struct stainfo_stats sta_stats;

	//for A-MPDU TX, ADDBA timeout check
	struct _timer_list addba_retry_timer;

	//for A-MPDU Rx reordering buffer control
	struct recv_reorder_ctrl recvreorder_ctrl[16];

	//for A-MPDU Tx
	//unsigned char		ampdu_txen_bitmap;
	u16	BA_starting_seqctrl[16];


	struct ht_priv	htpriv;

	struct vht_priv	vhtpriv;

	//Notes:
	//STA_Mode:
	//curr_network(mlme_priv/security_priv/qos/ht) + sta_info: (STA & AP) CAP/INFO
	//scan_q: AP CAP/INFO

	//AP_Mode:
	//curr_network(mlme_priv/security_priv/qos/ht) : AP CAP/INFO
	//sta_info: (AP & STA) CAP/INFO

#ifdef CONFIG_AP_MODE

	struct list_head asoc_list;
	struct list_head auth_list;

	unsigned int expire_to;
	unsigned int auth_seq;
	unsigned int authalg;
	unsigned char chg_txt[128];

	u16 capability;
	int flags;

	int dot8021xalg;//0:disable, 1:psk, 2:802.1x
	int wpa_psk;//0:disable, bit(0): WPA, bit(1):WPA2
	int wpa_group_cipher;
	int wpa2_group_cipher;
	int wpa_pairwise_cipher;
	int wpa2_pairwise_cipher;

	uint8_t bpairwise_key_installed;

	uint8_t wpa_ie[32];

	uint8_t nonerp_set;
	uint8_t no_short_slot_time_set;
	uint8_t no_short_preamble_set;
	uint8_t no_ht_gf_set;
	uint8_t no_ht_set;
	uint8_t ht_20mhz_set;

	unsigned int tx_ra_bitmap;
	uint8_t qos_info;

	uint8_t max_sp_len;
	uint8_t uapsd_bk;//BIT(0): Delivery enabled, BIT(1): Trigger enabled
	uint8_t uapsd_be;
	uint8_t uapsd_vi;
	uint8_t uapsd_vo;

	uint8_t has_legacy_ac;
	unsigned int sleepq_ac_len;

#ifdef CONFIG_TX_MCAST2UNI
	uint8_t under_exist_checking;
#endif	// CONFIG_TX_MCAST2UNI

	uint8_t keep_alive_trycnt;

#endif	// CONFIG_AP_MODE

	//for DM
	RSSI_STA	 rssi_stat;

	//
	// ================ODM Relative Info=======================
	// Please be care, dont declare too much structure here. It will cost memory * STA support num.
	//
	//
	// 2011/10/20 MH Add for ODM STA info.
	//
	// Driver Write
	uint8_t		bValid;				// record the sta status link or not?
	//uint8_t		WirelessMode;		//
	uint8_t		IOTPeer;			// Enum value.	HT_IOT_PEER_E
	uint8_t		rssi_level;			//for Refresh RA mask
	// ODM Write
	//1 PHY_STATUS_INFO
	uint8_t		RSSI_Path[4];		//
	uint8_t		RSSI_Ave;
	uint8_t		RXEVM[4];
	uint8_t		RXSNR[4];

	// ODM Write
	//1 TX_INFO (may changed by IC)
	//TX_INFO_T		pTxInfo;				// Define in IC folder. Move lower layer.
	//
	// ================ODM Relative Info=======================
	//

	/* To store the sequence number of received management frame */
	u16 RxMgmtFrameSeqNum;
};

#define sta_rx_pkts(sta) \
	(sta->sta_stats.rx_mgnt_pkts \
	+ sta->sta_stats.rx_ctrl_pkts \
	+ sta->sta_stats.rx_data_pkts)

#define sta_last_rx_pkts(sta) \
	(sta->sta_stats.last_rx_mgnt_pkts \
	+ sta->sta_stats.last_rx_ctrl_pkts \
	+ sta->sta_stats.last_rx_data_pkts)

#define sta_rx_data_pkts(sta) \
	(sta->sta_stats.rx_data_pkts)

#define sta_last_rx_data_pkts(sta) \
	(sta->sta_stats.last_rx_data_pkts)

#define sta_rx_mgnt_pkts(sta) \
	(sta->sta_stats.rx_mgnt_pkts)

#define sta_last_rx_mgnt_pkts(sta) \
	(sta->sta_stats.last_rx_mgnt_pkts)

#define sta_rx_beacon_pkts(sta) \
	(sta->sta_stats.rx_beacon_pkts)

#define sta_last_rx_beacon_pkts(sta) \
	(sta->sta_stats.last_rx_beacon_pkts)

#define sta_rx_probereq_pkts(sta) \
	(sta->sta_stats.rx_probereq_pkts)

#define sta_last_rx_probereq_pkts(sta) \
	(sta->sta_stats.last_rx_probereq_pkts)

#define sta_rx_probersp_pkts(sta) \
	(sta->sta_stats.rx_probersp_pkts)

#define sta_last_rx_probersp_pkts(sta) \
	(sta->sta_stats.last_rx_probersp_pkts)

#define sta_rx_probersp_bm_pkts(sta) \
	(sta->sta_stats.rx_probersp_bm_pkts)

#define sta_last_rx_probersp_bm_pkts(sta) \
	(sta->sta_stats.last_rx_probersp_bm_pkts)

#define sta_rx_probersp_uo_pkts(sta) \
	(sta->sta_stats.rx_probersp_uo_pkts)

#define sta_last_rx_probersp_uo_pkts(sta) \
	(sta->sta_stats.last_rx_probersp_uo_pkts)

#define sta_update_last_rx_pkts(sta) \
	do { \
		sta->sta_stats.last_rx_mgnt_pkts = sta->sta_stats.rx_mgnt_pkts; \
		sta->sta_stats.last_rx_beacon_pkts = sta->sta_stats.rx_beacon_pkts; \
		sta->sta_stats.last_rx_probereq_pkts = sta->sta_stats.rx_probereq_pkts; \
		sta->sta_stats.last_rx_probersp_pkts = sta->sta_stats.rx_probersp_pkts; \
		sta->sta_stats.last_rx_probersp_bm_pkts = sta->sta_stats.rx_probersp_bm_pkts; \
		sta->sta_stats.last_rx_probersp_uo_pkts = sta->sta_stats.rx_probersp_uo_pkts; \
		sta->sta_stats.last_rx_ctrl_pkts = sta->sta_stats.rx_ctrl_pkts; \
		sta->sta_stats.last_rx_data_pkts = sta->sta_stats.rx_data_pkts; \
	} while(0)

#define STA_RX_PKTS_ARG(sta) \
	sta->sta_stats.rx_mgnt_pkts \
	, sta->sta_stats.rx_ctrl_pkts \
	, sta->sta_stats.rx_data_pkts

#define STA_LAST_RX_PKTS_ARG(sta) \
	sta->sta_stats.last_rx_mgnt_pkts \
	, sta->sta_stats.last_rx_ctrl_pkts \
	, sta->sta_stats.last_rx_data_pkts

#define STA_RX_PKTS_DIFF_ARG(sta) \
	sta->sta_stats.rx_mgnt_pkts - sta->sta_stats.last_rx_mgnt_pkts \
	, sta->sta_stats.rx_ctrl_pkts - sta->sta_stats.last_rx_ctrl_pkts \
	, sta->sta_stats.rx_data_pkts -sta->sta_stats.last_rx_data_pkts

#define STA_PKTS_FMT "(m:%llu, c:%llu, d:%llu)"

struct	sta_priv {

	uint8_t *pallocated_stainfo_buf;
	uint8_t *pstainfo_buf;
	struct __queue	free_sta_queue;

	spinlock_t sta_hash_lock;
	struct list_head   sta_hash[NUM_STA];
	int asoc_sta_count;
	struct __queue sleep_q;
	struct __queue wakeup_q;

	struct rtl_priv *rtlpriv;


#ifdef CONFIG_AP_MODE
	struct list_head asoc_list;
	struct list_head auth_list;
	spinlock_t asoc_list_lock;
	spinlock_t auth_list_lock;
	uint8_t asoc_list_cnt;
	uint8_t auth_list_cnt;

	unsigned int auth_to;  //sec, time to expire in authenticating.
	unsigned int assoc_to; //sec, time to expire before associating.
	unsigned int expire_to; //sec , time to expire after associated.

	/* pointers to STA info; based on allocated AID or NULL if AID free
	 * AID is in the range 1-2007, so sta_aid[0] corresponders to AID 1
	 * and so on
	 */
	struct sta_info *sta_aid[NUM_STA];

	u16 sta_dz_bitmap;//only support 15 stations, staion aid bitmap for sleeping sta.
	u16 tim_bitmap;//only support 15 stations, aid=0~15 mapping bit0~bit15

	u16 max_num_sta;

	struct wlan_acl_pool acl_list;
#endif

};


__inline static u32 wifi_mac_hash(uint8_t *mac)
{
        u32 x;

        x = mac[0];
        x = (x << 2) ^ mac[1];
        x = (x << 2) ^ mac[2];
        x = (x << 2) ^ mac[3];
        x = (x << 2) ^ mac[4];
        x = (x << 2) ^ mac[5];

        x ^= x >> 8;
        x  = x & (NUM_STA - 1);

        return x;
}


extern u32	_rtw_init_sta_priv(struct sta_priv *pstapriv);
extern u32	_rtw_free_sta_priv(struct sta_priv *pstapriv);

#define stainfo_offset_valid(offset) (offset < NUM_STA && offset >= 0)
int rtw_stainfo_offset(struct sta_priv *stapriv, struct sta_info *sta);
struct sta_info *rtw_get_stainfo_by_offset(struct sta_priv *stapriv, int offset);

extern struct sta_info *rtw_alloc_stainfo(struct	sta_priv *pstapriv, uint8_t *hwaddr);
extern u32	rtw_free_stainfo(struct rtl_priv *rtlpriv , struct sta_info *psta);
extern void rtw_free_all_stainfo(struct rtl_priv *rtlpriv);
extern struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, uint8_t *hwaddr);
extern u32 rtw_init_bcmc_stainfo(struct rtl_priv* rtlpriv);
extern struct sta_info* rtw_get_bcmc_stainfo(struct rtl_priv* rtlpriv);
extern uint8_t rtw_access_ctrl(struct rtl_priv *rtlpriv, uint8_t *mac_addr);

#endif //_STA_INFO_H_

