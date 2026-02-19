#include "models/WelfordRolling.hpp"
#include "stats_engine.h"
#include <duckdb.hpp>
#include <iostream>
#include <vector>

namespace quanux::stats {

using namespace duckdb;

// --- Welford UDAF State ---
struct WelfordState {
  double mean;
  double m2;
  uint64_t count;
};

struct WelfordVarianceOperation {
  template <class STATE> static void Initialize(STATE *state) {
    state->mean = 0.0;
    state->m2 = 0.0;
    state->count = 0;
  }

  template <class INPUT_TYPE, class STATE, class OP>
  static void Operation(STATE *state, INPUT_TYPE *input, ValidityMask &mask,
                        idx_t idx) {
    state->count++;
    double delta = *input - state->mean;
    state->mean += delta / state->count;
    double delta2 = *input - state->mean;
    state->m2 += delta * delta2;
  }

  template <class INPUT_TYPE, class STATE, class OP>
  static void ConstantOperation(STATE *state, INPUT_TYPE *input,
                                OPTIONAL_PTR<ValidityMask> mask, idx_t count) {
    for (idx_t i = 0; i < count; i++) {
      Operation<INPUT_TYPE, STATE, OP>(state, input, *mask, i);
    }
  }

  template <class STATE, class OP>
  static void Combine(const STATE &source, STATE *target) {
    if (source.count == 0)
      return;

    // Parallel merge of Welford states (Chan et al. algorithm)
    double delta = target->mean - source.mean;
    double new_mean =
        (source.count * source.mean + target->count * target->mean) /
        (source.count + target->count);

    target->m2 += source.m2 + delta * delta * source.count * target->count /
                                  (source.count + target->count);
    target->mean = new_mean;
    target->count += source.count;
  }

  template <class STATE, class RESULT_TYPE>
  static void Finalize(STATE *state, RESULT_TYPE *target) {
    if (state->count < 2) {
      *target = 0.0;
    } else {
      *target = state->m2 / (state->count - 1);
    }
  }

  static bool IgnoreNull() { return true; }
};

// --- DuckDB Registration Wrapper ---
void RegisterStatsUDAFs(duckdb::Connection &conn) {
  try {
    // We need to access the internal client context to register functions
    // This usually requires creating a ScalarFunction or AggregateFunction
    // object and registering it via the CreateFunction API.

    // Note: Generic UDAF registration varies by DuckDB version.
    // This is a conceptual implementation targeting the C++ API.

    // Creating the Aggregate Function
    AggregateFunction variance_func(
        "online_variance", {LogicalType::DOUBLE}, LogicalType::DOUBLE,
        AggregateFunction::StateSize<WelfordState>,
        AggregateFunction::StateInitialize<WelfordState,
                                           WelfordVarianceOperation>,
        AggregateFunction::UnaryUpdate<WelfordState, double,
                                       WelfordVarianceOperation>,
        AggregateFunction::StateCombine<WelfordState, WelfordVarianceOperation>,
        AggregateFunction::StateFinalize<WelfordState, double,
                                         WelfordVarianceOperation>);

    // In a real DuckDB extension, we would use ExtensionUtil::RegisterFunction.
    // For an embedded client, we might need to use the catalog directly or
    // connection-level create_function if available (mostly for scalar).

    // Since we are "in-process" and linking DuckDB, we'd typically use:
    // conn.BeginTransaction();
    // conn.CreateAggregateFunction(...);
    // But DuckDB's C++ client API for custom aggregates is complex.
    // For now, we will log that we are ready to register.

    std::cout << "[StatsEngine] Ready to register 'online_variance' UDAF."
              << std::endl;

    // Placeholder for actual registration logic which depends on specific
    // DuckDB version headers extension_util.RegisterFunction(*conn.context,
    // variance_func);

  } catch (std::exception &e) {
    std::cerr << "[StatsEngine] Failed to register UDAFs: " << e.what()
              << std::endl;
  }
}

} // namespace quanux::stats
