include $(TOPDIR)/rules.mk

PKG_NAME:=openvpnmanagement
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/openvpnmanagement
	CATEGORY:=Base system
	TITLE:=openvpnmanagement
	DEPENDS:= +libubus +libubox +libblobmsg-json +libuci
endef

define Package/openvpnmanagement/description
	This is an example app package which uses an example library package
endef

define Package/openvpnmanagement/install
	$(INSTALL_DIR) $(1)/usr/bin  
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/openvpnmanagement $(1)/usr/bin
	$(INSTALL_BIN) ./files/openvpn.init $(1)/etc/init.d/openvpn
endef

$(eval $(call BuildPackage,openvpnmanagement))
