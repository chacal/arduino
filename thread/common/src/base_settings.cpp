#include <optional>
#include <nrf_log.h>
#include <chrono>
#include "base_settings.hpp"
#include "thread.hpp"

namespace settings {
  base_settings base_settings_from_json(const JsonDocument &doc) {
    base_settings s{};

    if (doc.containsKey("instance")) {
      if (doc["instance"].is<char *>()) {
        s.instance = doc["instance"].as<char *>();
      } else {
        NRF_LOG_ERROR("Instance must be a string!");
      }
    }

    if (doc.containsKey("txPower")) {
      if (doc["txPower"].is<int8_t>()) {
        s.txPower = doc["txPower"];
      } else {
        NRF_LOG_ERROR("Tx power must be an integer!");
      }
    }

    if (doc.containsKey("pollPeriod")) {
      if (doc["pollPeriod"].is<uint32_t>()) {
        s.pollPeriod = milliseconds(doc["pollPeriod"]);
      } else {
        NRF_LOG_ERROR("Poll period must be an integer!");
      }
    }

    if (doc.containsKey("increasedPollPeriod")) {
      if (doc["increasedPollPeriod"].is<uint32_t>()) {
        s.increasedPollPeriod = milliseconds(doc["increasedPollPeriod"]);
      } else {
        NRF_LOG_ERROR("Increased poll period must be an integer!");
      }
    }

    if (doc.containsKey("increasedPollDuration")) {
      if (doc["increasedPollDuration"].is<uint32_t>()) {
        s.increasedPollDuration = milliseconds(doc["increasedPollDuration"]);
      } else {
        NRF_LOG_ERROR("Increased poll duration must be an integer!");
      }
    }

    return s;
  }

  void write_thread_settings_to_json(JsonDocument &doc) {
    doc["txPower"]               = thread::get_tx_power();
    doc["pollPeriod"]            = thread::get_normal_poll_period().count();
    doc["increasedPollPeriod"]   = thread::get_increased_poll_period().count();
    doc["increasedPollDuration"] = thread::get_increased_poll_duration().count();
  }

  void set_thread_settings(const std::optional<base_settings> &s) {
    if (s->pollPeriod) {
      thread::set_tx_power(*s->txPower);
    }

    if (s->pollPeriod) {
      thread::set_normal_poll_period(milliseconds(*s->pollPeriod));
    }

    if (s->increasedPollPeriod) {
      thread::set_increased_poll_period(milliseconds(*s->increasedPollPeriod));
    }

    if (s->increasedPollDuration) {
      thread::set_increased_poll_duration(milliseconds(*s->increasedPollDuration));
    }
  }
}