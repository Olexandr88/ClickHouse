#pragma once

#include <Interpreters/SystemLog.h>
#include <Common/ProfileEvents.h>
#include <Common/CurrentMetrics.h>
#include <Common/ThreadPool_fwd.h>
#include <Core/NamesAndTypes.h>
#include <Core/NamesAndAliases.h>
#include <Storages/ColumnsDescription.h>

#include <vector>
#include <atomic>
#include <ctime>


namespace DB
{

/** TransposedMetricLog is a log of metric values measured at regular time interval.
  */

struct TransposedMetricLogElement
{
    UInt16 event_date;
    time_t event_time{};
    std::string metric_name;
    Int64 value;

    static std::string name() { return "TransposedMetricLog"; }
    static ColumnsDescription getColumnsDescription();
    static NamesAndAliases getNamesAndAliases() { return {}; }
    void appendToBlock(MutableColumns & columns) const;
};


class TransposedMetricLog : public SystemLog<TransposedMetricLogElement>
{
    using SystemLog<TransposedMetricLogElement>::SystemLog;

public:
    void shutdown() override;

    /// Launches a background thread to collect metrics with interval
    void startCollectMetric(size_t collect_interval_milliseconds_);

    /// Stop background thread. Call before shutdown.
    void stopCollectMetric();

private:
    void metricThreadFunction();

    std::unique_ptr<ThreadFromGlobalPool> metric_flush_thread;
    size_t collect_interval_milliseconds;
    std::atomic<bool> is_shutdown_metric_thread{false};
};

}
