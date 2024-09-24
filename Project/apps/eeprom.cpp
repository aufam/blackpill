#include <boost/preprocessor.hpp>
#include <fmt/ranges.h>
#include <apps/app.h>
#include <apps/terminal.h>
#include <apps/ip_config.h>
#include <delameta/http/http.h>
#include <stm32f4xx_hal.h>

using namespace Project;
namespace http = delameta::http;

#define FLASH_LAST_SECTOR FLASH_SECTOR_7

FMT_DECLARE(
    (EEPROM)
    ,
    (mac_t, mac)
    (ip_t , ip )
    (ip_t , sn )
    (ip_t , gw )
    (ip_t , dns)
)

JSON_TRAITS(
    (EEPROM)
    ,
    (mac_t, mac)
    (ip_t , ip )
    (ip_t , sn )
    (ip_t , gw )
    (ip_t , dns)
)

const __IO EEPROM eeprom __attribute__((section(".eeprom"))) = {
    .mac = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
    .ip  = {10, 20, 30, 2},
    .sn  = {255, 255, 255, 0},
    .gw  = {10, 20, 30, 1},
    .dns = {10, 20, 30, 1},
};

EEPROM eeprom_read() {
    EEPROM ee;
    ::memcpy(&ee, const_cast<const EEPROM*>(&eeprom), sizeof(EEPROM));
    return ee;
}

uint32_t eeprom_write(EEPROM ee) {
    FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;
    HAL_FLASH_Unlock();

	/* Unlock the Flash to enable the flash control register access *************/
	::memset(&EraseInitStruct,0,sizeof(EraseInitStruct));
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR  | FLASH_FLAG_WRPERR |
						   FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR );

	/* Erase the user Flash area*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = FLASH_LAST_SECTOR;
	EraseInitStruct.NbSectors = 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
		/*Error occurred while page erase.*/
        HAL_FLASH_Lock();
		return HAL_FLASH_GetError ();
	}

	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	     you have to make sure that these data are rewritten before they are accessed during code
	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	     DCRST and ICRST bits in the FLASH_CR register.*/
	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();

	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();

    auto data = reinterpret_cast<const uint8_t*>(&ee);
    auto len  = sizeof(EEPROM);
    uint32_t address = (uint32_t)&eeprom;

    for (size_t i = 0; i < len; ++i, ++address) {
        auto res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address, data[i]);
        if (res != HAL_OK) {
            HAL_FLASH_Lock();
            return res;
        }
    }

    HAL_FLASH_Lock();
    return 0;
}

mac_t eeprom_read_mac() {
    mac_t res;
    ::memcpy(&res, const_cast<const mac_t*>(&eeprom.mac), 6);
    return res;
}

void eeprom_write_mac(mac_t mac) {
    EEPROM ee = eeprom_read();
    ee.mac = mac;
    eeprom_write(ee);
}

ip_t eeprom_read_ip() {
    ip_t res;
    ::memcpy(&res, const_cast<const ip_t*>(&eeprom.ip), 4);
    return res;
}

void eeprom_write_ip(ip_t ip) {
    EEPROM ee = eeprom_read();
    ee.ip = ip;
    eeprom_write(ee);
}

ip_t eeprom_read_sn() {
    ip_t res;
    ::memcpy(&res, const_cast<const ip_t*>(&eeprom.sn), 4);
    return res;
}

void eeprom_write_sn(ip_t sn) {
    EEPROM ee = eeprom_read();
    ee.sn = sn;
    eeprom_write(ee);
}

ip_t eeprom_read_gw() {
    ip_t res;
    ::memcpy(&res, const_cast<const ip_t*>(&eeprom.gw), 4);
    return res;
}

void eeprom_write_gw(ip_t gw) {
    EEPROM ee = eeprom_read();
    ee.gw = gw;
    eeprom_write(ee);
}

ip_t eeprom_read_dns() {
    ip_t res;
    ::memcpy(&res, const_cast<const ip_t*>(&eeprom.dns), 4);
    return res;
}

void eeprom_write_dns(ip_t dns) {
    EEPROM ee = eeprom_read();
    ee.dns = dns;
    eeprom_write(ee);
}

extern Terminal terminal;
extern http::Http http_server;

template <> etl::Result<EEPROM, const char*>
Project::Terminal::deserialize_arg(etl::StringView sv) {
    auto j = etl::Json::parse(sv);
    if (!j) {
        return etl::Err(j.error_message().data());
    }

    auto ee = eeprom_read();
    auto mac = j["mac"];
    if (mac) {
        auto res = etl::json::deserialize<mac_t>(mac, ee.mac);
        if (res.is_err()) {
            return etl::Err(res.unwrap_err());
        }
    }

    auto ip = j["ip"];
    if (ip) {
        auto res = etl::json::deserialize<ip_t>(ip, ee.ip);
        if (res.is_err()) {
            return etl::Err(res.unwrap_err());
        }
    }

    auto sn = j["sn"];
    if (sn) {
        auto res = etl::json::deserialize<ip_t>(sn, ee.sn);
        if (res.is_err()) {
            return etl::Err(res.unwrap_err());
        }
    }

    auto gw = j["gw"];
    if (gw) {
        auto res = etl::json::deserialize<ip_t>(gw, ee.gw);
        if (res.is_err()) {
            return etl::Err(res.unwrap_err());
        }
    }

    auto dns = j["dns"];
    if (dns) {
        auto res = etl::json::deserialize<ip_t>(dns, ee.dns);
        if (res.is_err()) {
            return etl::Err(res.unwrap_err());
        }
    }

    return etl::Ok(ee);
}

APP(flash) {
    terminal.route("eeprom_read", &eeprom_read);
    http_server.Get("/eeprom_read", {}, &eeprom_read);

    terminal.route("eeprom_write", &eeprom_write);
    http_server.Put("/eeprom_write", std::tuple{
        http::arg::json_item_default_val("mac", std::nullopt),
        http::arg::json_item_default_val("ip ", std::nullopt),
        http::arg::json_item_default_val("sn ", std::nullopt),
        http::arg::json_item_default_val("gw ", std::nullopt),
        http::arg::json_item_default_val("dns", std::nullopt),
    }, [](
        std::optional<mac_t> mac,
        std::optional<ip_t>  ip,
        std::optional<ip_t>  sn,
        std::optional<ip_t>  gw,
        std::optional<ip_t>  dns
    ) -> http::Result<void> {
        auto ee = eeprom_read();
        int cnt = 0;
        if (mac) {
            ee.mac = *mac;
            cnt++;
        }
        if (ip) {
            ee.ip  = *ip;
            cnt++;
        }
        if (sn) {
            ee.sn  = *sn;
            cnt++;
        }
        if (gw) {
            ee.gw  = *gw;
            cnt++;
        }
        if (dns) {
            ee.dns = *dns;
            cnt++;
        }

        if (cnt > 0) {
            eeprom_write(ee);
            return etl::Ok();
        } else {
            return etl::Err(http::Error{http::StatusBadRequest, "Invalid JSON field"});
        }
    });
}
