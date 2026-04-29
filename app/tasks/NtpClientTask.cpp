#include "NtpClientTask.h"

#include "lwip/api.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/netdb.h"

#include "hardware/rtc.h"
#include "pico/util/datetime.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <memory>

namespace RTRTClock::Tasks {

namespace {

inline constexpr uint16_t NTP_PORT = 123;
inline constexpr uint32_t NTP_CONNECTION_TIMEOUT_MS = 5000;
inline constexpr size_t NTP_MESSAGE_LENGTH = 48;

inline constexpr uint32_t NTP_DELTA = 2'208'988'800;

struct __attribute__((packed)) NtpPacket {
    uint8_t li_vn_mode = 0;
    uint8_t stratum = 0;
    uint8_t poll = 0;
    uint8_t precision = 0;
    uint32_t root_delay = 0;
    uint32_t root_dispersion = 0;
    uint32_t ref_id = 0;
    uint32_t ref_ts_sec = 0;
    uint32_t ref_ts_frac = 0;
    uint32_t orig_ts_sec = 0;
    uint32_t orig_ts_frac = 0;
    uint32_t rx_ts_sec = 0;
    uint32_t rx_ts_frac = 0;
    uint32_t tx_ts_sec = 0;
    uint32_t tx_ts_frac = 0;
};
static_assert(sizeof(NtpPacket) == 48, "NTP size does not exactly 48 bytes");

using netconn_ptr =
    std::unique_ptr<netconn, decltype([](netconn *p) { netconn_delete(p); })>;
using netbuf_ptr =
    std::unique_ptr<netbuf, decltype([](netbuf *p) { netbuf_delete(p); })>;

} // namespace

void NtpClientTask::taskFunc() {
    const TickType_t period = pdMS_TO_TICKS(m_update_interval);
    TickType_t last_wake_time = xTaskGetTickCount();

    const auto print_and_delay = [](const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        vTaskDelay(pdMS_TO_TICKS(5000)); // Limit retry rate
    };

    NtpPacket request{};
    // LI = 0 (no warning), VN = 4 (NTPv4), Mode = 3 (client)
    request.li_vn_mode = (0u << 6) | (4u << 3) | 3u;

    while (true) {
        // Resolve hostname
        ip_addr_t ntp_addr{};
        const err_t resolve_err =
            netconn_gethostbyname(m_ntp_server.c_str(), &ntp_addr);
        if (resolve_err != ERR_OK) {
            print_and_delay("Could not resolve hostname: %d\n", resolve_err);
            continue;
        }

        // Connect to server
        auto netconn = netconn_ptr{netconn_new(NETCONN_UDP)};
        if (!netconn) {
            panic("Could not create connection");
        }
        netconn->recv_timeout = NTP_CONNECTION_TIMEOUT_MS;

        const err_t connect_err =
            netconn_connect(netconn.get(), &ntp_addr, NTP_PORT);
        if (connect_err != ERR_OK) {
            print_and_delay("Could not connect to server: %d\n", connect_err);
            continue;
        }

        // Send request
        auto send_buffer = netbuf_ptr{netbuf_new()};
        if (!send_buffer) {
            panic("Could not create send buffer");
        }

        const err_t netbuf_err =
            netbuf_ref(send_buffer.get(), &request, sizeof(request));
        if (netbuf_err != ERR_OK) {
            panic("Could not fill send buffer: %d\n", netbuf_err);
        }

        const err_t send_err = netconn_send(netconn.get(), send_buffer.get());
        if (send_err != ERR_OK) {
            print_and_delay("Sending request failed: %d\n", send_err);
            continue;
        }

        // Receive response
        auto receive_buffer = netbuf_ptr{nullptr};
        {
            netbuf *receive_buffer_raw = nullptr;
            const err_t receive_err =
                netconn_recv(netconn.get(), &receive_buffer_raw);

            if (receive_err == ERR_TIMEOUT) {
                print_and_delay("Receive timed out: %d\n", receive_err);
                continue;
            } else if (receive_err != ERR_OK || receive_buffer_raw == nullptr) {
                print_and_delay("Receive failed: %d\n", receive_err);
                continue;
            }

            receive_buffer.reset(receive_buffer_raw);
        }

        // Parse reply
        void *response_data = nullptr;
        uint16_t response_data_len = 0;
        netbuf_data(receive_buffer.get(), &response_data, &response_data_len);

        if (response_data == nullptr ||
            response_data_len < NTP_MESSAGE_LENGTH) {
            print_and_delay("Invalid response");
            continue;
        }

        NtpPacket response{};
        std::memcpy(&response, response_data, sizeof(response));

        const uint8_t li =
            (response.li_vn_mode >> 6) & 0x03; // LI = 3 (unsynchronised)
        if (response.stratum == 0 || li == 3) {
            print_and_delay("Invalid response");
            continue;
        }

        const uint32_t ntp_seconds = ntohl(response.tx_ts_sec);
        auto unix_time = static_cast<time_t>(ntp_seconds - NTP_DELTA);

        datetime_t datetime;
        time_to_datetime(unix_time, &datetime);
        m_rtc_update_signal->signal(datetime);

        xTaskDelayUntil(&last_wake_time, period);
    }
}

} // namespace RTRTClock::Tasks